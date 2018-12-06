#include "Object.h"
#include "Scene.h"
#include "RTMath.h"
#include "Light.h"
Scene::Scene()
{

}

Scene::~Scene()
{

}

void Scene::AddObject(const char *ObjFileName, const std::string &ObjName)
{
	Object *Obj = new Object();
	Obj->LoadObjModel(ObjFileName, ObjName);
	Objects.push_back(Obj);
}
bool Scene::GetIntersectionInfo(BWRay& ray , IntersectionInfo &Result)
{
	Result.IntersetionTriangleInfo.ClearData();
	TriangleInfo TempRes;
	float t, u, v;
	float Mint = FLT_MAX;
	for (auto Obj : Objects)
	{
		for (int i = 0 ; i < Obj->GetTriangleNum() ; i++)
		{
			TempRes.ClearData();
			Obj->GetTriangleInfoByIndex(i, TempRes);
			if (RayIntersectTriangle(ray, TempRes.P[0], TempRes.P[1], TempRes.P[2], t, u, v) && t  < Mint)
			{
				Mint = t;
				Result.IntersectionPoint = ray._start + ray._vector * t;
				Result.InputRay = -ray;
				Result.IntersetionTriangleInfo = TempRes;
				Result.IntersectionNormal = LinearInterpolation(Result.IntersetionTriangleInfo.N[0], Result.IntersetionTriangleInfo.N[1], u);
				Result.IntersectionNormal = LinearInterpolation(Result.IntersectionNormal, Result.IntersetionTriangleInfo.N[2], v);
				Result.IntersectionNormal.normalize();
			}
		}
	}
	return Mint != FLT_MAX;
}

Object* Scene::GetObjectByName(const std::string &Name)
{
	for (auto Obj : Objects)
	{
		if (Obj->GetName() == Name)
		{
			return Obj;
		}
	}
	return nullptr;
}

void Scene::AddLight(Light* L)
{
	if (L)
	{
		Lights.push_back(L);
	}
}

std::vector<Scene::DirectionLightType*>& Scene::GetAllDireciontLight()
{
	return DireciontLights;
}

std::vector<Scene::PointLightType*>& Scene::GetAllPointLight()
{
	return PointLights;
}

Light* Scene::GetLightByName(std::string &Name)
{
	for (auto L:Lights )
	{
		if (L->GetName() == Name) return L;
	}
	return NULL;
}

