#pragma once
#include <vector>
#include <string>
#include "Light.h"
#include "DirectionLight.h"
#include "PointLight.h"
#include "AreaLight.h"
#include "..\OfflineRenderer\RTMaterial.h"
#include <functional>
#include "Camera.h"
class Object;

class Scene
{
public:
	typedef Light LightType;
	typedef DirectionLight DirectionLightType;
	typedef PointLight PointLightType;
	typedef AreaLight AreaLightType;
	typedef Camera CameraType;
	Scene();
	~Scene();
	void AddObject(const char *ObjFileName, const std::string &ObjName);
	bool GetIntersectionInfo(const BWRay& ray, std::function<void( const std::vector<BWPoint3DD>& P, const std::vector<BWPoint3DD>& N, const std::vector<BWPoint2DD>& UV , float t, float u , float v,const BWRay &Ray , const RTMaterial* , bool &IsBreak)> IntersectionCallBack);
	Object* GetObjectByName(const std::string &Name);
	void AddLight(Light* L);
	void AddDirectionLight(DirectionLightType* L);
	void AddPointLight(PointLightType *L);
	void AddAreaLight(AreaLightType *L);
	std::vector<DirectionLightType*>& GetAllDireciontLight();
	std::vector<PointLightType*>& GetAllPointLight();
	std::vector<AreaLightType*>& GetAllAreaLight();
	Light* GetLightByName(std::string &Name);
	std::vector<Object*>& GetObjects() { return Objects; }
	void UpdateSceneInfo();
private:
	std::vector<Object*> Objects;
	std::vector<Light*> Lights;
	std::vector<DirectionLightType*> DireciontLights;
	std::vector<PointLightType*> PointLights;
	std::vector<AreaLightType*> AreaLights;
};