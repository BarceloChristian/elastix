/*======================================================================

  This file is part of the elastix software.

  Copyright (c) University Medical Center Utrecht. All rights reserved.
  See src/CopyrightElastix.txt or http://elastix.isi.uu.nl/legal.php for
  details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE. See the above copyright notices for more information.

======================================================================*/


#ifndef __itkAdvancedMeanSquaresImageToImageMetric_h
#define __itkAdvancedMeanSquaresImageToImageMetric_h

#include "itkSmoothingRecursiveGaussianImageFilter.h"
//#include "itkImageRandomCoordinateSampler.h"
#include "itkImageGridSampler.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkAdvancedImageToImageMetric.h"

namespace itk
{

/** \class AdvancedMeanSquaresImageToImageMetric
 * \brief Compute Mean square difference between two images, based on AdvancedImageToImageMetric...
 *
 * This Class is templated over the type of the fixed and moving
 * images to be compared.
 *
 * This metric computes the sum of squared differenced between pixels in
 * the moving image and pixels in the fixed image. The spatial correspondance
 * between both images is established through a Transform. Pixel values are
 * taken from the Moving image. Their positions are mapped to the Fixed image
 * and result in general in non-grid position on it. Values at these non-grid
 * position of the Fixed image are interpolated using a user-selected Interpolator.
 *
 * This implementation of the MeanSquareDifference is based on the
 * AdvancedImageToImageMetric, which means that:
 * \li It uses the ImageSampler-framework
 * \li It makes use of the compact support of B-splines, in case of B-spline transforms.
 * \li Image derivatives are computed using either the B-spline interpolator's implementation
 * or by nearest neighbor interpolation of a precomputed central difference image.
 * \li A minimum number of samples that should map within the moving image (mask) can be specified.
 *
 * \ingroup RegistrationMetrics
 * \ingroup Metrics
 */

template < class TFixedImage, class TMovingImage >
class AdvancedMeanSquaresImageToImageMetric :
    public AdvancedImageToImageMetric< TFixedImage, TMovingImage>
{
public:

  /** Standard class typedefs. */
  typedef AdvancedMeanSquaresImageToImageMetric   Self;
  typedef AdvancedImageToImageMetric<
    TFixedImage, TMovingImage >                   Superclass;
  typedef SmartPointer<Self>                      Pointer;
  typedef SmartPointer<const Self>                ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( AdvancedMeanSquaresImageToImageMetric, AdvancedImageToImageMetric );

  /** Typedefs from the superclass. */
  typedef typename
    Superclass::CoordinateRepresentationType              CoordinateRepresentationType;
  typedef typename Superclass::MovingImageType            MovingImageType;
  typedef typename Superclass::MovingImagePixelType       MovingImagePixelType;
  typedef typename Superclass::MovingImageConstPointer    MovingImageConstPointer;
  typedef typename Superclass::FixedImageType             FixedImageType;
  typedef typename Superclass::FixedImageConstPointer     FixedImageConstPointer;
  typedef typename Superclass::FixedImageRegionType       FixedImageRegionType;
  typedef typename Superclass::TransformType              TransformType;
  typedef typename Superclass::TransformPointer           TransformPointer;
  typedef typename Superclass::InputPointType             InputPointType;
  typedef typename Superclass::OutputPointType            OutputPointType;
  typedef typename Superclass::TransformParametersType    TransformParametersType;
  typedef typename Superclass::TransformJacobianType      TransformJacobianType;
  typedef typename Superclass::InterpolatorType           InterpolatorType;
  typedef typename Superclass::InterpolatorPointer        InterpolatorPointer;
  typedef typename Superclass::RealType                   RealType;
  typedef typename Superclass::GradientPixelType          GradientPixelType;
  typedef typename Superclass::GradientImageType          GradientImageType;
  typedef typename Superclass::GradientImagePointer       GradientImagePointer;
  typedef typename Superclass::GradientImageFilterType    GradientImageFilterType;
  typedef typename Superclass::GradientImageFilterPointer GradientImageFilterPointer;
  typedef typename Superclass::FixedImageMaskType         FixedImageMaskType;
  typedef typename Superclass::FixedImageMaskPointer      FixedImageMaskPointer;
  typedef typename Superclass::MovingImageMaskType        MovingImageMaskType;
  typedef typename Superclass::MovingImageMaskPointer     MovingImageMaskPointer;
  typedef typename Superclass::MeasureType                MeasureType;
  typedef typename Superclass::DerivativeType             DerivativeType;
  typedef typename Superclass::ParametersType             ParametersType;
  typedef typename Superclass::FixedImagePixelType        FixedImagePixelType;
  typedef typename Superclass::MovingImageRegionType      MovingImageRegionType;
  typedef typename Superclass::ImageSamplerType           ImageSamplerType;
  typedef typename Superclass::ImageSamplerPointer        ImageSamplerPointer;
  typedef typename Superclass::ImageSampleContainerType   ImageSampleContainerType;
  typedef typename
    Superclass::ImageSampleContainerPointer               ImageSampleContainerPointer;
  typedef typename Superclass::FixedImageLimiterType      FixedImageLimiterType;
  typedef typename Superclass::MovingImageLimiterType     MovingImageLimiterType;
  typedef typename
    Superclass::FixedImageLimiterOutputType               FixedImageLimiterOutputType;
  typedef typename
    Superclass::MovingImageLimiterOutputType              MovingImageLimiterOutputType;
  typedef typename
    Superclass::MovingImageDerivativeScalesType           MovingImageDerivativeScalesType;

  /** Some typedefs for computing the SelfHessian */
  typedef typename Superclass::HessianValueType           HessianValueType;
  typedef typename Superclass::HessianType                HessianType;

  /** Multithreading type */
  typedef typename Superclass::ThreaderType     ThreaderType;
  typedef typename Superclass::ThreadInfoType   ThreadInfoType;

  /** The fixed image dimension. */
  itkStaticConstMacro( FixedImageDimension, unsigned int,
    FixedImageType::ImageDimension );

  /** The moving image dimension. */
  itkStaticConstMacro( MovingImageDimension, unsigned int,
    MovingImageType::ImageDimension );

  /** Get the value for single valued optimizers. */
  virtual MeasureType GetValue( const TransformParametersType & parameters ) const;

  /** Get the derivatives of the match measure. */
  virtual void GetDerivative( const TransformParametersType & parameters,
    DerivativeType & derivative ) const;

  /** Get value and derivatives for multiple valued optimizers. */
  virtual void GetValueAndDerivative( const TransformParametersType & parameters,
    MeasureType& Value, DerivativeType& Derivative ) const;

  /** Get value and derivatives for each thread. */
  inline void ThreadedGetValueAndDerivative(unsigned int threadID );

  /** Gather the values and derivatives from all threads */
  inline void AfterThreadedGetValueAndDerivative(MeasureType & value, DerivativeType & derivative )const;

  /** ComputeDerivatives threader callback function */
  static ITK_THREAD_RETURN_TYPE ComputeDerivativesThreaderCallback( void * arg );

  /** Experimental feature: compute SelfHessian */
  virtual void GetSelfHessian( const TransformParametersType & parameters, HessianType & H ) const;

  /** Default: 1.0 mm */
  itkSetMacro( SelfHessianSmoothingSigma, double );
  itkGetConstMacro( SelfHessianSmoothingSigma, double );

  /** Default: 1.0 mm */
  itkSetMacro( SelfHessianNoiseRange, double );
  itkGetConstMacro( SelfHessianNoiseRange, double );

  /** Default: 100000 */
  itkSetMacro( NumberOfSamplesForSelfHessian, unsigned int );
  itkGetConstMacro( NumberOfSamplesForSelfHessian, unsigned int );

  /** Initialize the Metric by making sure that all the components
   *  are present and plugged together correctly.
   * \li Call the superclass' implementation
   * \li Estimate the normalization factor, if asked for.  */
  virtual void Initialize(void) throw ( ExceptionObject );

  /** Set/Get whether to normalize the mean squares measure.
   * This divides the MeanSquares by a factor (range/10)^2,
   * where range represents the maximum gray value range of the
   * images. Based on the ad hoc assumption that range/10 is the
   * maximum average difference that will be observed.
   * Dividing by range^2 sounds less ad hoc, but will yield
   * very small values. */
  itkSetMacro( UseNormalization, bool );
  itkGetConstMacro( UseNormalization, bool );

protected:
  AdvancedMeanSquaresImageToImageMetric();
  virtual ~AdvancedMeanSquaresImageToImageMetric();
  void PrintSelf( std::ostream& os, Indent indent ) const;

  /** Protected Typedefs ******************/

  /** Typedefs inherited from superclass */
  typedef typename Superclass::FixedImageIndexType                FixedImageIndexType;
  typedef typename Superclass::FixedImageIndexValueType           FixedImageIndexValueType;
  typedef typename Superclass::MovingImageIndexType               MovingImageIndexType;
  typedef typename Superclass::FixedImagePointType                FixedImagePointType;
  typedef typename Superclass::MovingImagePointType               MovingImagePointType;
  typedef typename Superclass::MovingImageContinuousIndexType     MovingImageContinuousIndexType;
  typedef typename Superclass::BSplineInterpolatorType            BSplineInterpolatorType;
  typedef typename Superclass::CentralDifferenceGradientFilterType CentralDifferenceGradientFilterType;
  typedef typename Superclass::MovingImageDerivativeType          MovingImageDerivativeType;
  typedef typename Superclass::NonZeroJacobianIndicesType         NonZeroJacobianIndicesType;

  /** Protected typedefs for SelfHessian */
  typedef SmoothingRecursiveGaussianImageFilter<
    FixedImageType, FixedImageType>                               SmootherType;
  typedef BSplineInterpolateImageFunction<
    FixedImageType, CoordinateRepresentationType>                 FixedImageInterpolatorType;
  typedef NearestNeighborInterpolateImageFunction<
    FixedImageType, CoordinateRepresentationType >                DummyFixedImageInterpolatorType;
  //typedef ImageRandomCoordinateSampler<FixedImageType>            SelfHessianSamplerType;
  typedef ImageGridSampler<FixedImageType>                        SelfHessianSamplerType;

  double m_NormalizationFactor;

  /** Computes the innerproduct of transform Jacobian with moving image gradient.
   * The results are stored in imageJacobian, which is supposed
   * to have the right size (same length as Jacobian's number of columns). */
  void EvaluateTransformJacobianInnerProduct(
    const TransformJacobianType & jacobian,
    const MovingImageDerivativeType & movingImageDerivative,
    DerivativeType & imageJacobian) const;

  /** Compute a pixel's contribution to the measure and derivatives;
   * Called by GetValueAndDerivative(). */
  void UpdateValueAndDerivativeTerms(
    const RealType fixedImageValue,
    const RealType movingImageValue,
    const DerivativeType & imageJacobian,
    const NonZeroJacobianIndicesType & nzji,
    MeasureType & measure,
    DerivativeType & deriv ) const;

  /** Compute a pixel's contribution to the SelfHessian;
   * Called by GetSelfHessian(). */
  void UpdateSelfHessianTerms(
    const DerivativeType & imageJacobian,
    const NonZeroJacobianIndicesType & nzji,
    HessianType & H) const;

private:
  AdvancedMeanSquaresImageToImageMetric(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  bool m_UseNormalization;
  double m_SelfHessianSmoothingSigma;
  double m_SelfHessianNoiseRange;
  unsigned int m_NumberOfSamplesForSelfHessian;

  mutable std::vector< MeasureType > m_ThreaderValues;
  mutable std::vector< DerivativeType > m_ThreaderDerivatives;
  mutable std::vector< unsigned long > m_ThreaderNbOfPixelCounted;

  mutable unsigned long m_SampleContainerSize;

  mutable ImageSampleContainerPointer m_SampleContainer;

  struct MultiThreaderComputeDerivativeType
  {
    typename DerivativeType::iterator derivativeIterator;
    typename std::vector< DerivativeType >::iterator  m_ThreaderDerivativesIterator;
    unsigned int numberOfParameters;
    double normal_sum;
  };

}; // end class AdvancedMeanSquaresImageToImageMetric

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkAdvancedMeanSquaresImageToImageMetric.hxx"
#endif

#endif // end #ifndef __itkAdvancedMeanSquaresImageToImageMetric_h

