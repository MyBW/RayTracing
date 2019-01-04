#pragma once
#include <vector>
#include <string>
#include "Light.h"
#include "DirectionLight.h"
#include "PointLight.h"
#include "..\OfflineRenderer\RTMaterial.h"
class Object;

struct IntersectionInfo 
{
	TriangleInfo IntersetionTriangleInfo;
	BWVector3DD IntersectionPoint;
	BWVector3DD IntersectionNormal;
	BWRay InputRay;
	BWPoint2DD IntersectionUV;
	RTMaterial Material;
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
	void AddDirectionLight(DirectionLightType* L);
	void AddPointLight(PointLightType *L);
	std::vector<DirectionLightType*>& GetAllDireciontLight();
	std::vector<PointLightType*>& GetAllPointLight();
	Light* GetLightByName(std::string &Name);
	std::vector<Object*>& GetObjects() { return Objects; }
	void UpdateSceneInfo();
private:
	std::vector<Object*> Objects;
	std::vector<Light*> Lights;
	std::vector<DirectionLightType*> DireciontLights;
	std::vector<PointLightType*> PointLights;
};