#pragma once
template<typename SceneType, typename CameraType>
void TestOfflineRenderer<SceneType, CameraType>::RenderScene(SceneType* Scene)
{
	
}

template<typename SceneType, typename CameraType>
TestOfflineRenderer<SceneType, CameraType>::TestOfflineRenderer(const CameraType* Camera)
{
	this->Camera = Camera;
}





template<typename SceneType, typename CameraType>
void TestOfflineRendererTask<SceneType, CameraType>::Run()
{
	SceneType *Scene = Render->GetScene();
	Film *CameraFilm = Render->GetFilm();
	for (int i = StarPixelIndex ; i< EndPiexlIndex; i++)
	{
		SceneType::IntersectionType Intersection;
		BWRay Ray = CameraFilm->GetRayFromCamera(i);
		if (Scene->GetIntersectionInfo(Ray, Intersection))
		{
			Spectrum Color;
			Color.C[0] = 1.0;
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


