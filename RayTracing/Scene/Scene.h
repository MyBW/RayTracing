#pragma once
#include <vector>
#include <string>
class Object;
class Light;
struct IntersectionInfo 
{
	TriangleInfo IntersetionTriangleInfo;
	BWVector3DD IntersectionPoint;
	BWVector3DD IntersectionNormal;
	BWRay InputRay;
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
	void AddLight(Light* L);
	Light* GetLightByName(std::string &Name);
	std::vector<Object*>& GetObjects() { return Objects; }
private:
	std::vector<Object*> Objects;
	std::vector<Light*> Lights;
};