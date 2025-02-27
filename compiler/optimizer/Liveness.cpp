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

#include <stddef.h>
#include <stdint.h>
#include "env/StackMemoryRegion.hpp"
#include "compile/Compilation.hpp"
#include "control/Options.hpp"
#include "control/Options_inlines.hpp"
#include "env/TRMemory.hpp"
#include "il/Node.hpp"
#include "infra/Assert.hpp"
#include "infra/BitVector.hpp"
#include "optimizer/DataFlowAnalysis.hpp"

class TR_BlockStructure;
class TR_Structure;
namespace TR { class Optimizer; }

TR_DataFlowAnalysis::Kind TR_Liveness::getKind()
   {
   return Liveness;
   }

TR_Liveness *TR_Liveness::asLiveness()
   {
   return this;
   }

bool TR_Liveness::supportsGenAndKillSets()
   {
   return true;
   }

int32_t TR_Liveness::getNumberOfBits()
   {
   return _liveVariableInfo->numLocals();
   }

void TR_Liveness::analyzeNode(TR::Node *, vcount_t, TR_BlockStructure *, TR_BitVector *)
   {
   }

TR_Liveness::TR_Liveness(TR::Compilation           *comp,
                         TR::Optimizer           *optimizer,
			                TR_Structure               *rootStructure,
                         bool                        ignoreOSRUses,
                         TR_LiveVariableInformation *liveVariableInfo,
                         bool                        splitLongs,
                         bool                        includeParms)
   : TR_BackwardUnionBitVectorAnalysis(comp, comp->getFlowGraph(), optimizer, comp->getOption(TR_TraceLiveness)),
     _liveVariableInfo(liveVariableInfo)
   {
   _traceLiveness = comp->getOption(TR_TraceLiveness);

   if (liveVariableInfo == NULL)
      {
      // can be re-used by the caller because it's allocated in caller's stack
      _liveVariableInfo = new (trStackMemory()) TR_LiveVariableInformation(comp, optimizer, rootStructure, splitLongs, includeParms,
                                                                           ignoreOSRUses);
      _liveVariableInfo->collectLiveVariableInformation();
      }
   }

void TR_Liveness::perform(TR_Structure *rootStructure)
   {
   if (traceLiveness())
      traceMsg(comp(), "Starting Liveness analysis\n");

   if (_liveVariableInfo->numLocals() == 0)
      return; // Nothing to do if there are no locals

   if (comp()->getVisitCount() > 8000)
      comp()->resetVisitCounts(1);

   // Allocate the block info before setting the stack mark - it will be used by
   // the caller
   //
   initializeBlockInfo();

   {
   TR::StackMemoryRegion stackMemoryRegion(*trMemory());

   performAnalysis(rootStructure, false);

   if (traceLiveness())
      {
      for (int32_t i = 1; i < _numberOfNodes; ++i)
         {
         if (_blockAnalysisInfo[i])
            {
            traceMsg(comp(), "\nLive variables for block_%d: ",i);
            _blockAnalysisInfo[i]->print(comp());
            }
         }
      traceMsg(comp(), "\nEnding Liveness analysis\n");
      }
   } // scope of the stack memory region

   }

bool TR_Liveness::postInitializationProcessing()
   {
   if (traceLiveness())
      {
      for (int32_t i = 1; i < _numberOfNodes; ++i)
         {
         traceMsg(comp(), "\nGen and kill sets for block_%d: ",i);
         if (_regularGenSetInfo[i])
            {
            traceMsg(comp(), " gen set ");
            _regularGenSetInfo[i]->print(comp());
            }
         if (_regularKillSetInfo[i])
            {
            traceMsg(comp(), " kill set ");
            _regularKillSetInfo[i]->print(comp());
            }
         if (_exceptionGenSetInfo[i])
            {
            traceMsg(comp(), " exception gen set ");
            _exceptionGenSetInfo[i]->print(comp());
            }
         if (_exceptionKillSetInfo[i])
            {
            traceMsg(comp(), " exception kill set ");
            _exceptionKillSetInfo[i]->print(comp());
            }
         }
      }
   return true;
   }

void TR_Liveness::initializeGenAndKillSetInfo()
   {
   _liveVariableInfo->initializeGenAndKillSetInfo(_regularGenSetInfo, _regularKillSetInfo, _exceptionGenSetInfo, _exceptionKillSetInfo);
   }


void TR_Liveness::analyzeTreeTopsInBlockStructure(TR_BlockStructure *blockStructure)
   {
   TR_ASSERT(false, "Liveness should use gen and kill sets");
   }
