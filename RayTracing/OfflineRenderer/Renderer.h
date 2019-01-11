#pragma once
#include "RTRenderer.h"
#include <vector>
class RTMaterial;
class VisibleTester
{
public:
	void SetRay(const BWVector3DD &Start, const BWVector3DD &Vector)
	{
		Ray._start = Start;
		Ray._vector = Vector;
		Ray.Length = FLT_MAX;
	}
	void SetSegment(const BWVector3DD &Start, const BWVector3DD &End)
	{
		Ray._start = Start;
		Ray._vector = End - Start;
		Ray.Length = Lenth(Ray._vector);
		Ray._vector.normalize();
	}
	template<typename SceneType>
	bool IsBlock(const SceneType&Scene)
	{
		bool Result = false;
		auto GetIntersectionInfo = [&Result](const std::vector<BWPoint3DD>& P,const std::vector<BWPoint3DD>& N,const std::vector<BWPoint2DD>& UV, float t, float u, float v,const BWRay &Ray, const RTMaterial* Material , bool &IsBreak)
		{
			float Distance = Lenth(Ray._vector * t);
			if ( Distance > ESP && Distance < Ray.Length - ESP)
			{
				IsBreak = true;
				Result = true;
			}
		};
		Scene->GetIntersectionInfo(Ray, GetIntersectionInfo);
		return Result;
	}
	BWRay Ray;
};
struct IntersectionInfo
{
	std::vector<BWVector3DD> TriangleP;
	std::vector<BWVector3DD> TriangleN;
	std::vector<BWPoint2DD> TriangleUV;
	BWVector3DD IntersectionPoint;
	BWVector3DD IntersectionNormal;
	BWVector3DD IntersectionBiNormal;
	BWVector3DD IntersectionTangent;
	BWRay InputRay;
	BWPoint2DD IntersectionUV;
	const RTMaterial* Material;
};
template<typename ScenenType>
class Renderer
{
public:
	virtual void RenderScene(ScenenType* Scene) = 0;
	// 计算两个点之间的遮蔽
	virtual Spectrum Transmittance() = 0;
	virtual Spectrum Li() = 0;
};