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

#ifndef OMR_METHOD_METADATAPOD_INCL
#define OMR_METHOD_METADATAPOD_INCL

/*
 * The following #define(s) and typedef(s) must appear before any #includes in this file
 */
#ifndef OMR_METHOD_METADATAPOD_CONNECTOR
#define OMR_METHOD_METADATAPOD_CONNECTOR
namespace OMR { struct MethodMetaDataPOD; }
namespace OMR { typedef OMR::MethodMetaDataPOD MethodMetaDataPODConnector; }
#endif

#include <stdint.h>
#include "infra/Annotations.hpp"

/*
 * This structure describes the shape of the method meta data information.
 * It must be a C++ POD object and follow all the rules behind POD formation.
 * Because its fields may be extracted by a runtime system, its exact layout
 * shape MUST be preserved.
 *
 */

namespace OMR
{

struct OMR_EXTENSIBLE MethodMetaDataPOD
   {
   uintptr_t startPC;
   uintptr_t endPC;
   };

}

#endif
