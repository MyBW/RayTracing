#include "Object.h"
#include "Scene.h"
#include "RTMath.h"
#include "Light.h"
#include "..\OfflineRenderer\BSDF.h"
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
//bool Scene::GetIntersectionInfo(BWRay& ray , IntersectionInfo &Result)
//{
//	Result.IntersetionTriangleInfo.ClearData();
//	TriangleInfo TempRes;
//	TempRes.Resize(3);
//	float t = 0, u = 0, v = 0;
//	float Mint = FLT_MAX;
//	for (auto Obj : Objects)
//	{
//		int TriangleNum = Obj->GetTriangleNum();
//		for (int i = 0 ; i < TriangleNum ; i++)
//		{
//			Obj->GetTriangleWorldInfoByIndex(i, TempRes);
//			if (RayIntersectTriangle(ray, TempRes.P[0], TempRes.P[1], TempRes.P[2], t, u, v) && t  < Mint)
//			{
//				Mint = t;
//				Result.IntersectionPoint = ray._start + ray._vector * t;
//				Result.InputRay = -ray;
//				Result.IntersetionTriangleInfo = TempRes;
//				Result.IntersectionNormal = LinearInterpolation(Result.IntersetionTriangleInfo.N[0], Result.IntersetionTriangleInfo.N[1], u);
//				Result.IntersectionNormal = LinearInterpolation(Result.IntersectionNormal, Result.IntersetionTriangleInfo.N[2], v);
//				Result.IntersectionNormal.normalize();
//				Result.Material = new RTMaterial<IntersectionInfo>();
//			}
//		}
//	}
//	return Mint != FLT_MAX;
//}

bool Scene::GetIntersectionInfo(const BWRay& ray, std::function<void(const std::vector<BWPoint3DD>& P, const std::vector<BWPoint3DD>& N, const std::vector<BWPoint2DD>& UV, float t, float u, float v,const BWRay &Ray, const RTMaterial*, bool &IsBreak)> IntersectionCallBack)
{
	TriangleInfo TempRes;
	TempRes.Resize(3);
	float t = 0, u = 0, v = 0;
	float Mint = FLT_MAX;
	bool IsBreak = false;
	for (auto Obj : Objects)
	{
		int TriangleNum = Obj->GetTriangleNum();
		for (int i = 0; i < TriangleNum; i++)
		{
			Obj->GetTriangleWorldInfoByIndex(i, TempRes);
			if (RayIntersectTriangle(ray, TempRes.P[0], TempRes.P[1], TempRes.P[2], t, u, v))
			{
				Mint = t;
				IntersectionCallBack(TempRes.P, TempRes.N, TempRes.UV, t, u, v, ray, Obj->GetMaterial() , IsBreak);
				if (IsBreak) return Mint != FLT_MAX;
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

void Scene::AddDirectionLight(Scene::DirectionLightType* L)
{
	if (L)
	{
		DireciontLights.push_back(L);
	}
}

void Scene::AddPointLight(Scene::PointLightType *L)
{
	if (L)
	{
		PointLights.push_back(L);
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

void Scene::UpdateSceneInfo()
{
	for (auto Obj : Objects)
	{
		Obj->UpdateWorldInfor();
	}
}

