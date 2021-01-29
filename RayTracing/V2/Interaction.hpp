#pragma once
#include "Ray.hpp"
#include "Base\BSDF.hpp"
#include "Util\Spectrum.hpp"
#include "Samplers.hpp"


namespace BlackWalnut
{
	class LightBase;
	class MaterialBase;
	class CameraBase;
	class Interaction
	{
	public:
		Interaction() = default;
		// Interaction Public Members
		Vector3f pi;
		float time = 0;
		Vector3f wo;
		Vector3f n;
		Vector2f uv;
		/*const MediumInterface *mediumInterface = nullptr;
		MediumHandle medium = nullptr;*/

		Interaction(Vector3f& p, float time):pi(p),time(time){}
		Vector3f P() const { return pi; }
		bool IsSurfaceInteraction() const { return n != Vector3f(0.0, 0.0, 0.0); }

		Vector3f OffsetRayOrigin(Vector3f W) const
		{
			return BlackWalnut::OffsetRayOrigin(pi, n, W);
		}
		RayDifferential SpawnRay(const Vector3f &d) const
		{
			return RayDifferential(OffsetRayOrigin(d), d, time);
		}
		Ray SpawnRayTo(Vector3f &P) const
		{
			Ray R = BlackWalnut::SpawnRayTo(pi, n, time, P);
			return R;
		}

		Ray SpawnRayTo(const Interaction &P) const
		{
			Ray R = BlackWalnut::SpawnRayTo(pi, n, time, P.pi, P.n);
			return R;
		}
	};
	class SurfaceInteraction : public Interaction
	{
	public:
		// SurfaceInteraction Public Members
		Vector3f dpdu, dpdv;
		Vector3f dndu, dndv;
		struct {
			Vector3f n;
			Vector3f dpdu, dpdv;
			Vector3f dndu, dndv;
		} shading;
		int faceIndex = 0;
		LightBase* AreaLightSrc = nullptr;
		MaterialBase* material;
		//LightBase* areaLight;
		mutable Vector3f dpdx, dpdy;
		mutable float dudx = 0, dvdx = 0, dudy = 0, dvdy = 0;
		BSDF GetBSDF(const RayDifferential& Ray, SampledWavelengths &Lambda, CameraBase* InCamera, SamplerBase* Sampler);
		void ComputeDifferentials(const RayDifferential &r, CameraBase* camera) const;
		void SkipIntersection(RayDifferential *Ray, float T) const;
		SampledSpectrum Le(const Vector3f &W, const SampledWavelengths &Lambda) const;
		using Interaction::SpawnRay;
		void SetShadingGeometry(const Vector3f &ns, const Vector3f &dpdus,
			const Vector3f &dpdvs, const Vector3f &dndus,
			const Vector3f &dndvs, bool orientationIsAuthoritative) {
			// Compute _shading.n_ for _SurfaceInteraction_
			shading.n = ns;
			CHECK_NE(shading.n, Vector3f(0, 0, 0));
			if (orientationIsAuthoritative)
				n = FaceForward(n, shading.n);
			else
				shading.n = FaceForward(shading.n, n);

			// Initialize _shading_ partial derivative values
			shading.dpdu = dpdus;
			shading.dpdv = dpdvs;
			shading.dndu = dndus;
			shading.dndv = dndvs;
			while (Length(shading.dpdu) * Length(shading.dpdu) > 1e16f ||
				Length(shading.dpdv) *Length(shading.dpdv) > 1e16f) {
				shading.dpdu = shading.dpdu / 1e8f;
				shading.dpdv = shading.dpdv / 1e8f;
			}
		}
	};
	struct ShapeIntersection
	{
		SurfaceInteraction Intr;
		float THit = -1.f;
	};
}