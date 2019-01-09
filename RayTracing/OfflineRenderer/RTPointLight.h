#pragma once
#include "RTLight.h"

template<typename PointLightType, typename IntersectionType>
class RTPointLight :public RTLight<IntersectionType>
{
public:
	RTPointLight() :LightSource(nullptr) { }
	PointLightType* GetLightSource() { return LightSource; }
	void SetLightSource(PointLightType *InLightSource) { LightSource = InLightSource; }
	BWVector3D GetLightDir(const IntersectionType *Intersection) 
	{
		BWVector3D LightDir;
		LightDir = GetDirection(Intersection->IntersectionPoint, LightSource->GetPosition());
		return LightDir;
	}

	bool IsDeltaLight() { return true; }
	Spectrum Sample_L(const IntersectionType *Intersection, const LightSample &InLightSample, BWVector3D &LightDir, float &Pdf) override
	{
		Spectrum Color;
		return Color;
	}
	float Pdf(const BWVector3D &P, const BWVector3D &Wi) override
	{
		return 0;
	}
	Spectrum Le(const IntersectionType *Intersection) override
	{
		Spectrum Color;
		return Color;
	}
	
private:
	PointLightType *LightSource;
};