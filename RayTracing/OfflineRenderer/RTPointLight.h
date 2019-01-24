#pragma once
#include "RTLight.h"

template<typename PointLightType>
class RTPointLight :public RTLight
{
public:
	RTPointLight() :LightSource(nullptr) { }

	void SetIndtensity(const Spectrum& Intensity) { Lo = Intensity; }
	PointLightType* GetLightSource() { return LightSource; }
	void SetLightSource(PointLightType *InLightSource) { LightSource = InLightSource; }
	bool IsDeltaLight() { return true; }
	Spectrum Sample_L(const IntersectionInfo *Intersection, const LightSample &InLightSample, BWVector3D &LightDir, float &Pdf, VisibleTester &VisibleTest) override
	{
		LightDir = GetDirection(Intersection->IntersectionPoint, LightSource->GetPosition());
		VisibleTest.SetSegment(Intersection->IntersectionPoint, LightSource->GetPosition());
		Pdf = 1.0f;
		return Lo / pow(VisibleTest.Ray.Length, 2);
	}
	Spectrum Power() override
	{
		return 4.0f * PI * Lo;
	}
	float Pdf(const BWVector3D &P, const BWVector3D &Wi) override
	{
		return 0;
	}
	Spectrum L(const IntersectionInfo *Intersection, const BWVector3D &PInLight, const BWVector3D &NInLight) override
	{
		return Lo / pow(Lenth(Intersection->IntersectionPoint - LightSource->GetPosition()) , 2.0f);
	}
	
private:
	PointLightType *LightSource;
	Spectrum Lo;
};