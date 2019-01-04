#pragma once
#include "..\Scene\Light.h"
#include "..\OfflineRenderer\Sample.h"
#include "..\OfflineRenderer\Sampler.h"
#include "..\OfflineRenderer\Integrator.h"
#include "RNG.h"
template<typename SceneType, typename CameraType>
void TestOfflineRenderer<SceneType, CameraType>::RenderScene(SceneType* Scene)
{
	if (!Camera)  return;
	
	this->Scene = Scene;
	this->Scene->UpdateSceneInfo();
	RendererIntegrator->Init(this->Scene);

	const int SplitScreenNum = 64;
	std::vector<Task*> OfflineRendererTasks;
	int AllPixelNum = ScreenFilm.GetWidth() * ScreenFilm.GetHeight();
	int PixelNumForTask = AllPixelNum / SplitScreenNum;
	for (int i = 0 ;i < SplitScreenNum ; i++)
	{
		OfflineRendererTasks.push_back(new TestOfflineRendererTask<SceneType, CameraType>(this, i *PixelNumForTask, i*PixelNumForTask + PixelNumForTask - 1));
	}
	if (AllPixelNum > (PixelNumForTask * SplitScreenNum))
	{
		OfflineRendererTasks.push_back(new TestOfflineRendererTask<SceneType, CameraType>(this, AllPixelNum - PixelNumForTask* SplitScreenNum, AllPixelNum - 1));
	}
    EnqueueTasks(OfflineRendererTasks);
	WaitTaskListFinish();
	CleanupTaskList();
}

template<typename SceneType, typename CameraType>
TestOfflineRenderer<SceneType, CameraType>::TestOfflineRenderer(CameraType* Camera ,Sampler *MainSampler)
{
	SetCamera(Camera);
	this->MainSampler = MainSampler;
}

template<typename SceneType, typename CameraType>
void TestOfflineRenderer<SceneType, CameraType>::SetCamera(CameraType* Camera)
{
	if (!Camera) return;
	this->Camera = Camera;
	ScreenFilm.InitFilm(Camera, Camera->GetScreenWidth(), Camera->GetScreenHeight());
}

template<typename SceneType, typename CameraType>
void TestOfflineRenderer<SceneType, CameraType>::SetIntegrator(Integrator<typename SceneType, IntersectionInfo> *InIntergrator)
{
	RendererIntegrator = InIntergrator;
}


template<typename SceneType, typename CameraType>
void TestOfflineRendererTask<SceneType, CameraType>::Run()
{
	SceneType *Scene = Render->GetScene();
	Film<CameraType> *CameraFilm = Render->GetFilm();
	Sampler *MainSampelr = Render->GetMainSampler();
	Sampler *SubSampler = MainSampelr->GetSubSampler(StarPixelIndex, EndPiexlIndex);
	int MaxSampleCount = SubSampler->GetMaxSampleCount();
	Sample *Samples = new Sample[MaxSampleCount];
	RNG Rng;
	int SampleNum = 0;
	for (int i  = StarPixelIndex; i < EndPiexlIndex;i++)
	{
		IntersectionInfo Intersection;
		BWRay Ray = CameraFilm->GetRayFromCamera(i);
		auto GetIntersectionInfo = [&Intersection](std::vector<BWPoint3DD>& P, std::vector<BWPoint3DD>& N, std::vector<BWPoint2DD>& UV, float t, float u, float v, BWRay &Ray, const RTMaterial* Material)
		{
			Intersection.IntersectionPoint = Ray._start + Ray._vector * t;
			Intersection.InputRay = -Ray;
			Intersection.TriangleP = P;
			Intersection.TriangleN = N;
			Intersection.TriangleUV = UV;
			Intersection.IntersectionNormal = LinearInterpolation(Intersection.TriangleN[0], Intersection.TriangleN[1], u);
			Intersection.IntersectionNormal = LinearInterpolation(Intersection.IntersectionNormal, Intersection.TriangleN[2], v);
			Intersection.IntersectionNormal.normalize();
			Intersection.Material = Material;
		};
		if (Scene->GetIntersectionInfo(Ray, GetIntersectionInfo))
		{
			Spectrum Color = Render->RendererIntegrator->Li(Scene, &Intersection);
			CameraFilm->SetSpectrum(i, &Color);
		}
	}
	delete SubSampler;
	delete [] Samples;
}

template<typename SceneType, typename CameraType>
TestOfflineRendererTask<SceneType, CameraType>::~TestOfflineRendererTask()
{

}

template<typename SceneType, typename CameraType>
TestOfflineRendererTask<SceneType, CameraType>::TestOfflineRendererTask(TestOfflineRenderer<SceneType, CameraType> *Render, int StarPixelIndex, int EndPiexlIndex)
{
	this->Render = Render;
	this->StarPixelIndex = StarPixelIndex;
	this->EndPiexlIndex = EndPiexlIndex;
}


