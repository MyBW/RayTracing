#pragma once
#include "RTLight.h"
#include "Renderer.h"
#include "Montecarlo.h"
#include <functional>

class Shape
{
public:
	float Area() { return SumArea; };
	BWVector3D Sample(const BWVector3D&p, float u, float v, BWVector3D &Ns);
	BWVector3D Sample(float u, float v, BWVector3D &Ns);
	virtual bool Intersect(const BWRay &Ray, std::function<void(const std::vector<BWPoint3DD>& P, const std::vector<BWPoint3DD>& N, const std::vector<BWPoint2DD>& UV, float t, float u, float v, const BWRay &Ray, const RTMaterial* Material, bool &IsBreak)> IntersectionCallBack) = 0;
	float Pdf(BWVector3D &P, BWVector3D &Wi) 
	{
		float ResultPdf = 0.0;
		BWRay Ray(P, Wi, FLT_MAX);
		auto InterCallBack = [&](const std::vector<BWPoint3DD>& InP, const std::vector<BWPoint3DD>& InN, const std::vector<BWPoint2DD>& UV, float t, float u, float v, const BWRay &InRay, const RTMaterial*Material, bool &IsBreak)
		{
			if (t > ESP && t < Ray.Length - ESP)
			{
				Ray.Length = t;
				BWVector3D ShapePoint = Ray._start + Ray._vector * Ray.Length;
				BWVector3D N = LinearInterpolation(InN[0], InN[1], u);
				N = LinearInterpolation(N, InN[2], v);
				N.normalize();
				ResultPdf = pow(Lenth(P - ShapePoint), 2.0) / TMax(Dot(-Ray._vector, N), 0.0);
			}
		};
		Intersect(Ray, InterCallBack);
		return ResultPdf;
	}
protected:
	float SumArea;
private:

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
	BWVector3D Sample(const BWVector3D& P, const LightSample &InLightSample, BWVector3D &N, float &Pdf) const
	{
		float AreaPdf = 0.f;
		int ShapeIndex = AreaDistribute->SampleDistribute(InLightSample.ShapeComponent, AreaPdf);

		BWVector3D ShapePoint = Shapes[ShapeIndex]->Sample(P, InLightSample.Pos[0], InLightSample.Pos[1], N);
		BWRay Ray(P, GetDirection(P, ShapePoint), Lenth(P - ShapePoint));
		auto InterCallBack = [&](const std::vector<BWPoint3DD>& InP, const std::vector<BWPoint3DD>& InN, const std::vector<BWPoint2DD>& UV, float t, float u, float v, const BWRay &InRay, const RTMaterial*Material, bool &IsBreak)
		{
			if (t > ESP && t < Ray.Length - ESP)
			{
				Ray.Length = t;
				ShapePoint = Ray._start +  Ray._vector * Ray.Length;
				N = LinearInterpolation(InN[0], InN[1], u);
				N = LinearInterpolation(N, InN[2], v);
				N.normalize();
				Pdf = pow(Lenth(P - ShapePoint) , 2.0)/ TMax(Dot( -Ray._vector , N) , 0.0);
			}
		};
		for (auto ShapeEle : Shapes)
		{
			ShapeEle->Intersect(Ray, InterCallBack);
		}
		Pdf = Pdf / SumArea;
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
			SumPdf += Areas[i]* Shapes[i]->Pdf(P, Wi);
		}
		return SumPdf / SumArea;
	}	
	bool Intersection(const BWRay &Ray, BWVector3D &PInLight, BWVector3D &NInLight, float &Thit)
	{
		float Thit = Ray.Length;
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
		for (auto ShapeEle : Shapes)
		{
			ShapeEle->Intersect(Ray, InterCallBack);
		}
	}
private:
	std::vector<Shape*> Shapes;
	float SumArea;
	std::vector<float> Areas;
	Distribution1D *AreaDistribute;
}

template<typename MeshType>
class MeshAreaLightShape : public Shape
{
	void InitMeshAreaLight(MeshType *SrcMesh)
	{
		if (SrcMesh)
		{
			AddMesh(SrcMesh);
			InitData(SrcMesh);
		}
	}
	bool Intersect(const BWRay &Ray, std::function<void(const std::vector<BWPoint3DD>& P, const std::vector<BWPoint3DD>& N, const std::vector<BWPoint2DD>& UV, float t, float u, float v, const BWRay &Ray, const RTMaterial* Material, bool &IsBreak)> IntersectionCallBack) override
	{
		return SrcMesh->GetIntersectionInfo(Ray, IntersectionCallBack);
	}
private:
	void AddMesh(MeshType *SrcMesh)
	{
		this->SrcMesh = SrcMesh;
	}
	void InitData(MeshType *SrcMesh)
	{
		int TriangleNum = SrcMesh->GetTriangleNum();
		SumArea = 0.0f;
		for (int i = 0; i < TriangleNum; i++)
		{
			BWVector3D &P0 = SrcMesh->GetPosData()[3 * i];
			BWVector3D &P1 = SrcMesh->GetPosData()[3 * i + 1];
			BWVector3D &P2 = SrcMesh->GetPosData()[3 * i + 2];
			SumArea = (float)TriangleArea(P0, P2, P1);
		}
	}
	MeshType *SrcMesh;
};

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
		BWVector3D PInLight = ShapeSetData->Sample(Intersection->IntersectionPoint, InLightSample, NInLight , &Pdf);
		VisibleTest.SetSegment(Intersection->IntersectionPoint, PInLight);
		float CosTheta = TMax(Dot(NInLight, GetDirection(PInLight, Intersection->IntersectionPoint)), 0.0f);
		return Emit * CosTheta / pow(VisibleTest.Ray.Length, 2.0f);
	}
	bool Intersection(const BWRay &Ray, BWVector3D &PInLight, BWVector3D &NInLight, float &Thit)
	{
		return ShapeSetData->Intersection(Ray, PInLight, NInLight, Thit);
	}
	void SetSrcAreaLight(AreaLightType *SrcAreaLight)
	{
		this->SrcAreaLight = SrcAreaLight;
	}
private:
	AreaLightType *SrcAreaLight;
	Spectrum Emit;
	ShapeSet *ShapeSetData;
};


