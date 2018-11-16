#pragma once
#include "RTLight.h"

template<typename DirectionLightType, typename IntersectionType>
class OfflineDirectionLight : public OfflineLight<IntersectionType>
{
public:
	OfflineDirectionLight():LightSource(nullptr){ }
	DirectionLightType* GetLightSource() { return LightSource; }
	DirectionLightType* SetLightSource(DirectionLightType *InLightSource) { LightSource = InLightSource; }

	Spectrum Sample_L(const IntersectionType *Intersection)
	{
		if (!LightSource) return Spectrum(0.0);
		BWVector3D LightDir = GetDirection(Intersection->IntersectionPoint, LightSource->GetPosition());
		BWVector3D ViewDir = Intersection->InputRay._vector;
		BWVector3D Normal = Intersection->IntersectionNormal;
		double LDotN = Dot(LightDir, Normal);
		Spectrum Color;
		if (LDotN < 0) LDotN = 0;
		Color.SetValue(0, LDotN);
		return Color;
	}
private:
	DirectionLightType *LightSource;
};