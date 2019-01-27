#pragma once
#include "RTLight.h"
#include "Renderer.h"
#include "Montecarlo.h"
#include "RTAreaLightShape.h"
#include <functional>
template<typename AreaLightType>
class RTAreaLight : public RTLight
{
public:
	~RTAreaLight()
	{
		delete EmitShape;
	}
	void SetShape(Shape *InShape) { EmitShape = InShape; }
	void SetEmit(const Spectrum &InEmit)
	{
		Emit = InEmit;
	}
	bool IsDeltaLight() { return false; }
	Spectrum Power() {  return Emit * EmitShape->Area(); }
	Spectrum L(const IntersectionInfo *Intersection, const BWVector3D &PInLight, const BWVector3D &NInLight) override
	{
		float CosTheta = Dot(GetDirection(PInLight, Intersection->IntersectionPoint), NInLight);
		if (CosTheta > 0)
		{
			return Emit;
		}
		return 0.0f;
	}
	float Pdf(const BWVector3D &P, const BWVector3D &Wi)
	{
		return EmitShape->Pdf(P, Wi);
	}
	Spectrum Sample_L(const IntersectionInfo *Intersection, const LightSample &InLightSample, BWVector3D &LightDir, float &Pdf, VisibleTester &VisibleTest)
	{
		BWPoint2DD U(InLightSample.Pos[0], InLightSample.Pos[1]);
		BWVector3D NInLight;
		BWVector3D PInLight = EmitShape->Sample(Intersection->IntersectionPoint, U, Pdf, NInLight);
		if (Pdf == 0.0f  || Lenth(Intersection->IntersectionPoint - PInLight ) == 0.0f)
		{
			Pdf = 0.0f;
			return 0.0f;
		}
		LightDir = GetDirection(Intersection->IntersectionPoint, PInLight);
		VisibleTest.SetSegment(Intersection->IntersectionPoint, PInLight);
		return L(Intersection, PInLight, NInLight);
	}
	bool Intersection(const BWRay &Ray, BWVector3D &PInLight, BWVector3D &NInLight, float &Thit)
	{
		Thit = Ray.Length;
		auto InterCallBack = [&](const std::vector<BWPoint3DD>& InP, const std::vector<BWPoint3DD>& InN, const std::vector<BWPoint2DD>& UV, float t, float u, float v, const BWRay &InRay, const RTMaterial*Material, bool &IsBreak)
		{
			if (t > ESP && t < Thit - ESP)
			{
				Thit = t;
				PInLight = Ray._start + Ray._vector * Thit;
				NInLight = LinearInterpolation(InN[0], InN[1], u);
				NInLight = LinearInterpolation(NInLight, InN[2], v);
				NInLight.normalize();
			}
		};
		return  EmitShape->Intersect(Ray, InterCallBack);
	}
	void SetLightSource(AreaLightType *SrcAreaLight)
	{
		this->SrcAreaLight = SrcAreaLight;
	}
	Spectrum Sample_Le(const BWPoint2DD &U, const BWPoint2DD &V, BWRay &Ray, BWVector3D &Normal, float &PosPdf, float &DirPdf) override;
private:
	AreaLightType *SrcAreaLight;
	Spectrum Emit;
	Shape *EmitShape;
};


template<typename AreaLightType>
Spectrum RTAreaLight<AreaLightType>::Sample_Le(const BWPoint2DD &U, const BWPoint2DD &V, BWRay &Ray, BWVector3D &Normal, float &PosPdf, float &DirPdf)
{
	EmitShape->Sample(U, PosPdf, Normal);
	Normal.normalize();
	BWVector3D w = ConsineSampleHemisphere(V.x, V.y);
	w.normalize();
	DirPdf = CosineHemispherePdf(w.z);

	BWVector3D v1, v2, n(Normal);
	CoordinateSystem(n, &v1, &v2);
	w = w.x * v1 + w.y * v2 + w.z * n;
	Ray._start = PosPdf;
	Ray._vector = w;
	Ray.Length = FLT_MAX;
	return Dot(w, Normal) > 0.0f ? Emit : Spectrum(0.0f);
}
