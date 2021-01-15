#pragma once
#include "Util\Spectrum.hpp"
#include "Util\ColorSpace.hpp"
#include "Math\Matrix.hpp"
#include "Util\Check.h"
#include "../Math/BWPrimitive.h"
#include "Filter.hpp"
#include <atomic>
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

	class VisibleSurface
	{
	public:
		/*VisibleSurface(const SurfaceInteraction& SI, const CamneraTransform &InCameraTransform, const SampledSpectrum &InAlbedo, const SampledWavelengths &Lambda)
			:Albedo(InAlbedo)
		{
			Set = true;

		}
		operator bool() const { return Set; }
		bool Set = false;
		Vector3f P;
		Vector3f Normal;
		float Time = 0;
		float DzDx = 0, DxDy = 0;
		SampledSpectrum Albedo;*/
	};

	struct FilmBaseParameters
	{
		Vector2i FullResolution;
		Bounds2i PixelBounds;
		FilterBase* Filter;
		float Diagonal;
		const PixelSensor *Sensor;
		std::string FileName;
	};
	class FilmBase
	{
	public:
		FilmBase(const FilmBaseParameters &Parameters)
			:FullResolution(Parameters.FullResolution)
			,PixelBounds(Parameters.PixelBounds)
			,Filter(Parameters.Filter)
			,Diagonal(Parameters.Diagonal * 0.001f)
			,Sensor(Parameters.Sensor)
			,FileName(Parameters.FileName)
		{
			
		}
		Vector2i GetFullResolution() { return FullResolution; }
		Bounds2i GetPixelBounds() { return PixelBounds; }
		FilterBase* GetFilterBase() { return Filter; }
		const PixelSensor* GetPixelSensor() { return Sensor; }
		std::string GetFileName() { return FileName; }
	protected:

		Vector2i FullResolution;
		Bounds2i PixelBounds;
		FilterBase* Filter;
		float Diagonal;
		const PixelSensor *Sensor;
		std::string FileName;
	};

	class RGBFilm : public FilmBase
	{
	private:
		struct Pixel
		{
			Pixel(const Pixel &S)
			{
				WeightSum = S.WeightSum;
				RGBSum[0] = S.RGBSum[0];
				RGBSum[1] = S.RGBSum[1];
				RGBSum[2] = S.RGBSum[2];
				//SplatRGB[0] = S.SplatRGB[0];
			}
			Pixel() = default;
			double RGBSum[3] = { 0.0, 0.0, 0.0 };
			double WeightSum = 0;
			std::atomic<double> SplatRGB[3];
			//double SplatRGB[3];
		};
	public:
		bool UseVisibleSurface() const { return false; }
		void AddSample(const Vector2i &FilmPos, SampledSpectrum L, const SampledWavelengths &Lambda, const VisibleSurface* Surface, float Weight)
		{
			SampledSpectrum H = L * Sensor->GetImageRatio();
			RGB SensorRGB = Sensor->ToSensorRGB(H, Lambda);
			float M = std::max({ SensorRGB.X,SensorRGB.Y ,SensorRGB.Z });
			if (M > MaxComponentValue)
			{
				H *= MaxComponentValue / M;
				SensorRGB *= MaxComponentValue / M;
			}
			CHECK(PixelBounds.IsInTheBound(FilmPos));
			Pixel& PixelData = Pixels[FilmPos.X][FilmPos.Y];
			for (int c = 0; c < 3; c++)
			{
				PixelData.RGBSum[c] += Weight * SensorRGB[c];
			}
			PixelData.WeightSum += Weight;
		}
		RGB GetPixelRGB(const Vector2i& PixelPos, float SplatScale = 1) const
		{
			const Pixel &PixelData = Pixels[PixelPos.X][PixelPos.Y];
			RGB PixelRGB(PixelData.RGBSum[0], PixelData.RGBSum[1], PixelData.RGBSum[2]);
			float WeightSum = PixelData.WeightSum;
			if (WeightSum) PixelRGB /= WeightSum;
			for (int i = 0; i < 3; i++)
			{
				PixelRGB[i] += SplatScale * PixelData.SplatRGB[i] / FilterIntegral;
			}
			PixelRGB = Mul<RGB, 3, RGB>(OutputRGBFromSensorRGB, PixelRGB);
			return PixelRGB;
		}
		RGBFilm() = default;
		RGBFilm(const FilmBaseParameters& Parameter, const RGBColorSpace* InColorSpace, float InMaxComponentValue = FLT_MAX, bool InWriteFP16 = true)
			:FilmBase(Parameter), ColorSpace(InColorSpace), MaxComponentValue(InMaxComponentValue), WriteFP16(InWriteFP16)
		{
			Pixels.resize(Parameter.PixelBounds.GetMax()[0]);
			for (auto & ColPixel : Pixels)
			{
				ColPixel.resize(Parameter.PixelBounds.GetMax()[1]);
			}
			FilterIntegral = Filter->Integral();
			CHECK(PixelBounds.Area() > 0);
			CHECK(ColorSpace != nullptr);
			
			OutputRGBFromSensorRGB = ColorSpace->RGBFromXYZ * Sensor->XYZFromSceneRGB;
		}
		SampledWavelengths SampleWavelengths(float u) const
		{
			return SampledWavelengths::SampleXYZ(u);
		}
		void AddSplat(const Vector2f &P, SampledSpectrum L, const SampledWavelengths &Lambda)
		{
			CHECK(!L.HasNaNs());
			SampledSpectrum H = L * Sensor->GetImageRatio();
			RGB SensorRGB = Sensor->ToSensorRGB(H, Lambda);
			float M = std::max({ SensorRGB.X,SensorRGB.Y ,SensorRGB.Z });
			if (M > MaxComponentValue)
			{
				H *= MaxComponentValue / M;
				SensorRGB *= MaxComponentValue / M;
			}

			Vector2f Distance = P + Vector2f(0.5, 0.5);
			Vector2f Radius = Filter->Radius();
			Vector2f Minf = Distance - Radius;
			Vector2i Min(std::floor(Minf.X), std::floor(Minf.Y));
			Vector2f Maxf = Distance + Radius;
			Vector2i Max(std::floor(Maxf.X + 1.0), std::floor(Maxf.Y + 1));

			Bounds2i SplatBounds;
			SplatBounds.SetMax(0, Max.X);
			SplatBounds.SetMax(1, Max.Y);
			SplatBounds.SetMin(0, Min.X);
			SplatBounds.SetMin(1, Min.Y);

			SplatBounds = Intersect(SplatBounds, PixelBounds);

			CHECK(false);// See PBRT_V4

		}
		RGB ToOutputRGB(const SampledSpectrum &L, const SampledWavelengths& Lambda) const
		{
			RGB SensorRGB = Sensor->ToSensorRGB(L * Sensor->GetImageRatio(), Lambda);
			return Mul<RGB, 3, RGB>(OutputRGBFromSensorRGB, SensorRGB);
		}
	private:
		float MaxComponentValue;
		typedef std::vector<std::vector<Pixel>> Pixel2D;
		Pixel2D Pixels;
		Matrix3X3f OutputRGBFromSensorRGB;
		float FilterIntegral;
		const RGBColorSpace* ColorSpace;
		float InMaxComponentValue;
		float WriteFP16;
	};
}