/*=========================================================================

Library:   TubeTK

Copyright 2010 Kitware Inc. 28 Corporate Drive,
Clifton Park, NY, 12065, USA.

All rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

=========================================================================*/

#include "itktubeRidgeSeedFilter.h"
#include "itktubeRidgeSeedFilterIO.h"
#include "tubeStringUtilities.h"

#include "EnhanceTubesUsingDiscriminantAnalysisCLP.h"

template< class TPixel, unsigned int TDimension >
int DoIt( int argc, char * argv[] );

// Must follow include of "...CLP.h"
//   and forward declaration of int DoIt( ... ).
#include "tubeCLIHelperFunctions.h"

template< class TPixel, unsigned int TDimension >
int DoIt( int argc, char * argv[] )
{
  PARSE_ARGS;

  std::vector< std::string > inputVolumesList;
  tube::StringToVector< std::string >( inputVolumesString,
    inputVolumesList );

  itk::TimeProbesCollectorBase timeCollector;

  typedef TPixel                                    InputPixelType;
  typedef itk::Image< InputPixelType, TDimension >  InputImageType;
  typedef itk::ImageFileReader< InputImageType >    InputImageReaderType;

  typedef itk::Image< unsigned short, TDimension >  LabelmapType;
  typedef itk::ImageFileReader< LabelmapType >      LabelmapReaderType;

  typedef itk::tube::RidgeSeedFilterIO< InputImageType, LabelmapType >
                                                    RidgeSeedFilterIOType;

  typedef itk::tube::RidgeSeedFilter< InputImageType, LabelmapType >
                                                    RidgeSeedFilterType;

  typedef typename RidgeSeedFilterType::ProbabilityImageType
                                                    OutputImageType;
  typedef itk::ImageFileWriter< OutputImageType >   OutputImageWriterType;


  typename RidgeSeedFilterType::Pointer tubeFilter =
    RidgeSeedFilterType::New();

  timeCollector.Start( "LoadData" );

  typename InputImageReaderType::Pointer reader;
  for( unsigned int vNum = 0; vNum < inputVolumesList.size(); vNum++ )
    {
    reader = InputImageReaderType::New();
    reader->SetFileName( inputVolumesList[vNum].c_str() );
    reader->Update();
    if( vNum == 0 )
      {
      tubeFilter->SetInput( reader->GetOutput() );
      }
    else
      {
      tubeFilter->AddInput( reader->GetOutput() );
      }
    }

  if( !labelmap.empty() )
    {
    typename LabelmapReaderType::Pointer inLabelmapReader =
      LabelmapReaderType::New();
    inLabelmapReader->SetFileName( labelmap.c_str() );
    inLabelmapReader->Update();
    tubeFilter->SetLabelMap( inLabelmapReader->GetOutput() );

    tubeFilter->SetRidgeId( tubeId );
    tubeFilter->SetBackgroundId( backgroundId );
    tubeFilter->SetUnknownId( unknownId );
    }

  timeCollector.Stop( "LoadData" );

  if( !loadDiscriminantInfo.empty() )
    {
    timeCollector.Start( "LoadBasis" );
    RidgeSeedFilterIOType ridgeSeedFilterIO( tubeFilter );
    ridgeSeedFilterIO.Read( loadDiscriminantInfo.c_str() );
    timeCollector.Stop( "LoadBasis" );

    tubeFilter->SetTrainClassifier( false );
    }
  else
    {

    tubeFilter->SetScales( tubeScales );
    tubeFilter->SetSkeletonize( false );
    tubeFilter->GetPDFSegmenter()->SetProbabilityImageSmoothingStandardDeviation(
     tubeScales[0] / 2 );

    tubeFilter->SetTrainClassifier( true );
    }

  timeCollector.Start( "Update" );
  tubeFilter->Update();
  timeCollector.Stop( "Update" );

  timeCollector.Start( "Classify" );
  tubeFilter->ClassifyImages();
  timeCollector.Stop( "Classify" );

  if( !saveDiscriminantInfo.empty() )
    {
    timeCollector.Start( "SaveBasis" );
    RidgeSeedFilterIOType ridgeSeedFilterIO( tubeFilter );
    ridgeSeedFilterIO.Write( saveDiscriminantInfo.c_str() );
    timeCollector.Stop( "SaveBasis" );
    }

  timeCollector.Start( "WriteOutput" );
  typename OutputImageWriterType::Pointer outputWriter =
    OutputImageWriterType::New();
  outputWriter->SetFileName( outputVolume.c_str() );
  outputWriter->SetUseCompression( true );
  outputWriter->SetInput( tubeFilter->
    GetClassProbabilityDifferenceForInput( 0 ) );
  outputWriter->Update();
  timeCollector.Stop( "WriteOutput" );

  timeCollector.Report();

  return EXIT_SUCCESS;
}

int main( int argc, char * argv[] )
{
  PARSE_ARGS;

  std::vector< std::string > inputVolumesList;
  tube::StringToVector< std::string >( inputVolumesString,
    inputVolumesList );

  // You may need to update this line if, in the project's .xml CLI file,
  //   you change the variable name for the inputVolume.
  return tube::ParseArgsAndCallDoIt( inputVolumesList[0], argc, argv );
}
