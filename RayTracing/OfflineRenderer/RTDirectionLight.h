#pragma once
#include "RTLight.h"

template<typename DirectionLightType, typename IntersectionType>
class RTDirectionLight : public RTLight<IntersectionType>
{
public:
	RTDirectionLight():LightSource(nullptr){ }
	DirectionLightType* GetLightSource() { return LightSource; }
	BWVector3D GetLightDir(const IntersectionType *Intersection)
	{
		BWVector3D LightDir;
		LightDir = GetDirection(Intersection->IntersectionPoint, LightSource->GetPosition());
		return LightDir;
	}
	void SetLightSource(DirectionLightType *InLightSource) { LightSource = InLightSource; }

	Spectrum Le(const IntersectionType *Intersection) override
	{
		Spectrum Color;
		Color.SetValue(0, 1);
		return Color;
	}
	bool IsDeltaLight() { return true; }
	Spectrum Sample_L(const IntersectionType *Intersection, const LightSample &InLightSample, BWVector3D& LightDir) override
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
