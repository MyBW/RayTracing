#pragma once
#include "Integrator.h"

template<typename SceneType, typename IntersectionType>
class DirectLightingIntegrator : public Integrator<SceneType, IntersectionType>
{
public:
	Spectrum Li(SceneType *InScene, IntersectionType *Intersction)
	{
		//计算积分
		Spectrum Color;
		BWVector3D LightDir;
		for (int i = 0;i< AllLights.size() ;i++)
		{
			//采样灯光
			Color = AllLights[i]->Sample_L(Intersction, LightDir);
			//采样材质
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