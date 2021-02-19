#include "Lights.hpp"
namespace BlackWalnut
{

	SampledSpectrum DistanceLight::Phi(const SampledWavelengths& Lambd) const
	{
		return Lemit.Sample(Lambd) * Scale  * Pi * SceneRadius * SceneRadius;
	}

	BlackWalnut::LightLeSample DistanceLight::SampleLe(const Vector2f &u1, const Vector2f &u2, SampledWavelengths &lambda, float time) const
	{
		// Choose point on disk oriented toward infinite light direction
		Vector3f w;
		Matrix4X4MultVector3(w, RenderFromLight, Vector3f(0, 0, 1));
		Normalize(w);
		Frame wFrame = Frame::FromZ(w);
		Vector2f cd = SampleUniformDiskConcentric(u1);
		Vector3f pDisk = SceneCenter +  wFrame.FromLocal(Vector3f(cd.X, cd.Y, 0)) * SceneRadius;

		// Compute _DistantLight_ light ray
		Ray ray(pDisk + w * SceneRadius , w * (-1), time);

		return LightLeSample(Lemit.Sample(lambda) * Scale , ray,
			1 / (Pi * SceneRadius * SceneRadius), 1);
	}
	void DistanceLight::PDF_Le(const Ray &InRay, float *pdfPos, float *pdfDir) const
	{
		*pdfPos = 1 / (Pi * SceneRadius * SceneRadius);
		*pdfDir = 0;
	}

	SampledSpectrum DiffuseAreaLight::Phi(const SampledWavelengths &Lambda) const
	{
		SampledSpectrum phi(0.f);
		if (image) {
			// Compute average light image emission
			for (int y = 0; y < image.Resolution.Y; ++y)
				for (int x = 0; x < image.Resolution.X; ++x) {
					RGB rgb;
					for (int c = 0; c < 3; ++c)
						rgb[c] = image.GetChannel({ x, y }, c);
					phi += RGBSpectrum(*imageColorSpace, rgb).Sample(Lambda);
				}
			phi /= float(image.Resolution.X * image.Resolution.Y);

		}
		else
			phi = Lemit.Sample(Lambda);
		return phi * ((twoSided ? 2 : 1) * Scale * area * Pi);
	}

	BlackWalnut::LightLiSample DiffuseAreaLight::SampleLi(LightSampleContext Ctx, Vector2f U, SampledWavelengths Lambd, LightSamplingMode Mode /*= LightSamplingMode::WithoutMIS*/) const
	{
		// Sample point on shape for _DiffuseAreaLight_
		ShapeSampleContext shapeCtx(Ctx.pi, Ctx.n, Ctx.ns, 0 /* time */);
		ShapeSample ss = Shape->Sample(shapeCtx, U);
		if (ss.pdf == 0)
			return{};
		
		CHECK(!IsNaN(ss.pdf));
		Vector3f wi = Ctx.p() - ss.intr.P(); //光源指向物体
		if (ss.pdf == 0 || Length(wi) == 0)
			return{};

		// Return _LightLiSample_ for sampled point on shape
		Normalize(wi);
		SampledSpectrum Le = L(ss.intr.P(), ss.intr.n, ss.intr.uv, wi, Lambd);
		if (!Le)
			return{};
		return LightLiSample(this, Le, wi, ss.pdf, ss.intr);
	}

	float DiffuseAreaLight::PDF_Li(LightSampleContext Ctx, Vector3f Wi, LightSamplingMode Mode /*= LightSamplingMode::WithoutMIS*/) const
	{
		ShapeSampleContext shapeCtx(Ctx.pi, Ctx.n, Ctx.ns, 0 /* time */);
		return Shape->PDF(shapeCtx, Wi);
	}

}