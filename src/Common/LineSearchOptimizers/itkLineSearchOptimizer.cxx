#ifndef __itkLineSearchOptimizer_cxx
#define __itkLineSearchOptimizer_cxx

#include "itkLineSearchOptimizer.h"
#include "itkNumericTraits.h"

namespace itk
{

  /**
   * ******************* Constructor *******************
   */

  LineSearchOptimizer::LineSearchOptimizer()
  {
    this->m_MinimumStepLength = NumericTraits<double>::Zero;
    this->m_MaximumStepLength = NumericTraits<double>::max();
    this->m_InitialStepLengthEstimate = NumericTraits<double>::One;
  } // end constructor


  /**
   * ***************** SetCurrentStepLength *************
   *
   * Set the current step length AND the current position, where
   * the current position is computed as:
   * m_CurrentPosition = 
   * m_InitialPosition + StepLength * m_LineSearchDirection
   */

  void
    LineSearchOptimizer::SetCurrentStepLength(double step)
  {
    itkDebugMacro("Setting current step length to " << step );
  
    this->m_CurrentStepLength = step;

    ParametersType newPosition =  this->GetInitialPosition();
    const unsigned int numberOfParameters = newPosition.GetSize();
    const ParametersType & LSD = this->GetLineSearchDirection();

    for (unsigned int i = 0; i < numberOfParameters; ++i)
    {
      newPosition[i] += (step * LSD[i]);
    }
    
    this->SetCurrentPosition(newPosition);

  } // end SetCurrentStepLength


  /**
   * ******************** DirectionalDerivative **************************
   *
   * Computes the inner product of the argument and the line search direction
   */

  double
    LineSearchOptimizer::
    DirectionalDerivative(const DerivativeType & derivative) const
  {
    /** Easy, thanks to the functions defined in vnl_vector.h */
    return inner_product( derivative, this->GetLineSearchDirection() );
  } // end DirectionalDerivative
  


} // end namespace itk


#endif // #ifndef __itkLineSearchOptimizer_cxx

