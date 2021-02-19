#include "Interaction.hpp"
#include "Math\Math.hpp"
#include "Math\MatrixMath.hpp"
#include "Materials.hpp"
#include "Cameras.hpp"
#include "Base/Light.hpp"
namespace BlackWalnut
{
	BSDF SurfaceInteraction::GetBSDF(const RayDifferential& Ray, SampledWavelengths &Lambda, CameraBase* InCamera, SamplerBase* Sampler)
	{
		ComputeDifferentials(Ray, InCamera);
		/*while (material.Is<MixMaterial>()) {
			MixMaterial *mix = material.CastOrNullptr<MixMaterial>();
			material = mix->ChooseMaterial(UniversalTextureEvaluator(), *this);
		}*/
		if (!material)
			return{};
		// Evaluate bump map and compute shading normal
		FloatTextureBase* displacement = material->GetDisplacement();
		if (displacement) {
			Vector3f dpdu, dpdv;
			Bump(UniversalTextureEvaluator(), displacement, *this, &dpdu, &dpdv);
			Vector3f Temp;
			CrossProduct(Temp,dpdu, dpdv);
			Normalize(Temp);
			SetShadingGeometry(Temp, dpdu, dpdv,
				shading.dndu, shading.dndv, false);
		}

		// Return BSDF for surface interaction
		BxDFBase* scratchBuffer = nullptr;
		BSDF bsdf =
			material->GetBSDF(UniversalTextureEvaluator(), *this, Lambda, scratchBuffer);
		bool ForceDiffuse = false;
		if (bsdf && ForceDiffuse) {
			SampledSpectrum r = bsdf.RHO(wo, { Sampler->Get1D() }, { Sampler->Get2D() });
			bsdf = BSDF(wo, n, shading.n, shading.dpdu,
				new IdealDiffuseBxDF(r), bsdf.Eta);
		}
		return bsdf;
	}

	void SurfaceInteraction::ComputeDifferentials(const RayDifferential &ray, CameraBase* camera) const
	{
		if (ray.HasDifferentials && Dot(n, ray.RxDirection) != 0 &&
			Dot(n, ray.RyDirection) != 0) {
			// Estimate screen-space change in $\pt{}$
			// Compute auxiliary intersection points with plane, _px_ and _py_
			float d = -Dot(n, Vector3f(P()));
			float tx = (-Dot(n, Vector3f(ray.RxOrigin)) - d) / Dot(n, ray.RxDirection);
			CHECK(!IsInf(tx) && !IsNaN(tx));
			Vector3f px = ray.RxOrigin +  ray.RxDirection * tx;
			float ty = (-Dot(n, Vector3f(ray.RyOrigin)) - d) / Dot(n, ray.RyDirection);
			CHECK(!IsInf(ty) && !IsNaN(ty));
			Vector3f py = ray.RyOrigin + ray.RyDirection *  ty;

			dpdx = px - P();
			dpdy = py - P();

		}
		else
			camera->ApproximatedPdxy(*this);
		// Estimate screen-space change in $(u,v)$
		float a00 = Dot(dpdu, dpdu), a01 = Dot(dpdu, dpdv), a11 = Dot(dpdv, dpdv);
		float invDet = 1 / (DifferenceOfProducts(a00, a11, a01, a01));

		float b0x = Dot(dpdu, dpdx), b1x = Dot(dpdv, dpdx);
		float b0y = Dot(dpdu, dpdy), b1y = Dot(dpdv, dpdy);

		/* Set the UV partials to zero if dpdu and/or dpdv == 0 */
		invDet = std::isfinite(invDet) ? invDet : 0.f;

		dudx = DifferenceOfProducts(a11, b0x, a01, b1x) * invDet;
		dvdx = DifferenceOfProducts(a00, b1x, a01, b0x) * invDet;

		dudy = DifferenceOfProducts(a11, b0y, a01, b1y) * invDet;
		dvdy = DifferenceOfProducts(a00, b1y, a01, b0y) * invDet;

		dudx = std::isfinite(dudx) ? Clamp(dudx, -1e8f, 1e8f) : 0.f;
		dvdx = std::isfinite(dvdx) ? Clamp(dvdx, -1e8f, 1e8f) : 0.f;
		dudy = std::isfinite(dudy) ? Clamp(dudy, -1e8f, 1e8f) : 0.f;
		dvdy = std::isfinite(dvdy) ? Clamp(dvdy, -1e8f, 1e8f) : 0.f;
	}

	void SurfaceInteraction::SkipIntersection(RayDifferential *InRay, float T) const
	{
		*((Ray *)InRay) = SpawnRay(InRay->D);
		if (InRay->HasDifferentials) {
			InRay->RxOrigin = InRay->RxOrigin +  InRay->RxDirection * T;
			InRay->RyOrigin = InRay->RyOrigin +  InRay->RyDirection * T;
		}
	}

	SampledSpectrum SurfaceInteraction::Le(const Vector3f &W, const SampledWavelengths &Lambda) const
	{
		return AreaLightSrc ? AreaLightSrc->L(P(), n, uv, W, Lambda) : SampledSpectrum(0.f);
	}

}


