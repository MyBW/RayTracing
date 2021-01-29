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
}