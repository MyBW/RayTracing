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
	
	OrigSample = new Sample();
	this->Scene = Scene;
	this->Scene->UpdateSceneInfo();
	RendererIntegrator->Init(this->Scene);
	RendererIntegrator->RequestSample(*OrigSample);

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
	std::vector<Sample*> Samples = Render->OrigSample->Duplicate(MaxSampleCount);
	RNG Rng;
	int SampleNum = 0;
	while ((SampleNum = SubSampler->GetMoreSamples(Samples, &Rng)) != 0)
	{
		for (int i = 0 ; i < SampleNum ; i++)
		{
			IntersectionInfo Intersection;
			BWRay Ray = CameraFilm->GetRayFromCamera(Samples[i]->ImageX, Samples[i]->ImageY);
			float Mint = FLT_MAX;
			auto GetIntersectionInfo = [&Intersection, &Mint](const std::vector<BWPoint3DD>& P, const std::vector<BWPoint3DD>& N, const std::vector<BWPoint2DD>& UV, float t, float u, float v,const BWRay &Ray, const RTMaterial* Material , bool &IsBreak)
			{
				if (t < Mint)
				{
					Mint = t;
					Intersection.IntersectionPoint = Ray._start + Ray._vector * t;
					Intersection.InputRay = -Ray;
					Intersection.TriangleP = P;
					Intersection.TriangleN = N;
					Intersection.TriangleUV = UV;
					Intersection.IntersectionNormal = LinearInterpolation(Intersection.TriangleN[0], Intersection.TriangleN[1], u);
					Intersection.IntersectionNormal = LinearInterpolation(Intersection.IntersectionNormal, Intersection.TriangleN[2], v);
					Intersection.IntersectionNormal.normalize();
					CoordinateSystem(Intersection.IntersectionNormal, &Intersection.IntersectionBiNormal, &Intersection.IntersectionTangent);
					Intersection.IntersectionBiNormal.normalize();
					Intersection.IntersectionTangent.normalize();
					Intersection.Material = Material;
				}
			};
			if (Scene->GetIntersectionInfo(Ray, GetIntersectionInfo))
			{
				Spectrum Color = Render->RendererIntegrator->Li(Scene, &Intersection, *Samples[i], Rng);
				CameraFilm->SetSpectrum(Samples[i]->ImageX, Samples[i]->ImageY, &Color);
			}
		}
		
	}
	delete SubSampler;
	for (auto SampleData : Samples)
	{
		delete SampleData;
	}
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


