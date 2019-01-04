#pragma once
#include "Integrator.h"

template<typename SceneType, typename IntersectionType>
class DirectLightingIntegrator : public Integrator<SceneType, IntersectionType>
{
public:
	Spectrum Li(SceneType *InScene, IntersectionType *Intersction)
	{
		//�������
		Spectrum Color;
		BWVector3D LightDir;
		for (int i = 0;i< AllLights.size() ;i++)
		{
			//�����ƹ�
			Color = AllLights[i]->Sample_L(Intersction, LightDir);
			//��������
			Color = Color * Intersction->Material.GetLightingModel().F(Intersction->InputRay._vector, LightDir);
			/*BWVector3D ViewDir = Intersection->InputRay._vector;
			BWVector3D Normal = Intersection->IntersectionNormal;
			double LDotN = Dot(LightDir, Normal);
			if (LDotN < 0) LDotN = 0;*/
		}
		return Color;
		/*RTDirectionLight<Light, IntersectionType> DirectionLight;
		DirectionLight.SetLightSource(InScene->GetLightByName(std::string("DirectionalLight")));
		Spectrum Color = DirectionLight.Sample_L(Intersction);
		return Color;*/
	}
};