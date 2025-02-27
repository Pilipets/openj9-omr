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

#include "codegen/Relocation.hpp"

#include <stddef.h>
#include <stdint.h>
#include "codegen/AheadOfTimeCompile.hpp"
#include "codegen/CodeGenerator.hpp"
#include "codegen/Instruction.hpp"
#include "compile/Compilation.hpp"
#include "control/Options.hpp"
#include "control/Options_inlines.hpp"
#include "env/CompilerEnv.hpp"
#include "env/IO.hpp"
#include "env/TRMemory.hpp"
#include "env/jittypes.h"
#include "il/LabelSymbol.hpp"
#include "infra/Assert.hpp"
#include "infra/Flags.hpp"
#include "infra/Link.hpp"
#include "runtime/Runtime.hpp"

void TR::Relocation::apply(TR::CodeGenerator *cg)
   {
   TR_ASSERT(0, "Should never get here");
   }

void TR::Relocation::trace(TR::Compilation* comp)
   {
   TR_ASSERT(0, "Should never get here");
   }

void TR::Relocation::setDebugInfo(TR::RelocationDebugInfo* info)
   {
   this->_genData= info;
   }

TR::RelocationDebugInfo* TR::Relocation::getDebugInfo()
   {
   return this->_genData;
   }

void TR::LabelRelocation::assertLabelDefined()
   {
   TR_ASSERT_FATAL(
      _label->getCodeLocation() != NULL,
      "cannot relocate reference to undefined label: %s (%p)\n",
      _label->getName(TR::comp()->getDebug()),
      _label);
   }

void TR::LabelRelative8BitRelocation::apply(TR::CodeGenerator *cg)
   {
   assertLabelDefined();
   cg->apply8BitLabelRelativeRelocation((int32_t *)getUpdateLocation(), getLabel());
   }

void TR::LabelRelative12BitRelocation::apply(TR::CodeGenerator *cg)
   {
   assertLabelDefined();
   cg->apply12BitLabelRelativeRelocation((int32_t *)getUpdateLocation(), getLabel(), isCheckDisp());
   }

void TR::LabelRelative16BitRelocation::apply(TR::CodeGenerator *cg)
   {
   assertLabelDefined();
   if (getAddressDifferenceDivisor() == 1)
      cg->apply16BitLabelRelativeRelocation((int32_t *)getUpdateLocation(), getLabel());
   else
      cg->apply16BitLabelRelativeRelocation((int32_t *)getUpdateLocation(), getLabel(), getAddressDifferenceDivisor(), isInstructionOffset());
   }

void TR::LabelRelative24BitRelocation::apply(TR::CodeGenerator *cg)
   {
   assertLabelDefined();
   cg->apply24BitLabelRelativeRelocation((int32_t *)getUpdateLocation(), getLabel());
   }

void TR::LabelRelative32BitRelocation::apply(TR::CodeGenerator *cg)
   {
   assertLabelDefined();
   cg->apply32BitLabelRelativeRelocation((int32_t *)getUpdateLocation(), getLabel());
   }

void TR::LabelAbsoluteRelocation::apply(TR::CodeGenerator *cg)
   {
   intptr_t *cursor = (intptr_t *)getUpdateLocation();
   assertLabelDefined();
   *cursor = (intptr_t)getLabel()->getCodeLocation();
   }

TR::InstructionLabelRelative16BitRelocation::InstructionLabelRelative16BitRelocation(TR::Instruction* cursor, int32_t offset, TR::LabelSymbol* l, int32_t divisor)
   :
      TR::LabelRelocation(NULL, l),
      _cursor(cursor),
      _offset(offset),
      _divisor(divisor)
   {
   }

uint8_t*
TR::InstructionLabelRelative16BitRelocation::getUpdateLocation()
   {
   uint8_t* updateLocation = TR::LabelRelocation::getUpdateLocation();

   if (updateLocation == NULL && _cursor->getBinaryEncoding() != NULL)
      {
      updateLocation = setUpdateLocation(_cursor->getBinaryEncoding() + _offset);
      }

   return updateLocation;
   }

