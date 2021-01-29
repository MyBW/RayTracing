#pragma once
#include "..\..\Math\BWPrimitive.h"
#include "..\V2\Util\Spectrum.hpp"
#include "..\V2\Interaction.hpp"
#include "..\V2\Math\Geom.hpp"
namespace BlackWalnut
{
	enum class LightType : int { DeltaPosition, DeltaDirection, Area, Infinite };
	enum class LightSamplingMode { WithMIS, WithoutMIS };
	class LightBase;

	struct LightLiSample
	{

		LightLiSample() = default;
		operator bool() const { return PDF > 0; }

		LightLiSample(const LightBase* light, const SampledSpectrum &L, const Vector3f &wi,
			float pdf, const Interaction &pLight)
			: L(L), Wi(wi), PDF(pdf), Light(light), InteractionPoint(pLight) {}

		SampledSpectrum L;
		Vector3f Wi;
		float PDF = 0;
		const LightBase* Light;
		Interaction InteractionPoint;
	};

	struct LightLeSample {
	public:
		LightLeSample() = default;

		LightLeSample(const SampledSpectrum &L, const Ray &ray, float pdfPos, float pdfDir)
			: L(L), ray(ray), pdfPos(pdfPos), pdfDir(pdfDir)
		{
		}
		LightLeSample(const SampledSpectrum &L, const Ray &ray, const Interaction &intr,
			float pdfPos, float pdfDir)
			: L(L), ray(ray), intr(intr), pdfPos(pdfPos), pdfDir(pdfDir)
		{
			//CHECK(this->intr->n != Normal3f(0, 0, 0));
			intrCanUse = true;
		}


		float AbsCosTheta(const Vector3f &w) const { return intrCanUse ? AbsDot(w, intr.n) : 1; }


		// FIXME: should this be || or && ?. Review usage...
		operator bool() const { return pdfPos > 0 || pdfDir > 0; }

		SampledSpectrum L;
		Ray ray;
		Interaction intr;
		bool intrCanUse = false;
		float pdfPos = 0, pdfDir = 0;
	};

	class LightSampleContext {
	public:
		LightSampleContext() = default;

		LightSampleContext(const SurfaceInteraction &si)
			: pi(si.pi), n(si.n), ns(si.shading.n) {}

		LightSampleContext(const Interaction &intr) : pi(intr.pi) {}

		LightSampleContext(const Vector3f &pi, const Vector3f &n, const Vector3f &ns)
			: pi(pi), n(n), ns(ns) {}


		Vector3f p() const { return Vector3f(pi); }

		Vector3f pi;
		Vector3f n, ns;
	};

	class LightBase
	{
	public:
		LightBase() = default;
		LightBase(LightType InType, Matrix4X4f Trans):mType(InType),RenderFromLight(Trans) { }
		virtual void PreProcess(const Bounds3f &SceneBounds) = 0;
		virtual LightType Type() const { return this->mType; }
		virtual SampledSpectrum Phi(const SampledWavelengths &Lambd) const = 0;
		virtual LightLiSample SampleLi(LightSampleContext Ctx, Vector2f U, SampledWavelengths Lambd, LightSamplingMode Mode = LightSamplingMode::WithoutMIS) const = 0;
		virtual float PDF_Li(LightSampleContext Ctx, Vector3f Wi, LightSamplingMode Mode = LightSamplingMode::WithoutMIS) const = 0;
		virtual void PDF_Le(const Interaction &InInteraction, Vector3f &W, float *pdfPos, float *pdfDir) const = 0;
		virtual void PDF_Le(const Ray &InRay, float *pdfPos, float *pdfDir) const = 0;
		
		virtual LightLeSample SampleLe(const Vector2f &u1, const Vector2f &u2, SampledWavelengths &lambda, float time) const = 0;
		
		virtual SampledSpectrum L(const Vector3f &p, const Vector3f &n, const Vector2f &uv, const Vector3f &w, const SampledWavelengths &lambda) const 
		{
			return SampledSpectrum(0.f);
		}
		virtual SampledSpectrum Le(const Ray &ray, const SampledWavelengths &lambda) const 
		{
			return SampledSpectrum(0.f);
		}
	protected:
		LightType mType;
		Matrix4X4f RenderFromLight;
	};
}