#pragma once
#include <vector>
#include <string>
class Object;
struct IntersectionInfo 
{
	TriangleInfo IntersetionTriangleInfo;
	BWVector3DD IntersectionPoint;
	BWVector3DD IntersectionNormal;
	BWPoint2DD IntersectionUV;
};
class Scene
{
public:
	typedef IntersectionInfo IntersectionType;
	Scene();
	~Scene();
	void AddObject(const char *ObjFileName, const std::string &ObjName);
	bool GetIntersectionInfo(BWRay& ray, IntersectionInfo &Result);
	Object* GetObjectByName(const std::string &Name);
private:
	std::vector<Object*> Objects;
};