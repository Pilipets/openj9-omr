/*******************************************************************************
 * Copyright IBM Corp. and others 2014
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

#include <string.h>
#include "omrTestHelpers.h"

omr_error_t
omrTestPrintError(const char *funcCall, const omr_error_t rc, OMRPortLibrary *portLibrary, const char *callFile, intptr_t callLine)
{
	if (OMR_ERROR_NONE != rc) {
		OMRPORT_ACCESS_FROM_OMRPORT(portLibrary);
		omrtty_printf("%s:%d %s failed, rc=%d (%s)\n", callFile, callLine, funcCall, rc, omrErrorToString(rc));
	}
	return rc;
}

omr_error_t
omrTestPrintUnexpectedIntRC(const char *funcCall, const intptr_t rc, const intptr_t expectedRC,
							OMRPortLibrary *portLibrary, const char *callFile, intptr_t callLine)
{
	omr_error_t omrRC = OMR_ERROR_NONE;
	if (expectedRC != rc) {
		OMRPORT_ACCESS_FROM_OMRPORT(portLibrary);
		omrtty_printf("%s:%d %s failed, got rc=%d, expected rc=%d\n",
					  callFile, callLine, funcCall, rc, expectedRC);
		omrRC = OMR_ERROR_INTERNAL;
	}
	return omrRC;
}

omr_error_t
omrTestPrintUnexpectedRC(const char *funcCall, const omr_error_t rc, const omr_error_t expectedRC,
						 OMRPortLibrary *portLibrary, const char *callFile, intptr_t callLine)
{
	if (expectedRC != rc) {
		OMRPORT_ACCESS_FROM_OMRPORT(portLibrary);
		omrtty_printf("%s:%d %s failed, got rc=%d (%s), expected rc=%d (%s)\n",
					  callFile, callLine, funcCall, rc, omrErrorToString(rc), expectedRC, omrErrorToString(expectedRC));
	}
	return rc;
}


#define RCSTRING(err) \
	case (err): rcstring = #err; break

const char *
omrErrorToString(omr_error_t rc)
{
	const char *rcstring = "";
	switch (rc) {
	RCSTRING(OMR_ERROR_NONE);
	RCSTRING(OMR_ERROR_OUT_OF_NATIVE_MEMORY);
	RCSTRING(OMR_ERROR_FAILED_TO_ATTACH_NATIVE_THREAD);
	RCSTRING(OMR_ERROR_MAXIMUM_VM_COUNT_EXCEEDED);
	RCSTRING(OMR_ERROR_MAXIMUM_THREAD_COUNT_EXCEEDED);
	RCSTRING(OMR_THREAD_STILL_ATTACHED);
	RCSTRING(OMR_VM_STILL_ATTACHED);
	RCSTRING(OMR_ERROR_FAILED_TO_ALLOCATE_MONITOR);
	RCSTRING(OMR_ERROR_INTERNAL);
	RCSTRING(OMR_ERROR_ILLEGAL_ARGUMENT);
	RCSTRING(OMR_ERROR_NOT_AVAILABLE);
	RCSTRING(OMR_THREAD_NOT_ATTACHED);
	RCSTRING(OMR_ERROR_FILE_UNAVAILABLE);
	RCSTRING(OMR_ERROR_RETRY);
	default:
		break;
	}
	return rcstring;
}

/* returns TRUE if the @ref s starts with @ref prefix */
BOOLEAN
strStartsWith(const char *s, const char *prefix)
{
	size_t sLen = strlen(s);
	size_t prefixLen = strlen(prefix);
	size_t i;

	if (sLen < prefixLen) {
		return FALSE;
	}
	for (i = 0; i < prefixLen; i++) {
		if (s[i] != prefix[i]) {
			return FALSE;
		}
	}
	return TRUE;
}

/**
 * Display a message to the console.
 *
 * Given a format string and it's arguments display the string to the console.
 *
 * @param[in] portLibrary The port library
 * @param[in] foramt Format of string to be output
 * @param[in] ... argument list for format string
 *
 * @note Does not increment number of failed tests.
 */
void
outputComment(struct OMRPortLibrary *portLibrary, const char *format, ...)
{
	OMRPORT_ACCESS_FROM_OMRPORT(portLibrary);
	va_list args;
	va_start(args, format);
	omrtty_printf("  ");
	omrtty_vprintf(format, args);
	va_end(args);
}
