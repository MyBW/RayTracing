#pragma once
#include "Integrator.h"
#include "RTDirectionLight.h"

template<typename SceneType, typename IntersectionType>
class DirectLightingIntegrator : public Integrator<SceneType, IntersectionType>
{
public:
	Spectrum Li(SceneType *InScene, IntersectionType *Intersction)
	{
		Spectrum Color;
		for (int i = 0;i< AllLights.size() ;i++)
		{
			Color += AllLights[i].Sample_L(Intersction);
		}
		/*RTDirectionLight<Light, IntersectionType> DirectionLight;
		DirectionLight.SetLightSource(InScene->GetLightByName(std::string("DirectionalLight")));
		Spectrum Color = DirectionLight.Sample_L(Intersction);
		return Color;*/
	}
};