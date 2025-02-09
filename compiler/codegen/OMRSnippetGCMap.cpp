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

#include <stdint.h>
#include "codegen/GCStackAtlas.hpp"
#include "codegen/SnippetGCMap.hpp"
#include "codegen/CodeGenerator.hpp"
#include "codegen/Instruction.hpp"


void
OMR::SnippetGCMap::registerStackMap(
      TR::Instruction *instruction,
      TR::CodeGenerator *cg)
   {
   if (_stackMap)
      {
      _stackMap->addToAtlas(instruction, cg);
      }
   }


void
OMR::SnippetGCMap::registerStackMap(
      uint8_t *callSiteAddress,
      TR::CodeGenerator *cg)
   {
   if (_stackMap)
      {
      _stackMap->addToAtlas(callSiteAddress, cg);
      }
   }