void
TR::InstructionLabelRelative16BitRelocation::apply(TR::CodeGenerator* cg)
   {
   uint8_t* p = getUpdateLocation();
   assertLabelDefined();
   *reinterpret_cast<int16_t*>(p) = static_cast<int16_t>(getLabel()->getCodeLocation() - p) / _divisor;
   }

TR::InstructionLabelRelative32BitRelocation::InstructionLabelRelative32BitRelocation(TR::Instruction* cursor, int32_t offset, TR::LabelSymbol* l, int32_t divisor)
   :
      TR::LabelRelocation(NULL, l),
      _cursor(cursor),
      _offset(offset),
      _divisor(divisor)
   {
   }

uint8_t*
TR::InstructionLabelRelative32BitRelocation::getUpdateLocation()
   {
   uint8_t* updateLocation = TR::LabelRelocation::getUpdateLocation();

   if (updateLocation == NULL && _cursor->getBinaryEncoding() != NULL)
      {
      updateLocation = setUpdateLocation(_cursor->getBinaryEncoding() + _offset);
      }

   return updateLocation;
   }

void
TR::InstructionLabelRelative32BitRelocation::apply(TR::CodeGenerator* cg)
   {
   uint8_t* p = getUpdateLocation();
   assertLabelDefined();
   *reinterpret_cast<int32_t*>(p) = static_cast<int32_t>(getLabel()->getCodeLocation() - p) / _divisor;
   }

uint8_t TR::ExternalRelocation::collectModifier()
   {
   TR::Compilation *comp = TR::comp();
   uint8_t * relocatableMethodCodeStart = (uint8_t *)comp->getRelocatableMethodCodeStart();
   uint8_t * updateLocation = getUpdateLocation();

   int32_t distanceFromStartOfBuffer = static_cast<int32_t>(updateLocation - relocatableMethodCodeStart);
   int32_t distanceFromStartOfMethod = static_cast<int32_t>(updateLocation - comp->cg()->getCodeStart());

   if (distanceFromStartOfBuffer < MIN_SHORT_OFFSET || distanceFromStartOfBuffer > MAX_SHORT_OFFSET)
      return RELOCATION_TYPE_WIDE_OFFSET;

   return 0;
   }

void TR::ExternalRelocation::addExternalRelocation(TR::CodeGenerator *cg)
   {
   TR::AheadOfTimeCompile::interceptAOTRelocation(this);

   TR_LinkHead<TR::IteratedExternalRelocation>& aot = cg->getAheadOfTimeCompile()->getAOTRelocationTargets();
   uint32_t narrowSize = getNarrowSize();
   uint32_t wideSize = getWideSize();
   flags8_t modifier(collectModifier());
   TR::IteratedExternalRelocation *r;

   for (r = aot.getFirst();
        r != 0;
        r = r->getNext())
      {
      if (r->full() == false                        &&
          r->getTargetAddress()  == _targetAddress  &&
          r->getTargetAddress2() == _targetAddress2 &&
          r->getTargetKind() == _kind               &&
          modifier.getValue() == r->getModifierValue())
         {
         if (!r->needsWideOffsets())
            {
            if (r->getSizeOfRelocationData() + narrowSize
                > MAX_SIZE_RELOCATION_DATA)
               {
               r->setFull();
               continue;  // look for one that's not full
               }
            }
         else
            {
            if (r->getSizeOfRelocationData() + wideSize
                > MAX_SIZE_RELOCATION_DATA)
               {
               r->setFull();
               continue;  // look for one that's not full
               }
            }
         r->setNumberOfRelocationSites(r->getNumberOfRelocationSites()+1);
         r->setSizeOfRelocationData(r->getSizeOfRelocationData() +
                            (r->needsWideOffsets()?wideSize:narrowSize));
         _relocationRecord = r;

         return;
         }
      }

   TR::IteratedExternalRelocation *temp =   _targetAddress2 ?
      new (cg->trHeapMemory()) TR::IteratedExternalRelocation(_targetAddress, _targetAddress2, _kind, modifier, cg) :
      new (cg->trHeapMemory()) TR::IteratedExternalRelocation(_targetAddress, _kind, modifier, cg);

   aot.add(temp);

   temp->setNumberOfRelocationSites(temp->getNumberOfRelocationSites()+1);
   temp->setSizeOfRelocationData(temp->getSizeOfRelocationData() +
                          (temp->needsWideOffsets()?wideSize:narrowSize));
   _relocationRecord = temp;
   }

