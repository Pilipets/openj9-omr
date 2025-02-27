/*******************************************************************************
 * Copyright IBM Corp. and others 2015
 *
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License 2.0 which accompanies this
 * distribution and is available at http://eclipse.org/legal/epl-2.0
 * or the Apache License, Version 2.0 which accompanies this distribution
 * and is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 * This Source Code may also be made available under the following Secondary
 * Licenses when the conditions for such availability set forth in the
 * Eclipse Public License, v. 2.0 are satisfied: GNU General Public License,
 * version 2 with the GNU Classpath Exception [1] and GNU General Public
 * License, version 2 with the OpenJDK Assembly Exception [2].
 *
 * [1] https://www.gnu.org/software/classpath/license.html
 * [2] https://openjdk.org/legal/assembly-exception.html
 *
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0-only WITH Classpath-exception-2.0 OR GPL-2.0-only WITH OpenJDK-assembly-exception-1.0
 *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "omrTest.h"
#include "thread_api.h"
#include "threadTestHelp.h"
#include "testHelper.hpp"

extern ThreadTestEnvironment *omrTestEnv;

typedef struct JoinThreadHelperData {
	omrthread_t threadToJoin;
	intptr_t expectedRc;
} JoinThreadHelperData;

static void createThread(omrthread_t *newThread, uintptr_t suspend, omrthread_detachstate_t detachstate,
						 omrthread_entrypoint_t entryProc, void *entryArg);
static int J9THREAD_PROC doNothingHelper(void *entryArg);
static int J9THREAD_PROC joinThreadHelper(void *entryArg);

TEST(JoinTest, joinSelf)
{
	ASSERT_EQ(J9THREAD_INVALID_ARGUMENT, omrthread_join(omrthread_self()));
}

TEST(JoinTest, joinNull)
{
	ASSERT_EQ(J9THREAD_INVALID_ARGUMENT, omrthread_join(NULL));
}

TEST(JoinTest, createDetachedThread)
{
	omrthread_t helperThr = NULL;

	/* We can't pass any local data to the child thread because we don't guarantee that
	 * it won't go out of scope before the child thread uses it.
	 */
	ASSERT_NO_FATAL_FAILURE(createThread(&helperThr, FALSE, J9THREAD_CREATE_DETACHED, doNothingHelper, NULL));
}

TEST(JoinTest, createJoinableThread)
{
	JoinThreadHelperData helperData;
	helperData.threadToJoin = NULL;
	helperData.expectedRc = J9THREAD_INVALID_ARGUMENT;

	omrthread_t helperThr = NULL;
	ASSERT_NO_FATAL_FAILURE(createThread(&helperThr, FALSE, J9THREAD_CREATE_JOINABLE, joinThreadHelper, (void *)&helperData));

	/* .. and joins the new thread */
	VERBOSE_JOIN(helperThr, J9THREAD_SUCCESS);
}

TEST(JoinTest, joinDetachedThread)
{
	JoinThreadHelperData helperData;

	/* we can guarantee this thread is live, so we won't crash in omrthread_join() */
	helperData.threadToJoin = omrthread_self();
	helperData.expectedRc = J9THREAD_INVALID_ARGUMENT;

	omrthread_t helperThr = NULL;
	ASSERT_NO_FATAL_FAILURE(createThread(&helperThr, FALSE, J9THREAD_CREATE_JOINABLE, joinThreadHelper, (void *)&helperData));

	VERBOSE_JOIN(helperThr, J9THREAD_SUCCESS);
}

TEST(JoinTest, joinExitedThread)
{
	omrthread_t helperThr = NULL;

	ASSERT_NO_FATAL_FAILURE(createThread(&helperThr, FALSE, J9THREAD_CREATE_JOINABLE, doNothingHelper, NULL));

	/* hopefully wait long enough for the child thread to exit */
	ASSERT_EQ(J9THREAD_SUCCESS, omrthread_sleep(3000));

	VERBOSE_JOIN(helperThr, J9THREAD_SUCCESS);
}

TEST(JoinTest, joinLiveThread)
{
	omrthread_t suspendedThr = NULL;
	omrthread_t joinHelperThr = NULL;
	JoinThreadHelperData helperData;

	/* suspend the child thread when it starts */
	ASSERT_NO_FATAL_FAILURE(createThread(&suspendedThr, TRUE, J9THREAD_CREATE_JOINABLE, doNothingHelper, NULL));

	/* create a thread to join on the suspended thread */
	helperData.threadToJoin = suspendedThr;
	helperData.expectedRc = J9THREAD_SUCCESS;
	ASSERT_NO_FATAL_FAILURE(createThread(&joinHelperThr, FALSE, J9THREAD_CREATE_JOINABLE, joinThreadHelper, &helperData));

	/* hopefully wait long enough for the join helper thread to start joining */
	ASSERT_EQ(J9THREAD_SUCCESS, omrthread_sleep(3000));

	/* resume the child thread */
	omrTestEnv->log("resuming suspendedThr\n");
	fflush(stdout);
	ASSERT_EQ(1, omrthread_resume(suspendedThr));

	VERBOSE_JOIN(joinHelperThr, J9THREAD_SUCCESS);
}

TEST(JoinTest, joinCanceledThread)
{
	omrthread_t threadToJoin = NULL;

	/* create a suspended thread */
	ASSERT_NO_FATAL_FAILURE(createThread(&threadToJoin, TRUE, J9THREAD_CREATE_JOINABLE, doNothingHelper, NULL));

	/* cancel the suspended thread */
	omrthread_cancel(threadToJoin);

	/* join the suspended + canceled thread */
	VERBOSE_JOIN(threadToJoin, J9THREAD_SUCCESS);
}

static void
createThread(omrthread_t *newThread, uintptr_t suspend, omrthread_detachstate_t detachstate,
			 omrthread_entrypoint_t entryProc, void *entryArg)
{
	omrthread_attr_t attr = NULL;
	intptr_t rc = 0;

	ASSERT_EQ(J9THREAD_SUCCESS, omrthread_attr_init(&attr));
	ASSERT_EQ(J9THREAD_SUCCESS, omrthread_attr_set_detachstate(&attr, detachstate));
	EXPECT_EQ(J9THREAD_SUCCESS,
			  rc = omrthread_create_ex(newThread, &attr, suspend, entryProc, entryArg));
	if (rc & J9THREAD_ERR_OS_ERRNO_SET) {
		omrTestEnv->log(LEVEL_ERROR, "omrthread_create_ex() returned os_errno=%d\n", (int)omrthread_get_os_errno());
	}
	ASSERT_EQ(J9THREAD_SUCCESS, omrthread_attr_destroy(&attr));
}

static int J9THREAD_PROC
doNothingHelper(void *entryArg)
{
	omrTestEnv->log("doNothingHelper() is exiting\n");
	fflush(stdout);
	return 0;
}

static int J9THREAD_PROC
joinThreadHelper(void *entryArg)
{
	JoinThreadHelperData *helperData = (JoinThreadHelperData *)entryArg;
	omrTestEnv->log("joinThreadHelper() is invoking omrthread_join()\n");
	fflush(stdout);
	VERBOSE_JOIN(helperData->threadToJoin, helperData->expectedRc);
	return 0;
}
