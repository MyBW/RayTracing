#pragma once
#include "RTLight.h"

template<typename DirectionLightType, typename IntersectionType>
class RTDirectionLight : public RTLight<IntersectionType>
{
public:
	RTDirectionLight():LightSource(nullptr){ }
	DirectionLightType* GetLightSource() { return LightSource; }
	void SetLightSource(DirectionLightType *InLightSource) { LightSource = InLightSource; }

	Spectrum Sample_L(const IntersectionType *Intersection , BWVector3D& LightDir)
	{
		if (!LightSource) return Spectrum(0.0);
		LightDir = GetDirection(Intersection->IntersectionPoint, LightSource->GetPosition());
		Spectrum Color;
		Color.SetValue(0, 1.0);
		Color.SetValue(1, 0);
		Color.SetValue(2, 0);
		return Color;
	}
private:
	DirectionLightType *LightSource;
};