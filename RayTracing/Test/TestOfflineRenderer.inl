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
void TestOfflineRenderer<SceneType, CameraType>::SetIntegrator(Integrator<typename SceneType, typename SceneType::IntersectionType> *InIntergrator)
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
		SceneType::IntersectionType Intersection;
		BWRay Ray = CameraFilm->GetRayFromCamera(i);
		if (Scene->GetIntersectionInfo(Ray, Intersection))
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


