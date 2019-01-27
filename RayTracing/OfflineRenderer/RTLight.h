#pragma once
#include "BWPrimitive.h"
class LightSample;
class VisibleTester;
struct IntersectionInfo;
class RTLight
{
public:
	virtual bool IsDeltaLight() = 0;
	virtual Spectrum L(const IntersectionInfo *Intersection, const BWVector3D &PInLight, const BWVector3D &NInLight) = 0;
	virtual Spectrum Sample_L(const IntersectionInfo *Intersection , const LightSample &InLightSample, BWVector3D &LightDir , float &Pdf, VisibleTester &VisibleTest) = 0;
	virtual float Pdf(const BWVector3D &P, const BWVector3D &Wi) = 0;
	virtual Spectrum Power() = 0;
	virtual Spectrum Sample_Le(const BWPoint2DD &U, const BWPoint2DD &V, BWRay &Ray, BWVector3D &Normal, float &PosPdf, float &DirPdf) = 0;
};