void TR::ExternalRelocation::apply(TR::CodeGenerator *cg)
   {
   TR::Compilation *comp = cg->comp();
   uint8_t * relocatableMethodCodeStart = (uint8_t *)comp->getRelocatableMethodCodeStart();
   getRelocationRecord()->addRelocationEntry((uint32_t)(getUpdateLocation() - relocatableMethodCodeStart));
   }

void TR::ExternalRelocation::trace(TR::Compilation* comp)
   {
   TR::RelocationDebugInfo* data = this->getDebugInfo();
   uint8_t* updateLocation;
   TR_ExternalRelocationTargetKind kind = getRelocationRecord()->getTargetKind();

   updateLocation = getUpdateLocation();
   uint8_t* relocatableMethodCodeStart = (uint8_t*)comp->getRelocatableMethodCodeStart();
   uint8_t* codeStart = comp->cg()->getCodeStart();
   uintptr_t methodOffset = updateLocation - relocatableMethodCodeStart;
   uintptr_t programOffset = updateLocation - codeStart;

   if (data)
      {
      traceMsg(comp, "%-35s %-32s %5d      %04x       %04x %8p\n",
       getName(this->getTargetKind()),
       data->file,
       data->line,
       methodOffset,
       programOffset,
       data->node);
      traceMsg(comp, "TargetAddress1:" POINTER_PRINTF_FORMAT ",  TargetAddress2:" POINTER_PRINTF_FORMAT "\n", this->getTargetAddress(), this->getTargetAddress2());
      }
   }

uint8_t* TR::BeforeBinaryEncodingExternalRelocation::getUpdateLocation()
   {
   if (NULL == TR::ExternalRelocation::getUpdateLocation())
      setUpdateLocation(getUpdateInstruction()->getBinaryEncoding());
   return TR::ExternalRelocation::getUpdateLocation();
   }

TR::ExternalOrderedPair32BitRelocation::ExternalOrderedPair32BitRelocation(
                 uint8_t                         *location1,
                 uint8_t                         *location2,
                 uint8_t                         *target,
                 TR_ExternalRelocationTargetKind  k,
                 TR::CodeGenerator                *cg) :
   TR::ExternalRelocation(), _update2Location(location2)
   {
   setUpdateLocation(location1);
   setTargetAddress(target);
   setTargetKind(k);
   }


uint8_t TR::ExternalOrderedPair32BitRelocation::collectModifier()
   {
   TR::Compilation *comp = TR::comp();
   uint8_t * relocatableMethodCodeStart = (uint8_t *)comp->getRelocatableMethodCodeStart();
   uint8_t * updateLocation;
   uint8_t * updateLocation2;
   TR_ExternalRelocationTargetKind kind = getTargetKind();

   if (comp->target().cpu.isPower() &&
          (kind == TR_ArrayCopyHelper || kind == TR_ArrayCopyToc || kind == TR_RamMethod || kind == TR_GlobalValue || kind == TR_BodyInfoAddressLoad || kind == TR_DataAddress
           || kind == TR_DebugCounter || kind == TR_BlockFrequency || kind == TR_RecompQueuedFlag || kind == TR_CatchBlockCounter || kind == TR_MethodEnterExitHookAddress))
      {
      TR::Instruction *instr = (TR::Instruction *)getUpdateLocation();
      TR::Instruction *instr2 = (TR::Instruction *)getLocation2();
      updateLocation = instr->getBinaryEncoding();
      updateLocation2 = instr2->getBinaryEncoding();
      }
   else
      {
      updateLocation = getUpdateLocation();
      updateLocation2 = getLocation2();
      }

   int32_t iLoc = static_cast<int32_t>(updateLocation - relocatableMethodCodeStart);
   int32_t iLoc2 = static_cast<int32_t>(updateLocation2 - relocatableMethodCodeStart);

   if ( (iLoc < MIN_SHORT_OFFSET  || iLoc > MAX_SHORT_OFFSET ) || (iLoc2 < MIN_SHORT_OFFSET || iLoc2 > MAX_SHORT_OFFSET ) )
      return RELOCATION_TYPE_WIDE_OFFSET | ITERATED_RELOCATION_TYPE_ORDERED_PAIR;

   return ITERATED_RELOCATION_TYPE_ORDERED_PAIR;
   }


