#pragma once
#include <vector>
#include <string>
#include "Light.h"
#include "DirectionLight.h"
#include "PointLight.h"
#include "..\OfflineRenderer\RTMaterial.h"
#include <functional>
class Object;

class Scene
{
public:
	typedef Light LightType;
	typedef DirectionLight DirectionLightType;
	typedef PointLight PointLightType;
	Scene();
	~Scene();
	void AddObject(const char *ObjFileName, const std::string &ObjName);
	bool GetIntersectionInfo(BWRay& ray, std::function<void(std::vector<BWPoint3DD>& P, std::vector<BWPoint3DD>& N, std::vector<BWPoint2DD>& UV , float t, float u , float v, BWRay &Ray , const RTMaterial*)> IntersectionCallBack);
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