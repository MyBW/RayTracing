#pragma once
#include "RTLight.h"

template<typename PointLightType, typename IntersectionType>
class RTPointLight :public RTLight<IntersectionType>
{
public:
	RTPointLight() :LightSource(nullptr) { }

	void SetIndtensity(const Spectrum& Intensity) { Lo = Intensity; }
	PointLightType* GetLightSource() { return LightSource; }
	void SetLightSource(PointLightType *InLightSource) { LightSource = InLightSource; }
	bool IsDeltaLight() { return true; }
	Spectrum Sample_L(const IntersectionType *Intersection, const LightSample &InLightSample, BWVector3D &LightDir, float &Pdf, VisibleTester &VisibleTest) override
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
	Spectrum L(const IntersectionType *Intersection, const BWVector3D &PInLight, const BWVector3D &NInLight) override
	{
		return Lo / Power(Lenth(Intersection->IntersectionPoint, LightSource->GetPosition()));
	}
	
private:
	PointLightType *LightSource;
	Spectrum Lo;
};