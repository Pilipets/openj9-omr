###############################################################################
# Copyright IBM Corp. and others 2016
#
# This program and the accompanying materials are made available under
# the terms of the Eclipse Public License 2.0 which accompanies this
# distribution and is available at http://eclipse.org/legal/epl-2.0
# or the Apache License, Version 2.0 which accompanies this distribution
# and is available at https://www.apache.org/licenses/LICENSE-2.0.
#
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the
# Eclipse Public License, v. 2.0 are satisfied: GNU General Public License,
# version 2 with the GNU Classpath Exception [1] and GNU General Public
# License, version 2 with the OpenJDK Assembly Exception [2].
#
# [1] https://www.gnu.org/software/classpath/license.html
# [2] https://openjdk.org/legal/assembly-exception.html
#
# SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0-only WITH Classpath-exception-2.0 OR GPL-2.0-only WITH OpenJDK-assembly-exception-1.0
###############################################################################

# Add our targets to the global targets
all: jit
clean: jit_cleanobjs jit_cleandeps jit_cleandll
cleanobjs: jit_cleanobjs
cleandeps: jit_cleandeps
cleandll: jit_cleandll

#
# Define our targets. "jit_cleanobjs" "jit_cleandeps" and "jit_cleandll" are double-colon so they can be appended to
# throughout the makefile.
#
.PHONY: jit jit_createdirs jit_cleanobjs jit_cleandeps jit_cleandll
jit:
jit_createdirs::
jit_cleanobjs::
jit_cleandeps::
jit_cleandll::

include $(JIT_MAKE_DIR)/rules/$(TOOLCHAIN)/common.mk
