#pragma once
#include "Base\Light.hpp"
namespace BlackWalnut
{

	class DistanceLight : public LightBase
	{
	public:
		DistanceLight(const Matrix4X4f& Transform, BaseSpectrum* L, float Scale) :LightBase(LightType::DeltaDirection, Transform), Scale(Scale), Lemit(*L)
		{

		}
		void PreProcess(const Bounds3f &sceneBounds) override {}
		SampledSpectrum Phi(const SampledWavelengths& Lambd) const override;
		float PDF_Li(LightSampleContext Ctx, Vector3f Wi, LightSamplingMode Mode /* = LightSamplingMode::WithoutMIS */) const override { return 0; }
		LightLeSample SampleLe(const Vector2f &u1, const Vector2f &u2, SampledWavelengths &lambda, float time) const override;
		void PDF_Le(const Ray &InRay, float *pdfPos, float *pdfDir) const override;
		void PDF_Le(const Interaction &InInteraction, Vector3f &W, float *pdfPos, float *pdfDir) const override
		{
			CHECK(0);
		}
		LightLiSample SampleLi(LightSampleContext Ctx, Vector2f U, SampledWavelengths Lambda, LightSamplingMode Mode /* = LightSamplingMode::WithoutMIS */) const override
		{
			Vector3f wi;
			Matrix4X4MultVector3(wi, RenderFromLight, Vector3f(0, 0, 1));
			Normalize(wi);
			Vector3f pOutside = Ctx.p() + wi * (2 * SceneRadius);
			return LightLiSample(this,  Lemit.Sample(Lambda) * Scale, wi, 1,
				Interaction(pOutside, 0 /* time */));
		}
	private:
		DenselySampledSpectrum Lemit;
		float Scale;
		Vector3f SceneCenter;
		float SceneRadius;
	};



}