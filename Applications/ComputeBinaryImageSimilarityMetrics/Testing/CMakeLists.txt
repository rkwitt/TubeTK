##############################################################################
#
# Library:   TubeTK
#
# Copyright 2010 Kitware Inc. 28 Corporate Drive,
# Clifton Park, NY, 12065, USA.
#
# All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
##############################################################################

include_regular_expression( "^.*$" )

include( Midas3FunctionAddTest )
set( MIDAS_REST_URL http://midas3.kitware.com/midas/api/rest )
set( MIDAS_KEY_DIR ${TubeTK_SOURCE_DIR}/MIDAS_Keys )

set( TEMP ${TubeTK_BINARY_DIR}/Temporary )

set( PROJ_EXE
 ${SlicerExecutionModel_CLI_RUNTIME_OUTPUT_DIRECTORY}/${MODULE_NAME} )

set( TEXTCOMPARE_EXE
 ${TubeTK_BINARY_DIR}/bin/TextCompareCommand )

# Test1
Midas3FunctionAddTest( ${MODULE_NAME}-Test1
            ${PROJ_EXE}
               -o ${TEMP}/${MODULE_NAME}Test1.txt
               MIDAS{GDS0015_Large_Modified.mha.md5}
               MIDAS{GDS0015_Large.mha.md5} )

# Test1-Compare
Midas3FunctionAddTest( ${MODULE_NAME}-Test1-Compare
            ${TEXTCOMPARE_EXE}
               -t ${TEMP}/${MODULE_NAME}Test1.txt
               -b MIDAS{${MODULE_NAME}Test1.txt.md5} )
set_property( TEST ${MODULE_NAME}-Test1-Compare
                      APPEND PROPERTY DEPENDS ${MODULE_NAME}-Test1 )
