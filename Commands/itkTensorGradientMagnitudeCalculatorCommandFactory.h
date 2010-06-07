#ifndef _itk_TensorGradientMagnitudeCalculatorCommandFactory_h_
#define _itk_TensorGradientMagnitudeCalculatorCommandFactory_h_

#include "itkObjectFactoryBase.h"

namespace itk
{
  
  class TensorGradientMagnitudeCalculatorCommandFactory : public ObjectFactoryBase
  {
    
  public:
    typedef TensorGradientMagnitudeCalculatorCommandFactory Self;
    typedef ObjectFactoryBase        Superclass;
    typedef SmartPointer<Self>       Pointer;
    typedef SmartPointer<const Self> ConstPointer;
    
    /** Class methods used to interface with the registered factories. */
    virtual const char* GetITKSourceVersion(void) const;
    virtual const char* GetDescription(void) const;
    
    /** Method for class instantiation. */
    itkFactorylessNewMacro(Self);
    static TensorGradientMagnitudeCalculatorCommandFactory* FactoryNew() { return new TensorGradientMagnitudeCalculatorCommandFactory;}
    
    /** Run-time type information (and related methods). */
    itkTypeMacro(TensorGradientMagnitudeCalculatorCommandFactory, ObjectFactoryBase);
    
    /** Register one factory of this type  */
    static void RegisterOneFactory(void)
    {
      TensorGradientMagnitudeCalculatorCommandFactory::Pointer CSFFactory = TensorGradientMagnitudeCalculatorCommandFactory::New();
      ObjectFactoryBase::RegisterFactory( CSFFactory );
    }
    
		
  protected:
    TensorGradientMagnitudeCalculatorCommandFactory();
    ~TensorGradientMagnitudeCalculatorCommandFactory();
    
  private:
    TensorGradientMagnitudeCalculatorCommandFactory(const Self&);
    void operator=(const Self&);
    
  };
  
}

#endif