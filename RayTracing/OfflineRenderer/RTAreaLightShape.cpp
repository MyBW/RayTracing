#include "RTAreaLightShape.h"
#include "RTMath.h"
#include "Montecarlo.h"
#include "RTLightSample.h"
#include<iostream>
BWVector3D Shape::Sample(const BWVector3D&p, float u, float v, BWVector3D &Ns)
{
	return Sample(u, v, Ns);
}

BWVector3D Shape::Sample(float u, float v, BWVector3D &Ns)
{
	return BWVector3D();
}

BWVector3D Shape::Sample(const BWVector3D &p, const BWPoint2DD &u, float &Pdf, BWVector3D &N)
{
	BWVector3D PInLight = Sample(u, Pdf, N);
	BWVector3D Wi = GetDirection(p, PInLight);
	if (Wi.lenth() == 0)
	{
		Pdf = 0.0f;
	}
	else
	{
		Pdf *= pow(Lenth(p - PInLight), 2.0f) / AbsDot(N, -Wi);
		if (isnan(Pdf)) Pdf = 0.0f;
	}
	return PInLight;
}

float Shape::Pdf(const BWVector3D &P, const BWVector3D &Wi)
{
	float ResultPdf = 0.0;
 	BWRay Ray(P, Wi, FLT_MAX);
	auto InterCallBack = [&](const std::vector<BWPoint3DD>& InP, const std::vector<BWPoint3DD>& InN, const std::vector<BWPoint2DD>& UV, float t, float u, float v, const BWRay &InRay, const RTMaterial*Material, bool &IsBreak)
	{
		/*if (t > ESP && t < Ray.Length - ESP)
		{
			Ray.Length = t;
			BWVector3D ShapePoint = Ray._start + Ray._vector * Ray.Length;
			BWVector3D N = LinearInterpolation(InN[0], InN[1], u);
			N = LinearInterpolation(N, InN[2], v);
			N.normalize();
			ResultPdf = pow(Lenth(P - ShapePoint), 2.0) / TMax(Dot(-Ray._vector, N), 0.0);
		}*/
		if (t > ESP)
		{
			BWVector3D ShapePoint = Ray._start + Ray._vector * t;
			BWVector3D N = LinearInterpolation(InN[0], InN[1], u);
			N = LinearInterpolation(N, InN[2], v);
			N.normalize();
			ResultPdf = pow(Lenth(P - ShapePoint), 2.0) / (AbsDot(-Ray._vector, N) * Area());
		}
	};
	Intersect(Ray, InterCallBack);
	return ResultPdf;
}

ShapeSet::ShapeSet():SumArea(0.0f)
{
	AreaDistribute = new Distribution1D();
}

ShapeSet::~ShapeSet()
{
	delete AreaDistribute;
	for (auto S : Shapes)
	{
		delete S;
	}
}

void ShapeSet::AddShape(Shape* InShape)
{
	if (!InShape) return;
	Shapes.push_back(InShape);
	Areas.push_back(InShape->Area());
	SumArea += InShape->Area();
	AreaDistribute->ResetDistributionData(Areas);
}

void ShapeSet::AddShapes(const std::vector<Shape*>& InShapes)
{
	for (int i = 0; i < InShapes.size(); i++)
	{
		if (InShapes[i])
		{
			Shapes.push_back(InShapes[i]);
			Areas.push_back(InShapes[i]->Area());
			SumArea += InShapes[i]->Area();
		}
	}
	AreaDistribute->ResetDistributionData(Areas);
}

BWVector3D ShapeSet::Sample(const BWVector3D& P, const LightSample &InLightSample, BWVector3D &N, float &Pdf) const
{
	float AreaPdf = 0.f;
	int ShapeIndex = AreaDistribute->SampleDistribute(InLightSample.ShapeComponent, AreaPdf);

	BWVector3D ShapePoint = Shapes[ShapeIndex]->Sample(P, InLightSample.Pos[0], InLightSample.Pos[1], N);
	BWRay Ray(P, GetDirection(P, ShapePoint), Lenth(P - ShapePoint));
	Pdf = Shapes[ShapeIndex]->Pdf(P, GetDirection(P, ShapePoint));
	float ShapePdf = 0.0f;
	auto InterCallBack = [&](const std::vector<BWPoint3DD>& InP, const std::vector<BWPoint3DD>& InN, const std::vector<BWPoint2DD>& UV, float t, float u, float v, const BWRay &InRay, const RTMaterial*Material, bool &IsBreak)
	{
		if (t > ESP && t < InRay.Length - ESP)
		{
			float TmpLent = InRay.Length;
			Ray.Length = t;
			ShapePoint = InRay._start + InRay._vector * Ray.Length;
			N = LinearInterpolation(InN[0], InN[1], u);
			N = LinearInterpolation(N, InN[2], v);
			N.normalize();
			if (TMax(Dot(-InRay._vector, N) * TriangleArea(InP[0], InP[1], InP[2]), 0.0) == 0.0f)
			{
				ShapePdf = 0.0f;
			}
			else
			{
				ShapePdf = pow(Lenth(P - ShapePoint), 2.0) / Dot(-InRay._vector, N) * TriangleArea(InP[0], InP[1], InP[2]);
			}

		}
	};
	for (int i = 0; i < Shapes.size(); i++)
	{
		if (Shapes[i]->Intersect(Ray, InterCallBack))
			Pdf += Areas[i] * ShapePdf;
		ShapePdf = 0.0f;
	}
	Pdf = Pdf / SumArea;
	return ShapePoint;
}

