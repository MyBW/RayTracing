#pragma once
namespace BlackWalnut
{
	struct SampledLight
	{
		SampledLight() = default;
		SampledLight(LightBase* Light, float Pdf):Light(Light),Pdf(Pdf) {}
		LightBase *Light = nullptr;
		float Pdf;
	};
	class LightSamplerBase
	{
		virtual SampledLight Sample(const LightSampleContext &Ctx, float U) const = 0;
		virtual float PDF(const LightSampleContext &Ctx, LightBase* Light) const = 0;
		virtual SampledLight Sample(float U) const = 0;
		virtual float PDF(LightBase* Light) const = 0;
	};
}