#pragma once
#include "BWPrimitive.h"
class LightSample;
class VisibleTester;
template<typename IntersectionType>
class RTLight
{
public:
	virtual bool IsDeltaLight() = 0;
	virtual Spectrum L(const IntersectionType *Intersection, const BWVector3D &PInLight, const BWVector3D &NInLight) = 0;
	virtual Spectrum Sample_L(const IntersectionType *Intersection , const LightSample &InLightSample, BWVector3D &LightDir , float &Pdf, VisibleTester &VisibleTest) = 0;
	virtual float Pdf(const BWVector3D &P, const BWVector3D &Wi) = 0;
	virtual Spectrum Power() = 0;
};