BWVector3D ShapeSet::Sample(const LightSample &InLightSample, BWVector3D &N) const
{
	float AreaPdf = 0.f;
	int ShapeIndex = AreaDistribute->SampleDistribute(InLightSample.ShapeComponent, AreaPdf);
	BWVector3D ShapePoint = Shapes[ShapeIndex]->Sample(InLightSample.Pos[0], InLightSample.Pos[1], N);
	return ShapePoint;
}
//PBRT Version Start
BWVector3D ShapeSet::Sample(const BWVector3D &P, const LightSample &InLightSample, BWVector3D &N) const
{
	float AreaPdf = 0.f;
	int ShapeIndex = AreaDistribute->SampleDistribute(InLightSample.ShapeComponent, AreaPdf);

	BWVector3D ShapePoint = Shapes[ShapeIndex]->Sample(P, InLightSample.Pos[0], InLightSample.Pos[1], N);
	BWRay Ray(P, GetDirection(P, ShapePoint), FLT_MAX_);
	float HitLenght = 0.f;
	auto InterCallBack = [&](const std::vector<BWPoint3DD>& InP, const std::vector<BWPoint3DD>& InN, const std::vector<BWPoint2DD>& UV, float t, float u, float v, const BWRay &InRay, const RTMaterial*Material, bool &IsBreak)
	{
		HitLenght = t;
		ShapePoint = InRay._start + InRay._vector * t;
		N = LinearInterpolation(InN[0], InN[1], u);
		N = LinearInterpolation(N, InN[2], v);
		N.normalize();
	};
	for (int i = 0; i < Shapes.size(); i++)
	{
		Shapes[i]->Intersect(Ray, InterCallBack);
	}
	return ShapePoint;
}
//PBRT Version End
float ShapeSet::Pdf(const BWVector3D &P, const BWVector3D &Wi)
{
	float SumPdf = 0;
	for (size_t i = 0; i < Shapes.size(); i++)
	{
		SumPdf += Areas[i] * Shapes[i]->Pdf(P, Wi);
	}
	return SumPdf / SumArea;
}

bool ShapeSet::Intersection(const BWRay &Ray, BWVector3D &PInLight, BWVector3D &NInLight, float &Thit)
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
	bool IsItersection = false;
	for (auto ShapeEle : Shapes)
	{
		IsItersection |= ShapeEle->Intersect(Ray, InterCallBack);
	}
	return IsItersection;
}

void TriganleLightShape::SetTriangleShape(std::vector<BWVector3D> &Pos, std::vector<BWVector3D> &Nor, std::vector<BWPoint2DD> &UV)
{
	P = Pos;
	N = Nor;
	this->UV = UV;
	SumArea = 0.0f;
	SumArea = (float)TriangleArea(Pos[0], Pos[1], Pos[2]);
}

bool TriganleLightShape::Intersect(const BWRay &Ray, std::function<void(const std::vector<BWPoint3DD> & P, const std::vector<BWPoint3DD> & N, const std::vector<BWPoint2DD> & UV, float t, float u, float v, const BWRay & Ray, const RTMaterial * Material, bool & IsBreak)> IntersectionCallBack)
{
	float t, u, v;
	bool IsBreak;
	if (RayIntersectTriangle(Ray, P[0], P[1], P[2], t, u, v))
	{
		IntersectionCallBack(P, N, UV, t, u, v, Ray, nullptr, IsBreak);
		return true;
	}
	return false;
}

BWVector3D TriganleLightShape::Sample(float u, float v, BWVector3D &Ns)
{
	float b1, b2;
	BWVector3D Pos;
	UniformSampleTriangle(u, v, &b1, &b2);
	Pos = LinearInterpolation(P[0], P[1], b1);
	Pos = LinearInterpolation(Pos, P[2], b2);
	Ns = LinearInterpolation(N[0], N[1], b1);
	Ns = LinearInterpolation(Ns, N[2], b2);
	return Pos;
}

BWVector3D TriganleLightShape::Sample(const BWPoint2DD& U, float &Pdf, BWVector3D &Ns)
{
	float b1, b2;
	BWVector3D Pos;
	UniformSampleTriangle(U.x, U.y, &b1, &b2);
	Pos = LinearInterpolation(P[0], P[1], b1);
	Pos = LinearInterpolation(Pos, P[2], b2);
	Ns = LinearInterpolation(N[0], N[1], b1);
	Ns = LinearInterpolation(Ns, N[2], b2);
	Pdf = 1 / Area();
	return Pos;
}
