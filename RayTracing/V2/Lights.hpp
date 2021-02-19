#pragma once
#include "Base\Light.hpp"
#include "Shape\Shape.hpp"
#include "Util\Image.hpp"
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
			Matrix4X4MultVector3(wi, RenderFromLight, Vector3f(0, 0, -1), 0.f); //OpenGL坐标系 灯光方向Z轴负方向
			Normalize(wi);// 世界坐标系 灯光指向物体
			Vector3f pOutside = Ctx.p() + wi * (2 * SceneRadius);
			return LightLiSample(this, Lemit.Sample(Lambda) * Scale, wi, 1,
				Interaction(pOutside, 0 /* time */));
		}
		void SetSceneRadius(float InSceneRadius)
		{
			SceneRadius = InSceneRadius;
		}
	private:
		DenselySampledSpectrum Lemit;
		float Scale;
		Vector3f SceneCenter;
		float SceneRadius;
	};

	class DiffuseAreaLight : public LightBase
	{
	public:
		DiffuseAreaLight(const Matrix4X4f &RenderFromLight, BaseSpectrum* Le, float Scale, ShapeBase* Shape, RGBColorSpace *ColorSpace, bool TwoSide)
			:LightBase(LightType::Area, RenderFromLight), Lemit(*Le), Scale(Scale), twoSided(TwoSide),Shape(Shape),imageColorSpace(ColorSpace)
		{
			area = Shape->Area();
		}
		void PreProcess(const Bounds3f &SceneBounds) override{ }
		virtual SampledSpectrum Phi(const SampledWavelengths &Lambd) const override;
		virtual LightLiSample SampleLi(LightSampleContext Ctx, Vector2f U, SampledWavelengths Lambd, LightSamplingMode Mode = LightSamplingMode::WithoutMIS) const override;
		virtual float PDF_Li(LightSampleContext Ctx, Vector3f Wi, LightSamplingMode Mode = LightSamplingMode::WithoutMIS) const;
		virtual void PDF_Le(const Interaction &InInteraction, Vector3f &W, float *pdfPos, float *pdfDir) const { }
		virtual void PDF_Le(const Ray &InRay, float *pdfPos, float *pdfDir) const { }

		virtual LightLeSample SampleLe(const Vector2f &u1, const Vector2f &u2, SampledWavelengths &lambda, float time) const { return {}; }

		SampledSpectrum L(const Vector3f &p, const Vector3f &n, const Vector2f &uv, const Vector3f &w, const SampledWavelengths &lambda) const {
			if (!twoSided && Dot(n, w) < 0)
				return SampledSpectrum(0.f);

			if (image) {
				RGB rgb;
				for (int c = 0; c < 3; ++c)
					rgb[c] = image.BilerpChannel(uv, c);
				return RGBSpectrum(*imageColorSpace, rgb).Sample(lambda) * Scale;
			}
			else
				return Lemit.Sample(lambda)* Scale;
		}

	private:
		DenselySampledSpectrum Lemit;
		float Scale;
		ShapeBase* Shape;
		bool twoSided;
		float area;
		const RGBColorSpace *imageColorSpace;
		Image image;
	};

}