#pragma once
#include "Integrator.h"

class DirectLightingIntegrator : public Integrator
{
public:
	template<typename SceneType, typename IntersectionType >
	Spectrum Li(SceneType *InScene, IntersectionType *Intersction)
	{
		OfflineDirectionLight<Light> DirectionLight;
		DirectionLight.SetLightSource(InScene->GetLightByName(std::string("DirectionalLight")));
		Spectrum Color = DirectionLight.Sample_L(Intersction);
		return Color;
	}
};