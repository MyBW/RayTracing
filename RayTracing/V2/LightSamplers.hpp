#pragma  once
#include "Base\LightSampler.hpp"
#include "Base\Light.hpp"
namespace BlackWalnut
{
	class UniformLightSampler : public LightSamplerBase
	{
	public:

		UniformLightSampler(std::vector<LightBase*> AllLights) :AllLights(AllLights)
		{

		}
		SampledLight Sample(float u) const override
		{
			if (AllLights.size() == 0) return{};
			uint32_t LightIndxe = std::min<int>(u * AllLights.size(), AllLights.size() - 1);
			return SampledLight(AllLights[LightIndxe], 1.f / AllLights.size());
		}
		float PDF(LightBase* Light) const
		{
			if (AllLights.size() == 0) return 0;
			return 1.0f / AllLights.size();
		}
		SampledLight Sample(const LightSampleContext &ctx, float U) const override
		{
			return Sample(U);
		}
		float PDF(const LightSampleContext &Ctx, LightBase *Light) const override
		{
			return PDF(Light);
		}
	private:
		std::vector<LightBase*> AllLights;
	};
}