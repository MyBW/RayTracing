#pragma once
#include "RTDirectionLight.h"
#include "RTPointLight.h"
class Sample;
template<typename SceneType, typename IntersectionType>
class Integrator
{
public:
	virtual void Init(SceneType *InScene);
	virtual void RequestSample(Sample &InSample){ }
	virtual Spectrum Li(SceneType *InScene, IntersectionType *Intersction , Sample &InSample, RNG& Rng) { return Spectrum(); };
	~Integrator()
	{
		for (auto Ele : DirectionLights)
		{
			delete Ele;
		}
		for (auto Ele : PointLights)
		{
			delete Ele;
		}
	}
protected:
	std::vector<RTDirectionLight< typename SceneType::DirectionLightType, IntersectionType>*> DirectionLights;
	std::vector<RTPointLight< typename SceneType::PointLightType, IntersectionType>*> PointLights;
	std::vector<RTAreaLight<typename SceneType::AreaLightType, IntersectionType>*> AreaLights;
	std::vector<RTLight<IntersectionType>*> AllLights;
private:
};

template<typename SceneType, typename IntersectionType>
void Integrator<SceneType, IntersectionType>::Init(SceneType *InScene)
{
	std::vector<SceneType::DirectionLightType*>& SceneDirectionLights = InScene->GetAllDireciontLight();
	for (int i = 0; i < SceneDirectionLights.size(); i++)
	{
		DirectionLights.push_back(new RTDirectionLight<SceneType::DirectionLightType, IntersectionType>());
		AllLights.push_back(DirectionLights[i]);
		DirectionLights[i]->SetLightSource(SceneDirectionLights[i]);
		DirectionLights[i]->SetRadiance(SceneDirectionLights[i]->GetRadiance());
	}
	std::vector<SceneType::PointLightType*>& ScenePointLights = InScene->GetAllPointLight();
	for (int i = 0; i < ScenePointLights.size(); i++)
	{
		PointLights.push_back(new RTPointLight<SceneType::PointLightType, IntersectionType>());
		AllLights.push_back(PointLights[i]);
		PointLights[i]->SetLightSource(ScenePointLights[i]);
	}
}
