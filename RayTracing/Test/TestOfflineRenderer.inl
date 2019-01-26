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

	

	{
		//Random Sampler;
		//HaltonSampler sampler(nIterations, pixelBounds);
		this->Scene = Scene;
		this->Scene->UpdateSceneInfo();
		RendererIntegrator->Init(this->Scene);
		RendererIntegrator->RequestSample(*OrigSample);
	}
	

	const int TileSize = 64;
	IteratorNum = 6;
	const float invSqrtSPP = 1.f / std::sqrt(IteratorNum);
	Bounds2i& PixelBounds = ScreenFilm.FilmBounds;
	std::vector<int> Extent = PixelBounds.Diagonal();
	std::vector<Bounds2i> SubBounds;
	PixelBounds.SplitBounds(TileSize, SubBounds);
	for (int itor = 0 ; itor < IteratorNum; itor++)
	{
		//Generate SPPM visible points
		{
			ParallelProcess([&](std::vector<Task*>& Tasks)
			{
				for (size_t i = 0; i < SubBounds.size(); i++)
				{
					Tasks.push_back(new GenerateSPPMVisiblePointTask<SceneType>(this, SubBounds[i], SPPMPixels));
				}
			});
		}

		//Create grid of all SPPM visible point
		const int GridNum = SPPMPixels.size();
		std::vector<SPPMPixleList*> Grid(GridNum);
		int GridRes[3];
		Bounds3f GridBounds;
		float MaxRadius = 0;
		for (int i = 0 ;i < SPPMPixels.size(); i++)
		{
			SPPMPixel &Pixel = *SPPMPixels[i];
			Bounds3f PixelBound(std::vector<float>{Pixel.VP.P.x, Pixel.VP.y, Pixel.VP.z});
			PixelBound = PixelBound.Expand(Pixel.Radius);
			GridBounds.Union(PixelBound);
			MaxRadius = TMax(Pixel.Radius, MaxRadius);
		}

		std::vector<float> Diag = GridBounds.Diagonal();
		float MaxDiag = TMax(TMax(Diag[0], Diag[1]), Diag[2]);
		int BaseGridRes = MaxDiag / MaxRadius;
		for (int i = 0 ;i < 3 ; i++)
		{
			GridRes[i] =  TMax( (int)BaseGridRes * Diag[i] / MaxDiag , 1);
		}
		//Add visible point to grid
		ParallelProcess([&](std::vector<Task*> Tasks)
		{
			for (int i = 0; i < SPPMPixels.size(); i++)
			{
				Tasks.push_back(new AddVisiblePointToGridTask(i, SPPMPixels, GridBounds , Grid));
			}
		});

	}
}



template<typename SceneType>
void GenerateSPPMVisiblePointTask<SceneType>::Run()
{
	SceneType *Scene = Render->GetScene();
	Film<typename SceneType::CameraType> *CameraFilm = Render->GetFilm();
	Sampler *MainSampelr = Render->GetMainSampler();
	Sampler *SubSampler = MainSampelr->GetSubSampler(PixelBounds);
	std::vector<Sample*> Samples = Render->GetOrigSample()->Duplicate(1);
	RNG Rng;
	while (SubSampler->GetOneSample(*Samples[0], &Rng) != 0)
	{
		IntersectionInfo Intersection;
		int SPPMPixleOffset = int(Samples[0]->ImageX) + int(Samples[0]->ImageY) * CameraFilm->GetWidth();
		SPPMPixel *CurSPPMPixle = (*SPPMPixels)[SPPMPixleOffset];
		BWRay Ray = CameraFilm->GetRayFromCamera(Samples[0]->ImageX, Samples[0]->ImageY);
		float Mint = FLT_MAX;
		auto GetIntersectionInfo = [&Intersection, &Mint](const std::vector<BWPoint3DD>& P, const std::vector<BWPoint3DD>& N, const std::vector<BWPoint2DD>& UV, float t, float u, float v, const BWRay &Ray, const RTMaterial* Material, bool &IsBreak)
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
		if (!Scene->GetIntersectionInfo(Ray, GetIntersectionInfo))
		{
			CurSPPMPixle->Ld = Spectrum(0.0f);
			CurSPPMPixle->VP.Beta = Spectrum(0.0f);
			continue;
		}
		//Direction Lighting Integrator
		CurSPPMPixle->Ld += Render->GetIntegrator()->Li(Scene, &Intersection, *Samples[0], Rng);
		//Debug output
		/*Spectrum Color = CurSPPMPixle->Ld / 6;
		CameraFilm->SetSpectrum(Samples[0]->ImageX, Samples[0]->ImageY, &Color);*/
		CurSPPMPixle->VP.Beta = Spectrum(1.0);
		CurSPPMPixle->VP.Bsdf = new BSDF();
		Intersection.Material->CreateBSDF(Intersection, *CurSPPMPixle->VP.Bsdf);
		CurSPPMPixle->VP.P = Intersection.IntersectionPoint;
		CurSPPMPixle->VP.Wo = -Intersection.InputRay._vector;//!!warning this wo vector direction is diffrent
	}
}


template<typename SceneType>
void AddVisiblePointToGridTask<SceneType>::Run()
{
	
}

