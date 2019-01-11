#pragma once

template<typename SceneType, typename IntersectionType>
class TestIntegrator : public Integrator<SceneType, IntersectionType>
{
public:
	Spectrum Li(SceneType *InScene, IntersectionType *Intersction, Sample &InSample, RNG &Rng) override
	{
		//¼ÆËã»ý·Ö
		Spectrum Color;
		BWVector3D LightDir;
		for (int i = 0; i < AllLights.size(); i++)
		{
			Spectrum LightColor;
			BSDF Bsdf;
			Intersction->Material->CreateBSDF(*Intersction, Bsdf);
			LightColor = AllLights[i]->Le(Intersction);
			LightDir = AllLights[i]->GetLightDir(Intersction);
			Color += LightColor * Bsdf.F(Intersction->InputRay._vector, LightDir);
		}
		return Color;
	}
};
