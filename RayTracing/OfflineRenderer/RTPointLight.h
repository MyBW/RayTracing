#pragma once
#include "RTLight.h"

template<typename PointLightType, typename IntersectionType>
class RTPointLight :public RTLight<IntersectionType>
{
public:
	RTDirectionLight() :LightSource(nullptr) { }
	PointLightType* GetLightSource() { return LightSource; }
	PointLightType* SetLightSource(PointLightType *InLightSource) { LightSource = InLightSource; }
	Spectrum Sample_L(const IntersectionType *Intersection)
	{
		Spectrum Color;
		return Color;
	}
private:
	PointLightType *LightSource;
};