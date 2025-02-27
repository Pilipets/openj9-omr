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

#if !defined(MM_STARTUPMANAGERIMPL_HPP_)
#define MM_STARTUPMANAGERIMPL_HPP_

#include "StartupManager.hpp"

class MM_CollectorLanguageInterface;
class MM_MarkingScheme;
class MM_VerboseManagerBase;

struct OMR_VM;

class MM_StartupManagerImpl : public MM_StartupManager
{
	/*
	 * Data members
	 */
private:

protected:
#if defined(OMR_GC_SEGREGATED_HEAP)
	bool _useSegregatedGC;
#endif /* defined(OMR_GC_SEGREGATED_HEAP) */
public:
	static const uintptr_t defaultMinimumHeapSize = (uintptr_t) 1*1024*1024;
	static const uintptr_t defaultMaximumHeapSize = (uintptr_t) 2*1024*1024;

	/*
	 * Function members
	 */

private:

protected:
	virtual bool handleOption(MM_GCExtensionsBase *extensions, char *option);
	virtual char * getOptions(void);

public:
	virtual MM_Configuration *createConfiguration(MM_EnvironmentBase *env);
	virtual MM_CollectorLanguageInterface * createCollectorLanguageInterface(MM_EnvironmentBase *env);
	virtual MM_VerboseManagerBase * createVerboseManager(MM_EnvironmentBase* env);

	MM_StartupManagerImpl(OMR_VM *omrVM)
		: MM_StartupManager(omrVM, defaultMinimumHeapSize, defaultMaximumHeapSize)
#if defined(OMR_GC_SEGREGATED_HEAP)
		, _useSegregatedGC(false)
#endif /* defined(OMR_GC_SEGREGATED_HEAP) */
	{
	}
};

#endif /* MM_STARTUPMANAGERIMPL_HPP_ */
