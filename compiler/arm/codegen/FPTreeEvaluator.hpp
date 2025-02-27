/*******************************************************************************
 * Copyright IBM Corp. and others 2000
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

#ifndef IA32FPTREEEVALUATOR_INCL
#define IA32FPTREEEVALUATOR_INCL

// FPCW values
//
#define SINGLE_PRECISION_ROUND_TO_ZERO      0x0c7f
#define DOUBLE_PRECISION_ROUND_TO_ZERO      0x0e7f
#define SINGLE_PRECISION_ROUND_TO_NEAREST   0x007f
#define DOUBLE_PRECISION_ROUND_TO_NEAREST   0x027f

// Binary representation of double 1.0
//
#ifndef _LONG_LONG
#define IEEE_DOUBLE_1_0 0x3ff0000000000000L
#else
#define IEEE_DOUBLE_1_0 0x3ff0000000000000LL
#endif

extern void insertPrecisionAdjustment(TR_Register *reg,
									  TR::Node     *node);
#endif

