#ifndef _itk_TensorToCsFunction_h_
#define _itk_TensorToCsFunction_h_

#include "itkTensorToScalarFunction.h"

namespace itk
{

  template <class TInputPixelType, class TOutputPixelType>
    class ITK_EXPORT TensorToCsFunction
    : public TensorToScalarFunction<TInputPixelType, TOutputPixelType>
  {
    
  public:
    typedef TensorToCsFunction Self;
    typedef TensorToScalarFunction<TInputPixelType,TOutputPixelType> Superclass;

    typedef SmartPointer<Self> Pointer;
    typedef SmartPointer<const Self> ConstPointer;

    itkTypeMacro (TensorToCsFunction, TensorToScalarFunction);

    itkNewMacro (Self);


    typedef TInputPixelType   InputPixelType;
    typedef TOutputPixelType  OutputPixelType;    

    OutputPixelType ComputeScalar (const InputPixelType&) const;


  protected:
    TensorToCsFunction(){};
    ~TensorToCsFunction(){};

    void PrintSelf(std::ostream& os, Indent indent) const
    {
      Superclass::PrintSelf(os, indent);
    }


  private:
    TensorToCsFunction (const Self&);
    void operator=(const Self&);        	

  };  
  
  
} // end of namespace


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkTensorToCsFunction.txx"
#endif


#endif
