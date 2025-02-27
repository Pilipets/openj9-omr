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

#ifndef S390CALLSNIPPET_INCL
#define S390CALLSNIPPET_INCL

#include <stddef.h>
#include <stdint.h>
#include "codegen/InstOpCode.hpp"
#include "il/DataTypes.hpp"
#include "runtime/Runtime.hpp"
#include "codegen/Snippet.hpp"

#include "z/codegen/S390Instruction.hpp"

namespace TR { class CodeGenerator; }
namespace TR { class Instruction; }
namespace TR { class LabelSymbol; }
namespace TR { class MethodSymbol; }
namespace TR { class Node; }
namespace TR { class RealRegister; }
namespace TR { class SymbolReference; }

namespace TR {

class S390CallSnippet : public TR::Snippet
   {
   int32_t  sizeOfArguments;
   TR::Instruction *branchInstruction;

   protected:
   TR::SymbolReference * _realMethodSymbolReference;
   public:

   S390CallSnippet(TR::CodeGenerator *cg, TR::Node *c, TR::LabelSymbol *lab, int32_t s)
      : TR::Snippet(cg, c, lab, false), sizeOfArguments(s),branchInstruction(NULL), _realMethodSymbolReference(NULL)
      {
      }

   S390CallSnippet(TR::CodeGenerator *cg, TR::Node *c, TR::LabelSymbol *lab, TR::SymbolReference *symRef, int32_t s)
      : TR::Snippet(cg, c, lab, false), sizeOfArguments(s),branchInstruction(NULL), _realMethodSymbolReference(symRef)
      {
      }

   virtual Kind getKind() { return IsCall; }

   /** Get call Return Address */
   virtual uint8_t *getCallRA();

   virtual uint8_t *emitSnippetBody();

   virtual uint32_t getLength(int32_t estimatedSnippetStart);

   int32_t getSizeOfArguments()          {return sizeOfArguments;}
   int32_t setSizeOfArguments(int32_t s) {return sizeOfArguments = s;}

   TR::Instruction *setBranchInstruction(TR::Instruction *i) {return branchInstruction=i;}
   TR::Instruction *getBranchInstruction() {return branchInstruction;}

   TR::SymbolReference *setRealMethodSymbolReference(TR::SymbolReference *s) {return _realMethodSymbolReference = s;}
   TR::SymbolReference *getRealMethodSymbolReference() {return _realMethodSymbolReference;}

   static uint8_t *storeArgumentItem(TR::InstOpCode::Mnemonic op, uint8_t *buffer, TR::RealRegister *reg, int32_t offset, TR::CodeGenerator *cg);
   static uint8_t *S390flushArgumentsToStack(uint8_t *buffer, TR::Node *callNode, int32_t argSize, TR::CodeGenerator *cg);
   static int32_t instructionCountForArguments(TR::Node *callNode, TR::CodeGenerator *cg);

   /**
    * @brief For a given call from snippet (Usually helper calls) calculate the
    * offset needed for RIL type instructions and if the call is not reachable,
    * create trampoline.
    *
    * @param targetAddr : address of the target function for call
    * @param currentInst : bufferAddress where call instruction will be encoded
    *
    * @return return number of halfwords required to add to the currentInst to
    *         get the targetAddress
    */
   static int32_t adjustCallOffsetWithTrampoline(uintptr_t targetAddr, uint8_t *currentInst, TR::SymbolReference *callSymRef, TR::Snippet *snippet);

   };

}

#endif
