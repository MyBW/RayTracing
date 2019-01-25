#pragma once
#include "..\Scene\Light.h"
#include "..\OfflineRenderer\Sample.h"
#include "..\OfflineRenderer\Sampler.h"
#include "..\OfflineRenderer\Integrator.h"
#include "RNG.h"
template<typename SceneType>
void TestOfflineRenderer<SceneType>::InitSPPMPixel()
{
	Bounds2i& PixelBounds = ScreenFilm.FilmBounds;
	int PixelNum = PixelBounds.Area();
	SPPMPixels.resize(PixelNum);
	for (auto& P : SPPMPixels)
	{
		P = new SPPMPixel();
		P->Radius = InitialSearchRadius;
	}
}
template<typename SceneType>
void TestOfflineRenderer<SceneType>::RenderScene(SceneType* Scene)
{
	InitSPPMPixel();
	Distribution1D *LightDistr = CreateLightPowerDistribute(Scene);

	const float invSqrtSPP = 1.f / std::sqrt(IteratorNum);
	Random Sampler;
	//HaltonSampler sampler(nIterations, pixelBounds);

	Bounds2i& PixelBounds = ScreenFilm.FilmBounds;
	std::vector<int> Extent = PixelBounds.Diagonal();
	const int TileSize = 16;
	BWEle2I NTile((Extent[0] + TileSize - 1) / TileSize, (Extent[1] + TileSize - 1) / TileSize);
	for (int itor = 0 ; itor < IteratorNum; itor++)
	{
		//Generate SPPM visible points
		{
			ParallelProcess([&](std::vector<Task*>& Tasks)
			{
				for (int i = 0 ;i <  NTile.x; i++)
				{
					for (int j = 0; j < NTile.y; j++)
					{
						Tasks.push_back(new GenerateSPPMVisiblePointTask<SceneType>(i, j));
					}
				}
			});
		}
	}
}
template<typename SceneType>
void TestOfflineRenderer<SceneType>::ParallelProcess(std::function<void(std::vector<Task*>&)> CreateTask)
{
	std::vector<Task*> Tasks;
	CreateTask(Tasks);
	EnqueueTasks(Tasks);
	WaitTaskListFinish();
	CleanupTaskList();
}



template<typename SceneType>
void TestOfflineRendererTask<SceneType>::Run()
{
	SceneType *Scene = Render->GetScene();
	Film<typename SceneType::CameraType> *CameraFilm = Render->GetFilm();
	Sampler *MainSampelr = Render->GetMainSampler();
	Sampler *SubSampler = MainSampelr->GetSubSampler(StarPixelIndex, EndPiexlIndex);
	int MaxSampleCount = SubSampler->GetMaxSampleCount();
	std::vector<Sample*> Samples = Render->OrigSample->Duplicate(MaxSampleCount);
	RNG Rng;
	int SampleNum = 0;
	while ((SampleNum = SubSampler->GetMoreSamples(Samples, &Rng)) != 0)
	{
		Spectrum Color(0.0f);
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
				Color += Render->RendererIntegrator->Li(Scene, &Intersection, *Samples[i], Rng);
			}
		}
		Color = Color / SampleNum;
		CameraFilm->SetSpectrum(Samples[0]->ImageX, Samples[0]->ImageY, &Color);
	}
	delete SubSampler;
	for (auto SampleData : Samples)
	{
		delete SampleData;
	}
}

template<typename SceneType>
TestOfflineRendererTask<SceneType>::~TestOfflineRendererTask()
{

}

template<typename SceneType>
TestOfflineRendererTask<SceneType>::TestOfflineRendererTask(TestOfflineRenderer<SceneType> *Render, int StarPixelIndex, int EndPiexlIndex)
{
	this->Render = Render;
	this->StarPixelIndex = StarPixelIndex;
	this->EndPiexlIndex = EndPiexlIndex;
}

template<typename SceneType>
void GenerateSPPMVisiblePointTask<SceneType>::Run()
{

}


