#pragma once
#include "RTRenderer.h"
#include <vector>
class RTMaterial;
struct IntersectionInfo
{
	std::vector<BWVector3DD> TriangleP;
	std::vector<BWVector3DD> TriangleN;
	std::vector<BWPoint2DD> TriangleUV;
	BWVector3DD IntersectionPoint;
	BWVector3DD IntersectionNormal;
	BWRay InputRay;
	BWPoint2DD IntersectionUV;
	const RTMaterial* Material;
};
template<typename ScenenType>
class Renderer
{
public:
	virtual void RenderScene(ScenenType* Scene) = 0;
	virtual Spectrum Li() = 0;
};