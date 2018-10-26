#pragma once
#include "..\OfflineRenderer\LightSampler.h"
#include "..\Scene\Light.h"
template<typename SceneType, typename CameraType>
void TestOfflineRenderer<SceneType, CameraType>::RenderScene(SceneType* Scene)
{
	if (!Camera)  return;
	
	this->Scene = Scene;
	const int SplitScreenNum = 1;
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
TestOfflineRenderer<SceneType, CameraType>::TestOfflineRenderer(CameraType* Camera)
{
	SetCamera(Camera);
}

template<typename SceneType, typename CameraType>
void TestOfflineRenderer<SceneType, CameraType>::SetCamera(CameraType* Camera)
{
	if (!Camera) return;
	this->Camera = Camera;
	ScreenFilm.InitFilm(Camera, Camera->GetScreenWidth(), Camera->GetScreenHeight());
}




template<typename SceneType, typename CameraType>
void TestOfflineRendererTask<SceneType, CameraType>::Run()
{
	Spectrum Color;
	Color.SetValue(0, 1.0f);
	SceneType *Scene = Render->GetScene();
	Film<CameraType> *CameraFilm = Render->GetFilm();
	for (int i = StarPixelIndex ; i< EndPiexlIndex; i++)
	{
		SceneType::IntersectionType Intersection;
		BWRay Ray = CameraFilm->GetRayFromCamera(i);
		if (Scene->GetIntersectionInfo(Ray, Intersection))
		{
			Color = LightSampler::GetDirectionalLightLighting<Light , SceneType::IntersectionType>(Scene->GetLightByName(std::string("DirectionalLight")), &Intersection);
			CameraFilm->SetSpectrum(i, &Color);
		}
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