void TR::ExternalOrderedPair32BitRelocation::apply(TR::CodeGenerator *cg)
   {
   TR::Compilation *comp = cg->comp();

   TR::IteratedExternalRelocation *rec = getRelocationRecord();
   uint8_t *codeStart = (uint8_t *)comp->getRelocatableMethodCodeStart();
   TR_ExternalRelocationTargetKind kind = getRelocationRecord()->getTargetKind();
   if (comp->target().cpu.isPower() &&
      (kind == TR_ArrayCopyHelper || kind == TR_ArrayCopyToc || kind == TR_RamMethodSequence || kind == TR_GlobalValue || kind == TR_BodyInfoAddressLoad || kind == TR_DataAddress
       || kind == TR_DebugCounter || kind == TR_BlockFrequency || kind == TR_RecompQueuedFlag || kind == TR_CatchBlockCounter || kind == TR_MethodEnterExitHookAddress))
      {
      TR::Instruction *instr = (TR::Instruction *)getUpdateLocation();
      TR::Instruction *instr2 = (TR::Instruction *)getLocation2();
      rec->addRelocationEntry((uint32_t)(instr->getBinaryEncoding() - codeStart));
      rec->addRelocationEntry((uint32_t)(instr2->getBinaryEncoding() - codeStart));
      }
   else
      {
      rec->addRelocationEntry(static_cast<uint32_t>(getUpdateLocation() - codeStart));
      rec->addRelocationEntry(static_cast<uint32_t>(getLocation2() - codeStart));
      }
   }

