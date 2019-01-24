#pragma once
#include "RTDirectionLight.h"
#include "RTPointLight.h"
#include "RTAreaLight.h"
class Sample;
class RNG;
template<typename SceneType>
class Integrator
{
public:
	virtual void Init(SceneType *InScene);
	virtual void RequestSample(Sample &InSample){ }
	virtual Spectrum Li(SceneType *InScene, IntersectionInfo *Intersction , Sample &InSample, RNG& Rng) { return Spectrum(); };
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
		for (auto Ele : AreaLights  )
		{
			delete Ele;
		}
	}
protected:
	std::vector<RTDirectionLight< typename SceneType::DirectionLightType>*> DirectionLights;
	std::vector<RTPointLight< typename SceneType::PointLightType>*> PointLights;
	std::vector<RTAreaLight<typename SceneType::AreaLightType>*> AreaLights;
	std::vector<RTLight*> AllLights;
};

template<typename SceneType>
void Integrator<SceneType>::Init(SceneType *InScene)
{
	std::vector<SceneType::DirectionLightType*>& SceneDirectionLights = InScene->GetAllDireciontLight();
	for (int i = 0; i < SceneDirectionLights.size(); i++)
	{
		DirectionLights.push_back(new RTDirectionLight<SceneType::DirectionLightType>());
		AllLights.push_back(DirectionLights[i]);
		DirectionLights[i]->SetLightSource(SceneDirectionLights[i]);
		DirectionLights[i]->SetRadiance(SceneDirectionLights[i]->GetRadiance());
	}
	std::vector<SceneType::PointLightType*>& ScenePointLights = InScene->GetAllPointLight();
	for (int i = 0; i < ScenePointLights.size(); i++)
	{
		PointLights.push_back(new RTPointLight<SceneType::PointLightType>());
		AllLights.push_back(PointLights[i]);
		PointLights[i]->SetLightSource(ScenePointLights[i]);
	}
	std::vector<SceneType::AreaLightType*>& SceneAreaLights = InScene->GetAllAreaLight();
	for (int i = 0; i < SceneAreaLights.size(); i++)
	{
		for (int n = 0 ;n < SceneAreaLights[i]->GetAllObject().size(); n++)
		{
			SceneType::AreaLightType::AreaLightShapeType* MeshData = SceneAreaLights[i]->GetAllObject()[n];
			int TriangleNum = MeshData->GetTriangleNum();
			const std::vector<BWVector3D> &PosData = MeshData->GetWorldPosData();
			const std::vector<BWVector3D> &NormalData = MeshData->GetWorldNormalData();
			const std::vector<BWPoint2DD> &UVData = MeshData->GetUVData();
			std::vector<BWVector3D> Pos;
			std::vector<BWVector3D> Normal;
			std::vector<BWPoint2DD> UV;
			
			for (int m = 0 ;m< TriangleNum ; m++)
			{
				TriganleLightShape*Mesh = new TriganleLightShape();
				Pos.resize(3);
				Normal.resize(3);
				UV.resize(3);
				Pos[0] = PosData[m * 3];
				Pos[1] = PosData[m * 3 + 1];
				Pos[2] = PosData[m * 3 + 2];
				Normal[0] = NormalData[m * 3];
				Normal[1] = NormalData[m * 3 + 1];
				Normal[2] = NormalData[m * 3 + 2];
				UV[0] = UVData[m * 2];
				UV[1] = UVData[m * 2 + 1];
				Mesh->SetTriangleShape(Pos, Normal, UV);
				AreaLights.push_back(new RTAreaLight<SceneType::AreaLightType>());
				AllLights.push_back(AreaLights[m]);
				AreaLights[m]->SetLightSource(SceneAreaLights[i]);
				AreaLights[m]->SetEmit(SceneAreaLights[i]->GetEmitColor());
				AreaLights[m]->SetShape(Mesh);
				Pos.clear();
				Normal.clear();
				UV.clear();
			}
		}
	}
}
