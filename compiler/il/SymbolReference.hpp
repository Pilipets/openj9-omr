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

#ifndef TR_SYMBOLREFERENCE_INCL
#define TR_SYMBOLREFERENCE_INCL

#include "il/OMRSymbolReference.hpp"

#include <stdint.h>
#include "compile/SymbolReferenceTable.hpp"
#include "env/KnownObjectTable.hpp"
#include "env/jittypes.h"
#include "infra/Annotations.hpp"

class mcount_t;
namespace TR { class Symbol; }

namespace TR
{

class OMR_EXTENSIBLE SymbolReference : public OMR::SymbolReferenceConnector
   {

public:
   SymbolReference() : OMR::SymbolReferenceConnector() {}

   SymbolReference(TR::SymbolReferenceTable * symRefTab) :
      OMR::SymbolReferenceConnector(symRefTab) {}

   SymbolReference(TR::SymbolReferenceTable * symRefTab,
                   TR::Symbol * symbol,
                   intptr_t offset = 0) :
      OMR::SymbolReferenceConnector(symRefTab,
                                    symbol,
                                    offset) {}

   SymbolReference(TR::SymbolReferenceTable * symRefTab,
                   int32_t refNumber,
                   TR::Symbol *ps,
                   intptr_t offset = 0) :
      OMR::SymbolReferenceConnector(symRefTab,
                                    refNumber,
                                    ps,
                                    offset) {}

   SymbolReference(TR::SymbolReferenceTable *symRefTab,
                   TR::SymbolReferenceTable::CommonNonhelperSymbol number,
                   TR::Symbol *ps,
                   intptr_t offset = 0) :
      OMR::SymbolReferenceConnector(symRefTab,
                                    number,
                                    ps,
                                    offset) {}

   SymbolReference(TR::SymbolReferenceTable *symRefTab,
                   TR::Symbol *sym,
                   mcount_t owningMethodIndex,
                   int32_t cpIndex,
                   int32_t unresolvedIndex = 0,
                   TR::KnownObjectTable::Index knownObjectIndex = TR::KnownObjectTable::UNKNOWN) :
      OMR::SymbolReferenceConnector(symRefTab,
                                    sym,
                                    owningMethodIndex,
                                    cpIndex,
                                    unresolvedIndex,
                                    knownObjectIndex) {}

   SymbolReference(TR::SymbolReferenceTable *symRefTab,
                   TR::SymbolReference& sr,
                   intptr_t offset,
                   TR::KnownObjectTable::Index knownObjectIndex = TR::KnownObjectTable::UNKNOWN) :
      OMR::SymbolReferenceConnector(symRefTab,
                                    sr,
                                    offset,
                                    knownObjectIndex) {}

protected:

   SymbolReference(TR::SymbolReferenceTable * symRefTab,
                   TR::Symbol *               symbol,
                   intptr_t                  offset,
                   const char *               name) :
      OMR::SymbolReferenceConnector(symRefTab,
                                    symbol,
                                    offset,
                                    name) {}

   };

}

#endif