// remember to update the debug extensions when add or changing relocations.
const char *TR::ExternalRelocation::_externalRelocationTargetKindNames[TR_NumExternalRelocationKinds] =
   {
   "TR_ConstantPool (0)",
   "TR_HelperAddress (1)",
   "TR_RelativeMethodAddress (2)",
   "TR_AbsoluteMethodAddress (3)",
   "TR_DataAddress (4)",
   "TR_ClassObject (5)",
   "TR_MethodObject (6)",
   "TR_InterfaceObject (7)",
   "TR_AbsoluteHelperAddress (8)",
   "TR_FixedSequenceAddress (9)",
   "TR_FixedSequenceAddress2 (10)",
   "TR_JNIVirtualTargetAddress (11)",
   "TR_JNIStaticTargetAddress  (12)",
   "TR_ArrayCopyHelper (13)",
   "TR_ArrayCopyToc (14)",
   "TR_BodyInfoAddress (15)",
   "TR_Thunks (16)",
   "TR_StaticRamMethodConst (17)",
   "TR_Trampolines (18)",
   "TR_PicTrampolines (19)",
   "TR_CheckMethodEnter(20)",
   "TR_RamMethod (21)",
   "TR_RamMethodSequence (22)",
   "TR_RamMethodSequenceReg (23)",
   "TR_VerifyClassObjectForAlloc (24)",
   "TR_ConstantPoolOrderedPair (25)",
   "TR_AbsoluteMethodAddressOrderedPair (26)",
   "TR_VerifyRefArrayForAlloc (27)",
   "TR_J2IThunks (28)",
   "TR_GlobalValue (29)",
   "TR_BodyInfoAddressLoad (30)",
   "TR_ValidateInstanceField (31)",
   "TR_InlinedStaticMethodWithNopGuard (32)",
   "TR_InlinedSpecialMethodWithNopGuard (33)",
   "TR_InlinedVirtualMethodWithNopGuard (34)",
   "TR_InlinedInterfaceMethodWithNopGuard (35)",
   "TR_SpecialRamMethodConst (36)",
   "TR_InlinedHCRMethod (37)",
   "TR_ValidateStaticField (38)",
   "TR_ValidateClass (39)",
   "TR_ClassAddress (40)",
   "TR_HCR (41)",
   "TR_ProfiledMethodGuardRelocation (42)",
   "TR_ProfiledClassGuardRelocation (43)",
   "TR_HierarchyGuardRelocation (44)",
   "TR_AbstractGuardRelocation (45)",
   "TR_ProfiledInlinedMethodRelocation (46)",
   "TR_MethodPointer (47)",
   "TR_ClassPointer (48)",
   "TR_CheckMethodExit (49)",
   "TR_ValidateArbitraryClass (50)",
   "TR_EmitClass (51)",
   "TR_JNISpecialTargetAddress (52)",
   "TR_VirtualRamMethodConst (53)",
   "TR_InlinedInterfaceMethod (54)",
   "TR_InlinedVirtualMethod (55)",
   "TR_NativeMethodAbsolute (56)",
   "TR_NativeMethodRelative (57)",
   "TR_ArbitraryClassAddress (58)",
   "TR_DebugCounter (59)",
   "TR_ClassUnloadAssumption (60)",
   "TR_J2IVirtualThunkPointer (61)",
   "TR_InlinedAbstractMethodWithNopGuard (62)",
   "TR_ValidateRootClass (63)",
   "TR_ValidateClassByName (64)",
   "TR_ValidateProfiledClass (65)",
   "TR_ValidateClassFromCP (66)",
   "TR_ValidateDefiningClassFromCP (67)",
   "TR_ValidateStaticClassFromCP (68)",
   "TR_ValidateClassFromMethod (69)",
   "TR_ValidateComponentClassFromArrayClass (70)",
   "TR_ValidateArrayClassFromComponentClass (71)",
   "TR_ValidateSuperClassFromClass (72)",
   "TR_ValidateClassInstanceOfClass (73)",
   "TR_ValidateSystemClassByName (74)",
   "TR_ValidateClassFromITableIndexCP (75)",
   "TR_ValidateDeclaringClassFromFieldOrStatic (76)",
   "TR_ValidateClassClass (77)",
   "TR_ValidateConcreteSubClassFromClass (78)",
   "TR_ValidateClassChain (79)",
   "TR_ValidateRomClass (80)",
   "TR_ValidatePrimitiveClass (81)",
   "TR_ValidateMethodFromInlinedSite (82)",
   "TR_ValidateMethodByName (83)",
   "TR_ValidateMethodFromClass (84)",
   "TR_ValidateStaticMethodFromCP (85)",
   "TR_ValidateSpecialMethodFromCP (86)",
   "TR_ValidateVirtualMethodFromCP (87)",
   "TR_ValidateVirtualMethodFromOffset (88)",
   "TR_ValidateInterfaceMethodFromCP (89)",
   "TR_ValidateMethodFromClassAndSig (90)",
   "TR_ValidateStackWalkerMaySkipFramesRecord (91)",
   "TR_ValidateArrayClassFromJavaVM (92)",
   "TR_ValidateClassInfoIsInitialized (93)",
   "TR_ValidateMethodFromSingleImplementer (94)",
   "TR_ValidateMethodFromSingleInterfaceImplementer (95)",
   "TR_ValidateMethodFromSingleAbstractImplementer (96)",
   "TR_ValidateImproperInterfaceMethodFromCP (97)",
   "TR_SymbolFromManager (98)",
   "TR_MethodCallAddress (99)",
   "TR_DiscontiguousSymbolFromManager (100)",
   "TR_ResolvedTrampolines (101)",
   "TR_BlockFrequency (102)",
   "TR_RecompQueuedFlag (103)",
   "TR_InlinedStaticMethod (104",
   "TR_InlinedSpecialMethod (105)",
   "TR_InlinedAbstractMethod (106)",
   "TR_Breakpoint (107)",
   "TR_InlinedMethodPointer (108)",
   "TR_VMINLMethod (109)",
   "TR_ValidateJ2IThunkFromMethod (110)",
   "TR_StaticDefaultValueInstance (111)",
   "TR_ValidateIsClassVisible (112)",
   "TR_CatchBlockCounter (113)",
   "TR_StartPC (114)",
   "TR_MethodEnterExitHookAddress (115)",
   };

