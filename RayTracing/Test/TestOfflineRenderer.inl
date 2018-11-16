#pragma once
#include "..\Scene\Light.h"
#include "..\OfflineRenderer\Sample.h"
#include "..\OfflineRenderer\Sampler.h"
#include "..\OfflineRenderer\Integrator.h"
#include "RNG.h"
template<typename SceneType, typename CameraType, typename IntegratorType>
void TestOfflineRenderer<SceneType, CameraType, IntegratorType>::RenderScene(SceneType* Scene)
{
	if (!Camera)  return;
	
	this->Scene = Scene;
	const int SplitScreenNum = 16;
	std::vector<Task*> OfflineRendererTasks;
	int AllPixelNum = ScreenFilm.GetWidth() * ScreenFilm.GetHeight();
	int PixelNumForTask = AllPixelNum / SplitScreenNum;
	for (int i = 0 ;i < SplitScreenNum ; i++)
	{
		OfflineRendererTasks.push_back(new TestOfflineRendererTask<SceneType, CameraType, IntegratorType>(this, i *PixelNumForTask, i*PixelNumForTask + PixelNumForTask - 1));
	}
	if (AllPixelNum > (PixelNumForTask * SplitScreenNum))
	{
		OfflineRendererTasks.push_back(new TestOfflineRendererTask<SceneType, CameraType, IntegratorType>(this, AllPixelNum - PixelNumForTask* SplitScreenNum, AllPixelNum - 1));
	}
    EnqueueTasks(OfflineRendererTasks);
	WaitTaskListFinish();
	CleanupTaskList();
}

template<typename SceneType, typename CameraType, typename IntegratorType>
TestOfflineRenderer<SceneType, CameraType, IntegratorType>::TestOfflineRenderer(CameraType* Camera ,Sampler *MainSampler)
{
	SetCamera(Camera);
	this->MainSampler = MainSampler;
}

template<typename SceneType, typename CameraType, typename IntegratorType>
void TestOfflineRenderer<SceneType, CameraType, IntegratorType>::SetCamera(CameraType* Camera)
{
	if (!Camera) return;
	this->Camera = Camera;
	ScreenFilm.InitFilm(Camera, Camera->GetScreenWidth(), Camera->GetScreenHeight());
}




template<typename SceneType, typename CameraType, typename IntegratorType>
void TestOfflineRendererTask<SceneType, CameraType, IntegratorType>::Run()
{
	SceneType *Scene = Render->GetScene();
	Film<CameraType> *CameraFilm = Render->GetFilm();
	Sampler *MainSampelr = Render->GetMainSampler();
	Sampler *SubSampler = MainSampelr->GetSubSampler(StarPixelIndex, EndPiexlIndex);
	int MaxSampleCount = SubSampler->GetMaxSampleCount();
	Sample *Samples = new Sample[MaxSampleCount];
	RNG Rng;
	int SampleNum = 0;
	while ((SampleNum = SubSampler->GetMoreSamples(Samples, &Rng)))
	{
		for (int i = 0; i < SampleNum; i++)
		{
			SceneType::IntersectionType Intersection;
			BWRay Ray = CameraFilm->GetRayFromCamera(i);
			if (Scene->GetIntersectionInfo(Ray, Intersection))
			{
			 	Spectrum Color = Render->RendererIntegrator->Li(Scene, &Intersection);
				CameraFilm->SetSpectrum(i, &Color);
			}
		}

	}
}

template<typename SceneType, typename CameraType, typename IntegratorType>
TestOfflineRendererTask<SceneType, CameraType, IntegratorType>::~TestOfflineRendererTask()
{

}

template<typename SceneType, typename CameraType, typename IntegratorType>
TestOfflineRendererTask<SceneType, CameraType, IntegratorType>::TestOfflineRendererTask(TestOfflineRenderer<SceneType, CameraType, IntegratorType> *Render, int StarPixelIndex, int EndPiexlIndex)
{
	this->Render = Render;
	this->StarPixelIndex = StarPixelIndex;
	this->EndPiexlIndex = EndPiexlIndex;
}


