#pragma once
#include "RTLight.h"

template<typename PointLightType, typename IntersectionType>
class RTPointLight :public RTLight<IntersectionType>
{
public:
	RTPointLight() :LightSource(nullptr) { }
	PointLightType* GetLightSource() { return LightSource; }
	void SetLightSource(PointLightType *InLightSource) { LightSource = InLightSource; }
	Spectrum Sample_L(const IntersectionType *Intersection, BWVector3D &LightDir) override
	{
		Spectrum Color;
		return Color;
	}
	Spectrum Le(const IntersectionType *Intersection) override
	{
		Spectrum Color;
		return Color;
	}
private:
	PointLightType *LightSource;
};