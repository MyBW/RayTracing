#pragma once
#include "RTRenderer.h"
#include <vector>
#include "Film.h"
#include "Integrator.h"
class RTMaterial;
class Sampler;
class Task;
class VisibleTester
{
public:
	void SetRay(const BWVector3DD &Start, const BWVector3DD &Vector)
	{
		Ray._start = Start;
		Ray._vector = Vector;
		Ray.Length = FLT_MAX;
	}
	void SetSegment(const BWVector3DD &Start, const BWVector3DD &End)
	{
		Ray._start = Start;
		Ray._vector = End - Start;
		Ray.Length = Lenth(Ray._vector);
		Ray._vector.normalize();
	}
	template<typename SceneType>
	bool IsBlock(const SceneType&Scene)
	{
		bool Result = false;
		auto GetIntersectionInfo = [&Result](const std::vector<BWPoint3DD>& P,const std::vector<BWPoint3DD>& N,const std::vector<BWPoint2DD>& UV, float t, float u, float v,const BWRay &Ray, const RTMaterial* Material , bool &IsBreak)
		{
			float Distance = Lenth(Ray._vector * t);
			if ( Distance > ESP && Distance < Ray.Length - ESP)
			{
				IsBreak = true;
				Result = true;
			}
		};
		Scene->GetIntersectionInfo(Ray, GetIntersectionInfo);
		return Result;
	}
	BWRay Ray;
};
struct IntersectionInfo
{
	std::vector<BWVector3DD> TriangleP;
	std::vector<BWVector3DD> TriangleN;
	std::vector<BWPoint2DD> TriangleUV;
	BWVector3DD IntersectionPoint;
	BWVector3DD IntersectionNormal;
	BWVector3DD IntersectionBiNormal;
	BWVector3DD IntersectionTangent;
	BWRay InputRay;
	BWPoint2DD IntersectionUV;
	const RTMaterial* Material;
};
template<typename SceneType>
class Renderer
{
public:
	typedef typename SceneType::CameraType CameraType;
	Renderer(CameraType* Camera = nullptr, Sampler *MainSampler = nullptr)
	{
		SetCamera(Camera);
		this->MainSampler = MainSampler;
		OrigSample = new Sample();
	}
	virtual void RenderScene(SceneType* Scene) = 0;
	void SetCamera(CameraType* Camera)
	{
		if (!Camera) return;
		this->Camera = Camera;
		ScreenFilm.InitFilm(Camera, Camera->GetScreenWidth(), Camera->GetScreenHeight());
	}
	CameraType* GetCamera() const { return Camera; }
	SceneType* GetScene() const { return Scene; }
	Film<CameraType>* GetFilm() { return &ScreenFilm; }
	void SetIntegrator(Integrator<typename SceneType> *InIntergrator) { RendererIntegrator = InIntergrator; }
	void SetSampler(Sampler* InSampler) { MainSampler = InSampler; }
	Sampler* GetMainSampler() { return MainSampler; }
	Sample* GetOrigSample() { return OrigSample; }
	Integrator<typename SceneType>* GetIntegrator() { return RendererIntegrator; }
	void ParallelProcess(std::function<void(std::vector<Task*>&)> CreateTask)
	{
		std::vector<Task*> Tasks;
		CreateTask(Tasks);
		EnqueueTasks(Tasks);
		WaitTaskListFinish();
		CleanupTaskList();
	}
protected:
	Film<CameraType> ScreenFilm;
	SceneType *Scene;
	CameraType *Camera;
	Sampler *MainSampler;
	Sample *OrigSample;
	Integrator<typename SceneType> *RendererIntegrator;
};