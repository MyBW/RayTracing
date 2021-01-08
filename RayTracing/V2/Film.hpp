#pragma once
#include "Util\Spectrum.hpp"
#include "Util\ColorSpace.hpp"
#include "Math\Matrix.hpp"
#include "Util\Check.h"
namespace BlackWalnut
{
	class PixelSensor
	{
	public:
		PixelSensor(DenselySampledSpectrum R, DenselySampledSpectrum G, DenselySampledSpectrum B,
			const RGBColorSpace* ColorSapce, float webTmp, float InImageRatio):
			R_Bar(R),G_Bar(G),B_Bar(B),ImageRatio(InImageRatio)
		{
			DenselySampledSpectrum SensorIlluminant = D(webTmp);
			Matrix3X3f m = SolveXYZFromSensorRGB(SensorIlluminant, *ColorSapce->Illuminant);
			if (!m) CHECK(false);
			XYZFromSceneRGB = m;
		}
		PixelSensor(const RGBColorSpace *OutputColorSpace, float WebTmp, float InImageRatio)
			:R_Bar(X()),G_Bar(Y()),B_Bar(Z()),ImageRatio(InImageRatio)
		{
			if (WebTmp != 0)
			{
				auto WhiteIlluminant = D(WebTmp);
				Vector2f SourceWhite = SpectrumToXYZ(&WhiteIlluminant).xy();
				Vector2f TargetWhite = OutputColorSpace->W;
				XYZFromSceneRGB = WhiteBalance(SourceWhite, TargetWhite);
			}
		}
		float GetImageRatio() const { return ImageRatio; }
		RGB ToSensorRGB(const SampledSpectrum &S, const SampledWavelengths &Lambda) const
		{
			return RGB((R_Bar.Sample(Lambda)*S).Average(),
				(G_Bar.Sample(Lambda)*S).Average(),
				(B_Bar.Sample(Lambda)*S).Average());
		}
		Matrix3X3f XYZFromSceneRGB;
	private:
		Matrix3X3f SolveXYZFromSensorRGB(BaseSpectrum &SensorIlluminant, BaseSpectrum &OutputIlluminant);
		RGB IlluminantToSensorRGB(BaseSpectrum &Illum) const
		{
			RGB rgb(InnerProduct(Illum, R_Bar), InnerProduct(Illum, G_Bar), InnerProduct(Illum, B_Bar));
			return rgb / rgb.Y;
		}
		template<typename Triplet>
		static Triplet ProjectReflectance(const BaseSpectrum &Ref1, const BaseSpectrum &Illum, const BaseSpectrum &B1,const BaseSpectrum &B2, const BaseSpectrum &B3);
		DenselySampledSpectrum R_Bar, G_Bar, B_Bar;
		float ImageRatio;
		std::vector<BaseSpectrum> SwatchReflectances;
	};

	template<typename Triplet>
	Triplet BlackWalnut::PixelSensor::ProjectReflectance(const BaseSpectrum &Ref1,const BaseSpectrum &Illum,const BaseSpectrum &B1,const BaseSpectrum &B2,const BaseSpectrum &B3)
	{
		Triplet Result;
		float G_Internal = 0;
		for (float lambda = Lambda_min; lambda <= Lambda_max; ++lambda) {
			G_Internal += B2(lambda) * Illum(lambda);
			Result[0] += B1(lambda) * Ref1(lambda) * Illum(lambda);
			Result[1] += B2(lambda) * Ref1(lambda) * Illum(lambda);
			Result[2] += B3(lambda) * Ref1(lambda) * Illum(lambda);
		}
		return Result / G_Internal;
	}



}