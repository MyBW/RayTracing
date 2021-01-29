#pragma once
#include "Base\Material.hpp"
#include "Base\Textures.hpp"
#include "BxDFs.hpp"
namespace BlackWalnut
{

	// BumpEvalContext Definition
	struct BumpEvalContext {
		BumpEvalContext() = default;
	
		BumpEvalContext(const SurfaceInteraction &si)
			: p(si.P()),
			uv(si.uv),
			dudx(si.dudx),
			dudy(si.dudy),
			dvdx(si.dvdx),
			dvdy(si.dvdy),
			dpdx(si.dpdx),
			dpdy(si.dpdy),
			faceIndex(si.faceIndex) {
			shading.n = si.shading.n;
			shading.dpdu = si.shading.dpdu;
			shading.dpdv = si.shading.dpdv;
			shading.dndu = si.shading.dndu;
			shading.dndv = si.shading.dndv;
		}

		
		operator TextureEvalContext() const {
			return TextureEvalContext(p, dpdx, dpdy, uv, dudx, dudy, dvdx, dvdy, faceIndex);
		}

		Vector3f p;
		Vector2f uv;
		struct {
			Vector3f n;
			Vector3f dpdu, dpdv;
			Vector3f dndu, dndv;
		} shading;
		float dudx = 0, dudy = 0, dvdx = 0, dvdy = 0;
		Vector3f dpdx, dpdy;
		int faceIndex = 0;
	};

	template <typename TextureEvaluator>
	void Bump(TextureEvaluator texEval, FloatTextureBase* displacement,
		const BumpEvalContext &si, Vector3f *dpdu, Vector3f *dpdv) {
		CHECK(displacement != nullptr);
		CHECK(texEval.CanEvaluate({ displacement }, {}));
		// Compute offset positions and evaluate displacement texture
		TextureEvalContext shiftedCtx = si;
		// Shift _shiftedCtx_ _du_ in the $u$ direction
		float du = .5f * (std::abs(si.dudx) + std::abs(si.dudy));
		if (du == 0)
			du = .0005f;
		shiftedCtx.p = si.p +  si.shading.dpdu * du;
		shiftedCtx.uv = si.uv + Vector2f(du, 0.f);

		float uDisplace = texEval(displacement, shiftedCtx);
		// Shift _shiftedCtx_ _dv_ in the $v$ direction
		float dv = .5f * (std::abs(si.dvdx) + std::abs(si.dvdy));
		if (dv == 0)
			dv = .0005f;
		shiftedCtx.p = si.p +  si.shading.dpdv * dv;
		shiftedCtx.uv = si.uv + Vector2f(0.f, dv);

		float vDisplace = texEval(displacement, shiftedCtx);
		float displace = texEval(displacement, si);

		// Compute bump-mapped differential geometry
		*dpdu = si.shading.dpdu + (uDisplace - displace) / du * Vector3f(si.shading.n) +
			displace * Vector3f(si.shading.dndu);
		*dpdv = si.shading.dpdv + (vDisplace - displace) / dv * Vector3f(si.shading.n) +
			displace * Vector3f(si.shading.dndv);
	}


	class DiffuseMaterial : public MaterialBase
	{
	public:
		using BxDF = DiffuseBxDF;
		using BSSRDF = void;

		DiffuseMaterial(SpectrumTextureBase* Reflectance, FloatTextureBase* Sigma, FloatTextureBase* Displacement)
			:Reflectance(Reflectance), Displacement(Displacement), Sigma(Sigma)
		{

		}
		FloatTextureBase* GetDisplacement() const override { return Displacement; }
		bool CanEvaluateTexture(UniversalTextureEvaluator TexEval) const override 
		{
			return TexEval.CanEvaluate({ Sigma }, { Reflectance });
		}
		bool IsTransparent() const override { return false; }
		static bool HasSubsurfaceScattering() { return false; }

		template<typename TextureEvaluator>
		bool CanEvaluateTexture(TextureEvaluator TexEval) const
		{
			return TexEval.CanEvaluate({ Sigma }, { Reflectance });
		}
		BSDF GetBSDF(UniversalTextureEvaluator TexEval, MaterialEvalContext Ctx, SampledWavelengths &Lambd, BxDFBase *InBxDF) const override
		{
			// Evaluate textures for _DiffuseMaterial_ and allocate BSDF
			SampledSpectrum r = ClampSampledSpectrum(TexEval(Reflectance, Ctx, Lambd), 0.0f, 1.0);
			float sig = Clamp(TexEval(Sigma, Ctx), 0, 90);
			*dynamic_cast<DiffuseBxDF*>(InBxDF) = DiffuseBxDF(r, SampledSpectrum(0), sig);
			return BSDF(Ctx.Wo, Ctx.N, Ctx.NS, Ctx.DpDus, InBxDF);
		}
	private:
		FloatTextureBase* Displacement;
		SpectrumTextureBase* Reflectance;
		FloatTextureBase* Sigma;
	};

	class ConductorMaterial : public MaterialBase
	{
	public:
		bool CanEvaluateTexture(UniversalTextureEvaluator TexEval) const override
		{
			return TexEval.CanEvaluate({ uRoughness, vRoughness }, { eta, k });
		}
		BSDF GetBSDF(UniversalTextureEvaluator TexEval, MaterialEvalContext Ctx, SampledWavelengths &Lambda, BxDFBase *InBxDF) const override
		{
			
			float uRough = TexEval(uRoughness, Ctx), vRough = TexEval(vRoughness, Ctx);
			if (remapRoughness) {
				uRough = TrowbridgeReitzDistribution::RoughnessToAlpha(uRough);
				vRough = TrowbridgeReitzDistribution::RoughnessToAlpha(vRough);
			}
			SampledSpectrum etas = TexEval(eta, Ctx, Lambda);
			SampledSpectrum ks = TexEval(k, Ctx, Lambda);

			TrowbridgeReitzDistribution distrib(uRough, vRough);
			*(dynamic_cast<ConductorBxDF*>(InBxDF)) = ConductorBxDF(distrib, etas, ks);
			return BSDF(Ctx.Wo, Ctx.N, Ctx.NS, Ctx.DpDus, InBxDF);
		}

		ConductorMaterial(SpectrumTextureBase* eta, SpectrumTextureBase* k,
			FloatTextureBase* uRoughness, FloatTextureBase* vRoughness,
			FloatTextureBase* displacement, bool remapRoughness)
			: displacement(displacement),
			eta(eta),
			k(k),
			uRoughness(uRoughness),
			vRoughness(vRoughness),
			remapRoughness(remapRoughness) {}
		FloatTextureBase* GetDisplacement() const { return displacement; }

		bool IsTransparent() const { return false; }
		static constexpr bool HasSubsurfaceScattering() { return false; }
	private:
		FloatTextureBase *displacement;
		SpectrumTextureBase *eta;
		SpectrumTextureBase *k;
		FloatTextureBase* uRoughness;
		FloatTextureBase* vRoughness;
		bool remapRoughness;
	};
}