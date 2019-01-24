#pragma once
#include "BWPrimitive.h"
#include <vector>
#include <functional>
class Distribution1D;
class RTMaterial;
class LightSample;
class Shape
{
public:
	float Area() { return SumArea; }
	virtual bool Intersect(const BWRay &Ray, std::function<void(const std::vector<BWPoint3DD>& P, const std::vector<BWPoint3DD>& N, const std::vector<BWPoint2DD>& UV, float t, float u, float v, const BWRay &Ray, const RTMaterial* Material, bool &IsBreak)> IntersectionCallBack) = 0;
	float Pdf(const BWVector3D &P, const BWVector3D &Wi);
	virtual BWVector3D Sample(const BWPoint2DD &U, float &Pdf , BWVector3D &N)  = 0;
	virtual BWVector3D Sample(const BWVector3D &p, const BWPoint2DD &u, float &Pdf, BWVector3D &N) ;
protected:
	float SumArea;
};
//class ShapeSet
//{
//public:
//	ShapeSet();
//	~ShapeSet();
//	void AddShape(Shape* InShape);
//	void AddShapes(const std::vector<Shape*> &InShapes);
//	BWVector3D Sample(const BWVector3D& P, const LightSample &InLightSample, BWVector3D &N, float &Pdf) const;
//	BWVector3D Sample(const LightSample &InLightSample, BWVector3D &N) const;
//	//BPRT Version Start
//	BWVector3D Sample(const BWVector3D &P, const LightSample &InLightSample, BWVector3D &N) const;
//	//PBRT Version End
//	float GetArea() { return SumArea; }
//	float Pdf(const BWVector3D &P, const BWVector3D &Wi);
//	bool Intersection(const BWRay &Ray, BWVector3D &PInLight, BWVector3D &NInLight, float &Thit);
//private:
//	std::vector<Shape*> Shapes;
//	float SumArea;
//	std::vector<float> Areas;
//	Distribution1D *AreaDistribute;
//};

class TriganleLightShape : public Shape
{
public:
	void SetTriangleShape(std::vector<BWVector3D> &Pos, std::vector<BWVector3D> &Nor, std::vector<BWPoint2DD> &UV);
	bool Intersect(const BWRay &Ray, std::function<void(const std::vector<BWPoint3DD> & P, const std::vector<BWPoint3DD> & N, const std::vector<BWPoint2DD> & UV, float t, float u, float v, const BWRay & Ray, const RTMaterial * Material, bool & IsBreak)> IntersectionCallBack) override;
	BWVector3D Sample(const BWPoint2DD& U, float &Pdf, BWVector3D &Ns) override;
	std::vector<BWPoint3DD> P;
	std::vector<BWPoint3DD> N;
	std::vector<BWPoint2DD> UV;
};