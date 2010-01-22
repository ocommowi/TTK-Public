#include "itkResampleTensorImageFilter.h"
#include "itkLogTensorImageFilter.h"
#include "itkExpTensorImageFilter.h"
#include "itkTensorImageIO.h"
#include "itkAffineTensorTransform.h"
#include "itkTensorLinearInterpolateImageFunction.h"

#include "GetPot.h"

void PrintHelp(const char* exec)
{
  std::cout << exec << ": " << std::endl;
  std::cout << "-i  [Tensor Image File]" << std::endl;
  std::cout << "-m  [matrix file]" << std::endl;
  std::cout << "-x  [int]" << std::endl;
  std::cout << "-y  [int]" << std::endl;
  std::cout << "-z  [int]" << std::endl;
  std::cout << "-sx [double]" << std::endl;
  std::cout << "-sy [double]" << std::endl;
  std::cout << "-sz [double]" << std::endl;
  std::cout << "-ox [double]" << std::endl;
  std::cout << "-oy [double]" << std::endl;
  std::cout << "-oz [double]" << std::endl;
  std::cout << "-o  [Output File Name]" << std::endl;
  exit(0);
}

int main(int argc, char* argv[])
{
  
  GetPot   cl(argc, argv); // argument parser
  if( cl.size() == 1 || cl.search(2, "--help", "-h") ) 
    PrintHelp(cl[0]);

  const bool IsInputPresent = cl.search(2,"-I","-i");
  const bool IsOutputPresent = cl.search(2,"-O","-o");

  if( !IsInputPresent || !IsOutputPresent )
  {
    std::cerr << "Error: Input and (or) output not set." << std::endl;
    exit (-1);
  }
  
  const char* tensorFile = cl.follow("NoFile",2,"-I","-i");
  const char* outFile = cl.follow("NoFile",2,"-O","-o");
  const char* mat = cl.follow("NoFile",2,"-m","-M");
  const int x = cl.follow(0,2,"-X","-x");
  const int y = cl.follow(0,2,"-Y","-y");
  const int z = cl.follow(0,2,"-Z","-z");
  const double sx = cl.follow(1.0,2,"-SX","-sx");
  const double sy = cl.follow(1.0,2,"-SY","-sy");
  const double sz = cl.follow(1.0,2,"-SZ","-sz");
  const double ox = cl.follow(0.0,2,"-OX","-ox");
  const double oy = cl.follow(0.0,2,"-OY","-oy");
  const double oz = cl.follow(0.0,2,"-OZ","-oz");
  const bool le = cl.follow (1, 2, "-l", "-L");
  
  
  typedef double ScalarType;  
  typedef itk::TensorImageIO<ScalarType, 3, 3>  IOType;
  typedef IOType::TensorImageType               TensorImageType;
  typedef itk::ResampleTensorImageFilter<TensorImageType,TensorImageType>
    FilterType;
  typedef TensorImageType::SizeType    SizeType;
  typedef TensorImageType::SpacingType SpacingType;
  typedef TensorImageType::PointType   PointType;

  
  SizeType size = {{x,y,z}};
  SpacingType spacing;
  spacing[0] = sx;
  spacing[1] = sy;
  spacing[2] = sz;
  PointType origin;
  origin[0] = ox;
  origin[1] = oy;
  origin[2] = oz;    

  
  IOType::Pointer myIO = IOType::New();
  myIO->SetFileName(tensorFile);

  std::cout << "Reading: " << tensorFile << std::endl;
  try
    {
      myIO->Read();
    }
  catch(itk::ExceptionObject &e)
  {
    std::cerr << e << std::endl;
    exit(-1);
  }
  
  TensorImageType::Pointer tensors = myIO->GetOutput();





  typedef itk::AffineTensorTransform< ScalarType, 3 >  TransformType;
  TransformType::Pointer transform = TransformType::New();
  

  // read the affine matrix
  std::ifstream buffer (mat);
  if( buffer.fail() )
  {
    std::cerr << "Error: Cannot read file " << mat << std::endl;
    exit (-1);
  }

  
  TransformType::MatrixType       matrix;
  TransformType::OutputVectorType translation;

  // skip the first 12 floats
  char junk [512];
  for( unsigned int i=0; i<12; i++)
  {
    buffer >> junk;
  }
  
  for( unsigned int i=0 ;i<3; i++)
  {
    buffer >> matrix (i,0);
    buffer >> matrix (i,1);
    buffer >> matrix (i,2);
  }
  
  for( unsigned int i=0; i<3; i++)
  {
    buffer >> translation[i];
  }
  
  transform->SetMatrix (matrix);
  transform->SetTranslation (translation);
  
  TransformType::Pointer inv_transform = TransformType::New();
  transform->GetInverse(inv_transform);
  
  transform = inv_transform;
  buffer.close();
  
  std::cout << "Matrix is: " << std::endl;
  std::cout << matrix << std::endl;
  std::cout << "Translation is: " << std::endl;
  std::cout << translation << std::endl;
  
  std::cout << transform << std::endl;




  
  if( le )
  {
    // log:
    typedef itk::LogTensorImageFilter<TensorImageType, TensorImageType>
      LogFilterType;
    LogFilterType::Pointer myLog = LogFilterType::New();
    myLog->SetInput(tensors);

    myLog->Update();
   
    tensors = myLog->GetOutput();
  }
  
    
    
  typedef itk::TensorLinearInterpolateImageFunction<TensorImageType, double>  InterpolatorType;
  InterpolatorType::Pointer interpolator = InterpolatorType::New();
  
  FilterType::Pointer myFilter = FilterType::New();
  myFilter->SetTensorInterpolator( interpolator );
  myFilter->SetSize(size);
  myFilter->SetOutputSpacing(spacing);
  myFilter->SetOutputOrigin(origin);

  myFilter->SetInput(tensors);

  std::cout << "Desired output size is   : " << myFilter->GetSize() << std::endl;
  std::cout << "Desired output spacing is: " << myFilter->GetOutputSpacing() << std::endl;
  std::cout << "Desired output origin is : " << myFilter->GetOutputOrigin() << std::endl;

  
  myFilter->Update();

  tensors = myFilter->GetOutput();

  
  if( le )
  {
    // exp:
    typedef itk::ExpTensorImageFilter<TensorImageType, TensorImageType> ExpFilterType;
    ExpFilterType::Pointer myExp = ExpFilterType::New();
    
    myExp->SetInput(tensors);

    myExp->Update();

    tensors = myExp->GetOutput();
    
  }


   
    myIO->SetFileName(outFile);
    myIO->SetInput(tensors);
    
    std::cout << "Writing: " << outFile << std::flush;
    try
    {
      myIO->Write();
    }
    catch(itk::ExceptionObject &e)
    {
      std::cerr << e << std::endl;
      exit(-1);
    }
    std::cout << " Done." << std::endl;
      
  return 0;

}
