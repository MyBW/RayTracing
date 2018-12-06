#pragma once
#include <vector>
#include <string>
#include "Light.h"
#include "DirectionLight.h"
#include "PointLight.h"
class Object;

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
	typedef Light LightType;
	typedef DirectionLight DirectionLightType;
	typedef PointLight PointLightType;
	Scene();
	~Scene();
	void AddObject(const char *ObjFileName, const std::string &ObjName);
	bool GetIntersectionInfo(BWRay& ray, IntersectionInfo &Result);
	Object* GetObjectByName(const std::string &Name);
	void AddLight(Light* L);
	std::vector<DirectionLightType*>& GetAllDireciontLight();
	std::vector<PointLightType*>& GetAllPointLight();
	Light* GetLightByName(std::string &Name);
	std::vector<Object*>& GetObjects() { return Objects; }
private:
	std::vector<Object*> Objects;
	std::vector<Light*> Lights;
	std::vector<DirectionLightType*> DireciontLights;
	std::vector<PointLightType*> PointLights;
};