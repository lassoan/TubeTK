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

set( MODULE_NAME AtlasBuilderUsingIntensity )
project( ${MODULE_NAME} )

set( AtlasBuilderUsingIntensity_H_Files
  itktubeCompleteImageResampleFilter.h
  itktubeMeanAndSigmaImageBuilder.h
  itktubeMinimizeImageSizeFilter.h
  itktubeRobustMeanAndSigmaImageBuilder.h
  tubeAtlasSummation.h )

set( AtlasBuilderUsingIntensity_HXX_Files
  itktubeCompleteImageResampleFilter.hxx
  itktubeMeanAndSigmaImageBuilder.hxx
  itktubeMinimizeImageSizeFilter.hxx
  itktubeRobustMeanAndSigmaImageBuilder.hxx )

add_custom_target( AtlasBuilderUsingIntensityInclude SOURCES
  ${AtlasBuilderUsingIntensity_H_Files}
  ${AtlasBuilderUsingIntensity_HXX_Files} )

if( NOT TubeTK_SOURCE_DIR )
  find_package( TubeTK REQUIRED )
  include( ${TubeTK_USE_FILE} )
endif( NOT TubeTK_SOURCE_DIR )

find_package( SlicerExecutionModel REQUIRED )
include( ${SlicerExecutionModel_USE_FILE} )

find_package( ITK REQUIRED )
if( TubeTK_BUILD_WITHIN_SLICER )
  set( ITK_NO_IO_FACTORY_REGISTER_MANAGER 1 ) # See Libs/ITKFactoryRegistration/CMakeLists.txt
endif( TubeTK_BUILD_WITHIN_SLICER )
include( ${ITK_USE_FILE} )

SEMMacroBuildCLI(
  NAME ${MODULE_NAME}
  ADDITIONAL_SRCS tubeAtlasSummation.cxx
  LOGO_HEADER ${TubeTK_SOURCE_DIR}/Base/CLI/TubeTKLogo.h
  TARGET_LIBRARIES
    ${ITK_LIBRARIES}
    TubeTKCommon TubeTKObjectDocuments )

if( BUILD_TESTING )
  find_package( PythonInterp )
  if( PYTHON_EXECUTABLE )
    add_subdirectory( Testing )
  endif( PYTHON_EXECUTABLE )
endif( BUILD_TESTING )
