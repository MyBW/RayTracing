#pragma once
#include "RTLight.h"
#include "Renderer.h"
#include "Montecarlo.h"

class Shape
{
public:
	float Area();
	BWVector3D Sample(const BWVector3D&p, float u, float v, BWVector3D &Ns);
	BWVector3D Sample(float u, float v, BWVector3D &Ns);
	bool Intersect(const BWRay &Ray, float &THit, BWVector3D HitPoint, BWVector3D &Normal);
	float Pdf(BWVector3D &P, BWVector3D &Wi);
};

class ShapeSet
{
public:
	ShapeSet() :SumArea(0) { }
	~ShapeSet()
	{
		for (auto S : Shapes)
		{
			delete S;
		}
	}
	void AddShape(Shape* InShape)
	{
		if (!InShape) return;
		Shapes.push_back(InShape);
		Areas.push_back(InShape->Area());
		SumArea += InShape->Area();
		AreaDistribute->ResetDistributionData(Areas);
	}
	BWVector3D Sample(const BWVector3D& P, const LightSample &InLightSample, BWVector3D &N) const
	{
		float AreaPdf = 0.f;
		int ShapeIndex = AreaDistribute->SampleDistribute(InLightSample.ShapeComponent, AreaPdf);
		BWVector3D ShapePoint = Shapes[ShapeIndex]->Sample(P, InLightSample.Pos[0], InLightSample.Pos[1], N);
		float THit = Lenth(P - ShapePoint);
		BWVector3D HitNormal;
		BWVector3D HitPoint;
		BWRay Ray(P, GetDirection(P, ShapePoint), THit);
		for (auto ShapeEle : Shapes)
		{
			if (ShapeEle->Intersect(Ray, THit, HitPoint, HitNormal) && THit > ESP && THit < Ray.Length - ESP)
			{
				Ray.Length = THit;
				ShapePoint = HitPoint;
				N = HitNormal;
			}
		}
		return ShapePoint;
	}
	BWVector3D Sample(const LightSample &InLightSample, BWVector3D &N) const
	{
		float AreaPdf = 0.f;
		int ShapeIndex = AreaDistribute->SampleDistribute(InLightSample.ShapeComponent, AreaPdf);
		BWVector3D ShapePoint = Shapes[ShapeIndex]->Sample(InLightSample.Pos[0], InLightSample.Pos[1], N);
		return ShapePoint;
	}
	float GetArea() { return SumArea; }
	float Pdf(BWVector3D &P, BWVector3D &Wi)
	{
		float SumPdf = 0;
		for (size_t i = 0; i < Shapes.size(); i++)
		{
			SumPdf += Shapes[i]->Pdf(P, Wi);
		}
		return SumPdf;
	}	
	bool Intersection(const BWRay &Ray, BWVector3D &PInLight, BWVector3D &NInLight, float Thit)
	{
		float CurThit , MinThit = Ray.Length;
		for (auto ShapeEle : Shapes)
		{
			if (ShapeEle->Intersect(Ray, CurThit, PInLight, NInLight) && CurThit > ESP && CurThit < MinThit - ESP)
			{
				MinThit = CurThit;
			}
		}
		Thit = MinThit;
		return MinThit != Ray.Length;
	}
private:
	std::vector<Shape*> Shapes;
	float SumArea;
	std::vector<float> Areas;
	Distribution1D *AreaDistribute;
}


template<typename AreaLightType, typename IntersectionInfoType>
class RTAreaLight : public RTLight<IntersectionInfoType>
{
public:
	~RTAreaLight()
	{
		delete ShapeSetData;
	}
	void SetShapeSet(ShapeSet *ShapeSetData) { this->ShapeSetData = ShapeSetData; }
	bool SetEmit(Spectrum InEmit)
	{
		Emit = InEmit;
	}
	bool IsDeltaLight() { return false; }
	Spectrum Power() {  return Emit * ShapeSetData->GetArea(); }
	Spectrum L(const IntersectionInfoType *Intersection, const BWVector3D &PInLight, const BWVector3D &NInLight)
	{
		float CosTheta = TMax(0.0, Dot(GetDirection(PInLight, Intersection->IntersectionPoint), NInLight))
		return Emit * CosTheta / pow(Lenth(PInLight - Intersection->IntersectionPoint), 2.0f);
	}
	float Pdf(const BWVector3D &P, const BWVector3D &Wi)
	{
		return ShapeSetData->Pdf(P, Wi);
	}
	Spectrum Sample_L(const IntersectionInfoType *Intersection, const LightSample &InLightSample, BWVector3D &LightDir, float &Pdf, VisibleTester &VisibleTest)
	{
		BWVector3D NInLight;
		BWVector3D PInLight = ShapeSetData->Sample(Intersection->IntersectionPoint, InLightSample, NInLight);
		VisibleTest.SetSegment(Intersection->IntersectionPoint, PInLight);
		float CosTheta = TMax(Dot(NInLight, GetDirection(PInLight, Intersection->IntersectionPoint)) , 0.0f);
		LightDir = VisibleTest.Ray._vector;
		Pdf = ShapeSetData->Pdf(Intersection->IntersectionPoint, LightDir);
		return Emit * CosTheta / pow(VisibleTest.Ray.Length, 2.0f);
	}
	bool Intersection(const BWRay &Ray, BWVector3D &PInLight, BWVector3D &NInLight, float Thit)
	{
		return ShapeSetData->Intersection(Ray, PInLight, NInLight, Thit);
	}
private:
	Spectrum Emit;
	ShapeSet *ShapeSetData;
};


