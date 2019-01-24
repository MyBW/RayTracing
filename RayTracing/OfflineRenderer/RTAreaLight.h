#pragma once
#include "RTLight.h"
#include "Renderer.h"
#include "Montecarlo.h"
#include "RTAreaLightShape.h"
#include <functional>
template<typename AreaLightType, typename IntersectionInfoType>
class RTAreaLight : public RTLight<IntersectionInfoType>
{
public:
	~RTAreaLight()
	{
		delete ShapeSetData;
	}
	void SetShapeSet(ShapeSet *ShapeSetData) { this->ShapeSetData = ShapeSetData; }

	void SetShape(Shape *InShape) { EmitShape = InShape; }
	void SetEmit(const Spectrum &InEmit)
	{
		Emit = InEmit;
	}
	bool IsDeltaLight() { return false; }
	Spectrum Power() {  return Emit * ShapeSetData->GetArea(); }
	Spectrum L(const IntersectionInfoType *Intersection, const BWVector3D &PInLight, const BWVector3D &NInLight) override
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
	Spectrum Sample_L(const IntersectionInfoType *Intersection, const LightSample &InLightSample, BWVector3D &LightDir, float &Pdf, VisibleTester &VisibleTest)
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
private:
	AreaLightType *SrcAreaLight;
	Spectrum Emit;
	ShapeSet *ShapeSetData;
	Shape *EmitShape;
};


