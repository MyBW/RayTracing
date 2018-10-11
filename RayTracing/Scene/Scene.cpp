#include "Object.h"
#include "Scene.h"
#include "..\Math\Math.h"
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
	for (auto Obj : Objects)
	{
		for (int i = 0 ; i < Obj->GetTriangleNum() ; i++)
		{
			float t, u, v;
			Result.IntersetionTriangleInfo.ClearData();
			Obj->GetTriangleInfoByIndex(i, Result.IntersetionTriangleInfo);
			if (RayIntersectTriangle(ray, Result.IntersetionTriangleInfo.P[0], Result.IntersetionTriangleInfo.P[1], Result.IntersetionTriangleInfo.P[2], t, u, v))
			{
				Result.IntersectionPoint = ray._start + ray._vector * t;
				return true;
			}
		}
	}
	return false;
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

