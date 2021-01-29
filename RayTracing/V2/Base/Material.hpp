#pragma once
#include "Textures.hpp"
namespace BlackWalnut
{
	struct MaterialEvalContext : public TextureEvalContext 
	{
		MaterialEvalContext() = default;
		MaterialEvalContext(const SurfaceInteraction &SI)
			:TextureEvalContext(SI),Wo(SI.wo),N(SI.n),NS(SI.shading.n),DpDus(SI.shading.dpdu)
		{

		}
		Vector3f Wo;
		Vector3f N;
		Vector3f NS;
		Vector3f DpDus;
	};
	class MaterialBase
	{
	public:
		
		virtual bool CanEvaluateTexture(UniversalTextureEvaluator TexEval) const = 0;
		
		virtual BSDF GetBSDF(UniversalTextureEvaluator TexEval, MaterialEvalContext Ctx, SampledWavelengths &Lambd, BxDFBase *InBxDF) const = 0;

		/*template<typename TextureEvaluator>
		virtual BSDF GetBSSRDF(TextureEvaluator TexEval, MaterialEvalContext Ctx, SampledWavelengths &Lambd, BxDFBase *InBxDF) const = 0;*/
		virtual FloatTextureBase* GetDisplacement() const = 0;
		virtual bool IsTransparent() const = 0;
		//virtual bool HasSubsurfaceScattering() const = 0;
	};
}