uintptr_t TR::ExternalRelocation::_globalValueList[TR_NumGlobalValueItems] =
   {
   0,          // not used
   0,          // TR_CountForRecompile
   0,          // TR_HeapBase
   0,          // TR_HeapTop
   0,          // TR_HeapBaseForBarrierRange0
   0,          // TR_ActiveCardTableBase
   0           // TR_HeapSizeForBarrierRange0
   };

char *TR::ExternalRelocation::_globalValueNames[TR_NumGlobalValueItems] =
   {
   "not used (0)",
   "TR_CountForRecompile (1)",
   "TR_HeapBase (2)",
   "TR_HeapTop (3)",
   "TR_HeapBaseForBarrierRange0 (4)",
   "TR_ActiveCardTableBase (5)",
   "TR_HeapSizeForBarrierRange0 (6)"
   };


TR::IteratedExternalRelocation::IteratedExternalRelocation(uint8_t *target, TR_ExternalRelocationTargetKind k, flags8_t modifier, TR::CodeGenerator *cg)
      : TR_Link<TR::IteratedExternalRelocation>(),
        _numberOfRelocationSites(0),
        _targetAddress(target),
        _targetAddress2(NULL),
        _relocationData(NULL),
        _relocationDataCursor(NULL),
        // initial size is size of header for this type
        _sizeOfRelocationData(cg->getAheadOfTimeCompile()->getSizeOfAOTRelocationHeader(k)),
        _recordModifier(modifier.getValue()),
        _full(false),
        _kind(k)
      {
      }

TR::IteratedExternalRelocation::IteratedExternalRelocation(uint8_t *target, uint8_t *target2, TR_ExternalRelocationTargetKind k, flags8_t modifier, TR::CodeGenerator *cg)
      : TR_Link<TR::IteratedExternalRelocation>(),
        _numberOfRelocationSites(0),
        _targetAddress(target),
        _targetAddress2(target2),
        _relocationData(NULL),
        _relocationDataCursor(NULL),
        // initial size is size of header for this type
        _sizeOfRelocationData(cg->getAheadOfTimeCompile()->getSizeOfAOTRelocationHeader(k)),
        _recordModifier(modifier.getValue()),
        _full(false),
        _kind(k)
      {
      }

void TR::IteratedExternalRelocation::initializeRelocation(TR::CodeGenerator *cg)
   {
   _relocationDataCursor = cg->getAheadOfTimeCompile()->initializeAOTRelocationHeader(this);
   }

void TR::IteratedExternalRelocation::addRelocationEntry(uint32_t locationOffset)
   {
   TR::Compilation *comp = TR::comp();
   if (!needsWideOffsets())
      {
      *(uint16_t *)_relocationDataCursor = (uint16_t)locationOffset;
      _relocationDataCursor += 2;
      }
   else
      {
      *(uint32_t *)_relocationDataCursor = locationOffset;
      _relocationDataCursor += 4;
      }
   }

TR::ExternalRelocation *TR::ExternalRelocation::create(
      uint8_t *codeAddress,
      uint8_t *targetAddress,
      TR_ExternalRelocationTargetKind kind,
      TR::CodeGenerator *cg)
   {
   return new (cg->trHeapMemory()) TR::ExternalRelocation(codeAddress, targetAddress, kind, cg);
   }

TR::ExternalRelocation *TR::ExternalRelocation::create(
      uint8_t *codeAddress,
      uint8_t *targetAddress,
      uint8_t *targetAddress2,
      TR_ExternalRelocationTargetKind kind,
      TR::CodeGenerator *cg)
   {
   return new (cg->trHeapMemory()) TR::ExternalRelocation(codeAddress, targetAddress, targetAddress2, kind, cg);
   }
