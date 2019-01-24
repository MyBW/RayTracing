#pragma once
#include "RTLight.h"

template<typename DirectionLightType>
class RTDirectionLight : public RTLight
{
public:
	RTDirectionLight():LightSource(nullptr){ }

	
	void SetRadiance(const Spectrum &Radiance) { Lo = Radiance; }
	void SetWorldRadius(const float Radius) { WorldRadius = Radius; }
	DirectionLightType* GetLightSource() { return LightSource; }
	void SetLightSource(DirectionLightType *InLightSource) { LightSource = InLightSource; }
	Spectrum L(const IntersectionInfo *Intersection, const BWVector3D &PInLight, const BWVector3D &NInLight) override
	{
		return Lo;
	}
	bool IsDeltaLight() { return true; }
	Spectrum Sample_L(const IntersectionInfo *Intersection, const LightSample &InLightSample, BWVector3D& LightDir, float &Pdf, VisibleTester &VisibleTest) override
	{
		LightDir = LightSource->GetDirection() * -1;
		VisibleTest.SetRay(Intersection->IntersectionPoint, LightDir);
		Pdf = 1.0f;
		return Lo;
	}
	Spectrum Power() override
	{
		return Lo * PI * WorldRadius * WorldRadius;
	}
	float Pdf(const BWVector3D &P, const BWVector3D &Wi) override
	{
		return 0.0;
	}
private:
	DirectionLightType *LightSource;
	Spectrum Lo;
	float WorldRadius;
};
