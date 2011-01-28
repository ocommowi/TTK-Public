/*=========================================================================

  Program:   Tensor ToolKit - TTK
  Module:    $URL$
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) INRIA 2010. All rights reserved.
  See LICENSE.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "itkAverage4DImageCommand.h"

#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkImage.h>
#include <itkGetAverageSliceImageFilter.h>
#include <itkExtractImageFilter.h>

#include <iostream>
#include "GetPot.h"


namespace itk
{

  Average4DImageCommand::Average4DImageCommand()
  {
    m_ShortDescription = "Average a 4D image";
    m_LongDescription = "Usage:\n";
    m_LongDescription += "<-i input> <-n offset> <-o output>\n\n";
    m_LongDescription += m_ShortDescription;
  }

  Average4DImageCommand::~Average4DImageCommand()
  {}

  int Average4DImageCommand::Execute (int narg, const char* arg[])
  {
    itk::Object::GlobalWarningDisplayOff();
    
    GetPot cl(narg, const_cast<char**>(arg)); // argument parser
    if( cl.size() == 1 || cl.search(2, "--help", "-h") )
    {
      std::cout << this->GetLongDescription() << std::endl;
      return -1;
    }

    const char* input = cl.follow("","-i");
    const char* output = cl.follow("output.nii.gz","-o");
    int offset = cl.follow (0, "-n");
    

    typedef unsigned short                ScalarType;
    typedef Image<ScalarType, 4>          InputImageType;
    typedef Image<ScalarType, 3>          OutputImageType;
    
    typedef itk::ImageFileReader<InputImageType>     ImageReaderType;
    typedef itk::ImageFileWriter<OutputImageType>    ImageWriterType;
    
    InputImageType::Pointer image = 0;
    {
      ImageReaderType::Pointer reader = ImageReaderType::New();
      reader->SetFileName( input );
      try
      {
	reader->Update();
      }
      catch (itk::ExceptionObject &e)
      {
	std::cerr << e;
	return EXIT_FAILURE;
      }

      image = reader->GetOutput();
      image->DisconnectPipeline();
    }


    if (offset>0)
    {
      typedef ExtractImageFilter<InputImageType, InputImageType> Extract4DFilterType;
      InputImageType::RegionType region = image->GetLargestPossibleRegion();
      InputImageType::SizeType size = region.GetSize();
      size[3] -= offset;
      InputImageType::IndexType index = region.GetIndex();
      index[3] = offset;
      region.SetSize  (size);
      region.SetIndex (index);
      Extract4DFilterType::Pointer extractor = Extract4DFilterType::New();
      extractor->SetInput (image);
      extractor->SetExtractionRegion (region);
      try
      {
	extractor->Update();
      }
      catch (itk::ExceptionObject &e)
      {
	std::cerr << e;
	return EXIT_FAILURE;
      }

      image = extractor->GetOutput();
      image->DisconnectPipeline();
    }    


    
    typedef GetAverageSliceImageFilter<InputImageType, InputImageType> FilterType;
    {
      FilterType::Pointer filter = FilterType::New();
      filter->SetInput (image);
      filter->SetAveragedOutDimension (3);
      try
      {
	filter->Update();
      }
      catch (itk::ExceptionObject &e)
      {
	std::cerr << e;
	return EXIT_FAILURE;
      }

      image = filter->GetOutput();
      image->DisconnectPipeline();
    }


    OutputImageType::Pointer outImage = 0;
    typedef ExtractImageFilter<InputImageType, OutputImageType> ExtractFilterType;
    {
      InputImageType::RegionType region = image->GetLargestPossibleRegion();
      InputImageType::SizeType size = region.GetSize();
      size[3] = 0;
      InputImageType::IndexType index = region.GetIndex();
      index[3] = 0;
      region.SetSize  (size);
      region.SetIndex (index);
      ExtractFilterType::Pointer extractor = ExtractFilterType::New();
      extractor->SetInput (image);
      extractor->SetExtractionRegion (region);
      try
      {
	extractor->Update();
      }
      catch (itk::ExceptionObject &e)
      {
	std::cerr << e;
	return EXIT_FAILURE;
      }

      outImage = extractor->GetOutput();
      outImage->DisconnectPipeline();
    }
    
    // write the image
    ImageWriterType::Pointer writer = ImageWriterType::New();
    writer->SetFileName( output );
    writer->SetInput( outImage );
    
    std::cout << "Writing: " << output << std::flush;
    try
    {
      writer->Update();
    }
    catch( itk::ExceptionObject &e)
    {
      std::cerr << e;
      return -1;
    }
    
    std::cout << " Done." << std::endl;
    
    
    return 0;
  }
  
}