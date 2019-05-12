#pragma once
#include "..\Scene\Light.h"
#include "..\OfflineRenderer\Sample.h"
#include "..\OfflineRenderer\Sampler.h"
#include "..\OfflineRenderer\Integrator.h"
#include "RNG.h"

inline int GetSPPMPixelListIndex(int GridPos[3] , int GridRes[3])
{
	for (int i = 0 ;i < 3; i++)
	{
		if (GridPos[i] < 0 || GridPos[i] >= GridRes[i]) return -1;
	}
	return GridPos[0] + GridPos[1] * GridRes[0] + GridPos[2] * GridRes[0] * GridRes[1];
}
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
Distribution1D* TestOfflineRenderer<SceneType>::CreateLightPowerDistribute(SceneType *Scene)
{
	std::vector<RTLight*> &AllLight = GetIntegrator()->GetAllLight();
	std::vector<float> LightPower;
	for (size_t i = 0 ; i < AllLight.size(); i++)
	{
		RTLight *L = AllLight[i];
	     LightPower.push_back(L->Power().y());
	}
	Distribution1D *LightDistribute = new Distribution1D();
	LightDistribute->ResetDistributionData(LightPower);
	return LightDistribute;
}
template<typename SceneType>
void TestOfflineRenderer<SceneType>::RenderScene(SceneType* Scene)
{

	//{
	//	//Create grid of all SPPM visible point
	//	float MaxRadius = 0.5f;
	//	int GridRes[3] = { 0, 0, 0 };
	//	Bounds3f GridBounds(std::vector<float>{0, 0, 0}, std::vector<float>{10, 10, 10});
	//	std::vector<float> Diag = GridBounds.Diagonal();
	//	for (int i = 0; i < 3; i++)
	//	{
	//		GridRes[i] = TMax((int)(Diag[i] / MaxRadius + 1), 1);
	//	}
	//	const int GridNum = GridRes[0] * GridRes[1] * GridRes[2];
	//	std::vector<SPPMPixelList*> Grid(GridNum);
	//	float Radius = 0.5;
	//	BWVector3D P(0.5 ,0.5 , 0.5);
	//	int Max[3], Min[3];
	//	ToGrid(P - BWVector3D(Radius, Radius, Radius), GridBounds, GridRes, Min);
	//	ToGrid(P + BWVector3D(Radius, Radius, Radius), GridBounds, GridRes, Max);
	//	for (int x = Min[0]; x <= Max[0]; x++)
	//	{
	//		for (int y = Min[1]; y <= Max[1]; y++)
	//		{
	//			for (int z = Min[2]; z <= Max[2]; z++)
	//			{
	//				int PPos[3] = { x, y, z };
	//				//int N = Hash(P, Grid.size());
	//				int N = GetSPPMPixelListIndex(PPos, GridRes);
	//				SPPMPixelList *SPPMPixelListNode = new SPPMPixelList();
	//				SPPMPixelListNode->VisiblePixle =  new SPPMPixel();
	//				SPPMPixelListNode->VisiblePixle->VP.P = P;
	//				SPPMPixelListNode->Next = Grid[N];
	//				Grid[N] = SPPMPixelListNode;
	//			}
	//		}
	//	}
	//	BWVector3D SearchP(0.5, 0, 0);
	//	int SearchPPos[3];
	//	ToGrid(SearchP, GridBounds, GridRes, SearchPPos);
	//	int N = GetSPPMPixelListIndex(SearchPPos, GridRes);
	//	Grid[N];
	//	int a;
	//}


	IteratorNum = 8;
	PhotonNumPreItor = 80000;
	MaxTraceDepth = 20;
	InitialSearchRadius = 1.00f;

	InitSPPMPixel();
	this->Scene = Scene;
	this->Scene->UpdateSceneInfo();
	RendererIntegrator->Init(this->Scene);
	RendererIntegrator->RequestSample(*OrigSample);
	LightDistr = CreateLightPowerDistribute(Scene);
	//Random Sampler;
	//HaltonSampler sampler(nIterations, pixelBounds);

	const int TileSize = 32;
	
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
		int GridRes[3] = {0, 0, 0};
		Bounds3f GridBounds;
		float MaxRadius = 0;
		for (int i = 0 ;i < SPPMPixels.size(); i++)
		{
			SPPMPixel &Pixel = *SPPMPixels[i];
			Bounds3f PixelBound(std::vector<float>{Pixel.VP.P.x, Pixel.VP.P.y, Pixel.VP.P.z});
			PixelBound = PixelBound.Expand(Pixel.Radius);
			GridBounds.Union(PixelBound);
			MaxRadius = TMax(Pixel.Radius, MaxRadius);
		}
		std::vector<float> Diag = GridBounds.Diagonal();
		for (int i = 0 ;i < 3 ; i++)
		{
			GridRes[i] =  TMax((int)(Diag[i] / MaxRadius + 1) , 1);
		}
		/*DebugShowLine->push_back(BWVector3D(GridBounds.GetMin()[0], GridBounds.GetMin()[1], GridBounds.GetMin()[2]));
		DebugShowLine->push_back(BWVector3D(GridBounds.GetMin()[0] + Diag[0] , GridBounds.GetMin()[1] , GridBounds.GetMin()[2]));
		DebugShowLine->push_back(BWVector3D(GridBounds.GetMin()[0], GridBounds.GetMin()[1], GridBounds.GetMin()[2]));
		DebugShowLine->push_back(BWVector3D(GridBounds.GetMin()[0], GridBounds.GetMin()[1] + Diag[1], GridBounds.GetMin()[2]));
		DebugShowLine->push_back(BWVector3D(GridBounds.GetMin()[0], GridBounds.GetMin()[1], GridBounds.GetMin()[2]));
		DebugShowLine->push_back(BWVector3D(GridBounds.GetMin()[0], GridBounds.GetMin()[1], GridBounds.GetMin()[2] + Diag[2]));

		DebugShowLine->push_back(BWVector3D(GridBounds.GetMax()[0], GridBounds.GetMax()[1], GridBounds.GetMax()[2]));
		DebugShowLine->push_back(BWVector3D(GridBounds.GetMax()[0] - Diag[0], GridBounds.GetMax()[1], GridBounds.GetMax()[2]));
		DebugShowLine->push_back(BWVector3D(GridBounds.GetMax()[0], GridBounds.GetMax()[1], GridBounds.GetMax()[2]));
		DebugShowLine->push_back(BWVector3D(GridBounds.GetMax()[0], GridBounds.GetMax()[1] - Diag[1], GridBounds.GetMax()[2]));
		DebugShowLine->push_back(BWVector3D(GridBounds.GetMax()[0], GridBounds.GetMax()[1], GridBounds.GetMax()[2]));
		DebugShowLine->push_back(BWVector3D(GridBounds.GetMax()[0], GridBounds.GetMax()[1], GridBounds.GetMax()[2] - Diag[2]));*/
		
		//Add visible point to grid
		const int GridNum = GridRes[0] * GridRes[1] * GridRes[2];
		std::vector<SPPMPixelList*> Grid(SPPMPixels.size());
		Mutex *GridMutex = Mutex::Create();
		ParallelProcess([&](std::vector<Task*> &Tasks)
		{
			for (int i = 0; i < SPPMPixels.size(); i++)
			{
				Tasks.push_back(new AddVisiblePointToGridTask<SceneType>(i, SPPMPixels, GridBounds , Grid , GridRes[0], GridRes[1], GridRes[2], GridMutex));
			}
		},true);
		//Debug for Grid
		//for (int k = 0 ; k< GridRes[0]; k++)
		//{
		//	for (int i = 0; i < GridRes[1]; i++)
		//	{
		//		for (int j = 0; j < GridRes[2]; j++)
		//		{
		//			int GridPos[3] = { k , i, j };
		//			//int N = Hash(GridPos, GridNum);
		//			int N = GetSPPMPixelListIndex(GridPos, GridRes);
		//			SPPMPixelList *CurGridSPPM = Grid[N];
		//			while (CurGridSPPM)
		//			{
		//				SPPMPixel &PixelData = *CurGridSPPM->VisiblePixle;
		//				DebugShowLine->push_back(PixelData.VP.P);
		//				DebugShowLine->push_back(BWVector3D(PixelData.VP.P + BWVector3D(PixelData.Radius, PixelData.Radius, PixelData.Radius)));
		//				CurGridSPPM = CurGridSPPM->Next;
		//			}
		//		}
		//	}
		//}
		//
		//return;
		//Trace photons and accumulate contributions
		std::vector<RTLight*> &AllLights = GetIntegrator()->GetAllLight();
		ParallelProcess([&](std::vector<Task*> &Tasks)
		{
			for (int i = 0 ;i < PhotonNumPreItor ;i+=20)
			{
				TracePhotonsTask<SceneType> *NewTask = new TracePhotonsTask<SceneType>(this, i, 20, PhotonNumPreItor, itor, LightDistr,
					AllLights, MaxTraceDepth, GridBounds,
					GridRes[0], GridRes[1], GridRes[2], Grid, SPPMPixels, GridMutex);
				NewTask->DebugShowLine = DebugShowLine;
				Tasks.push_back(NewTask);
			}
			
		},true);

		//Update Pixle Value
		ParallelProcess([&](std::vector<Task*> &Tasks) {
			for (int i = 0 ;i < SPPMPixels.size() ; i++)
			{
				Tasks.push_back(new UpdateVisiblePointValueTask<SceneType>(i, SPPMPixels));
			}
		}, true);
		for (int i = 0 ; i < Grid.size() ; i++)
		{
			SPPMPixelList *SPPMPixelListNode = Grid[i];
			while (SPPMPixelListNode)
			{
				SPPMPixelList *CurNode = SPPMPixelListNode;
				SPPMPixelListNode = SPPMPixelListNode->Next;
				CurNode->Next = nullptr;
				delete CurNode;
			}
			Grid[i] = nullptr;
		}
		if (itor + 1 == IteratorNum)
		{
			std::vector<int> Min = ScreenFilm.FilmBounds.GetMin();
			std::vector<int> Max = ScreenFilm.FilmBounds.GetMax();
			unsigned long int Np = (unsigned long int)(IteratorNum) * (unsigned long int)PhotonNumPreItor;
			for (int i = Min[0] ;i < Max[0]; i++)
			{
				for (int j = Min[1] ;j < Max[1]; j++)
				{
					SPPMPixel &Pixel = *SPPMPixels[i + j * ScreenFilm.GetWidth()];
					Spectrum L = Pixel.Ld / IteratorNum;
				    L += Pixel.Tau / (Np * PI * Pixel.Radius * Pixel.Radius) * 150;
					ScreenFilm.SetSpectrum(i, j, &L);
				}
			}
		}
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
	SPPMPixel &CurSPPMPixel = *SPPMPixels[SPPMPixelIndex];
	if (!CurSPPMPixel.VP.Beta.IsBlack())
	{
		float Radius = CurSPPMPixel.Radius;
		int Max[3], Min[3];
		ToGrid(CurSPPMPixel.VP.P - BWVector3D(Radius , Radius, Radius), GridBound, GridRes, Min);
		ToGrid(CurSPPMPixel.VP.P + BWVector3D(Radius, Radius, Radius), GridBound, GridRes, Max);
		for (int x = Min[0] ; x <= Max[0];  x++)
		{
			for (int y = Min[1]; y <= Max[1]; y++)
			{
				for (int z = Min[2]; z <= Max[2]; z++)
				{
					int P[3] = { x, y, z };
					int N = Hash(P, SPPMPixels.size());
					//int N = GetSPPMPixelListIndex(P, GridRes);
					SPPMPixelList *SPPMPixelListNode = new SPPMPixelList();
					SPPMPixelListNode->VisiblePixle = &CurSPPMPixel;
					{
						MutexLock TMutexLock(*GridMutex);
						SPPMPixelListNode->Next = Grid[N];
						Grid[N] = SPPMPixelListNode;
					}
				}
			}
		}
	}
}


template<typename SceneType>
void TracePhotonsTask<SceneType>::Run()
{
	for (int PhotonIndex = CurrentPhotonIndex ; PhotonIndex < CurrentPhotonIndex + PhotonNumInTask ; PhotonIndex++)
	{
		SceneType *Scene = Render->GetScene();
		unsigned long long int HaltonIndex = CurItor * PhotonNumPreItor + PhotonIndex;
		int HaltonDim = 0;
		float LightPdf;

		float LightSample = RadicalInverse(HaltonDim++, HaltonIndex);
		int LightNum = LightsDistr->SampleDistribute(LightSample, LightPdf);
		RTLight *CurLight = Lights[LightNum];

		BWPoint2DD ULight(RadicalInverse(HaltonDim, HaltonIndex), RadicalInverse(HaltonDim + 1, HaltonIndex));
		BWPoint2DD VLight(RadicalInverse(HaltonDim + 2, HaltonIndex), RadicalInverse(HaltonDim + 3, HaltonIndex));
		HaltonDim += 4;

		BWRay PhotonRay;
		BWVector3D NInLight;
		float PosPdf;
		float DirPdf;
		Spectrum LightLe = CurLight->Sample_Le(ULight, VLight, PhotonRay, NInLight, PosPdf, DirPdf);
		if (LightLe.IsBlack() || PosPdf == 0.0f || DirPdf == 0.0f) return;
		Spectrum beta = (AbsDot(NInLight, PhotonRay._vector) * LightLe) / (LightPdf * PosPdf * DirPdf) * 0.05;
		if (beta.IsBlack()) return;
		std::vector<BWVector3D> Points;
		Points.push_back(PhotonRay._start);
		for (int CurDepht = 0; CurDepht < MaxTraceDepth; CurDepht++)
		{
			IntersectionInfo Intersection;
			float Mint = FLT_MAX;
			auto GetIntersectionInfo = [&Intersection, &Mint](const std::vector<BWPoint3DD>& P, const std::vector<BWPoint3DD>& N, const std::vector<BWPoint2DD>& UV, float t, float u, float v, const BWRay &Ray, const RTMaterial* Material, bool &IsBreak)
			{
				if (t < Mint && t > ESP)
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
			if (!Scene->GetIntersectionInfo(PhotonRay, GetIntersectionInfo)) return;

			if (CurDepht > 0)
			{
				int GridPos[3];
				if (ToGrid(Intersection.IntersectionPoint, GridBound, GridRes, GridPos))
				{
					int N = Hash(GridPos, SPPMPixels.size());
					//int N = GetSPPMPixelListIndex(GridPos, GridRes);
					{
						MutexLock GridLock(*GridMutex);
						SPPMPixelList *CurGridSPPM = Grid[N];
						while (CurGridSPPM)
						{
							SPPMPixel &PixelData = *CurGridSPPM->VisiblePixle;
							float Radius = PixelData.Radius;

							int PixelGridPos[3];
							ToGrid(PixelData.VP.P, GridBound, GridRes, PixelGridPos);
							std::vector<float> GridSize = GridBound.Diagonal();
							/*std::cout << "Grid Size£º" << GridSize[0] << " " << GridSize[1] << " " << GridSize[2] << std::endl;
							std::cout << "Grid Res: " << GridRes[0] << " "<<GridRes[1] << " "<<GridRes[2] << std::endl;
							std::cout << "VP :" << PixelGridPos[0] << " " << PixelGridPos[1] << " " << PixelGridPos[2] << std::endl;
							std::cout << "IntersectionPoint :" << GridPos[0] << " " << GridPos[1] << " " << GridPos[2] << std::endl;
							std::cout << Lenth(PixelData.VP.P - Intersection.IntersectionPoint) << " "<< Radius << std::endl;
							*/

							if (Lenth(PixelData.VP.P - Intersection.IntersectionPoint) <= Radius)
							{
								BWVector3D wi = -PhotonRay._vector;
								PixelData.M += 1;
								PixelData.Phi += beta * PixelData.VP.Bsdf->F(PixelData.VP.Wo, wi);
								//PixelData.Phi += beta;  // only undirect light input
								//if (beta.GetValue(0) == 1.0 && CurDepht == MaxTraceDepth - 1)
								//{
								//	for (int j = 0 ; j < Points.size();j++)
								//	{
								//		DebugShowLine->push_back(Points[j]);
								//	}
								//	DebugShowLine->push_back(Intersection.IntersectionPoint);
								//}

							}
							CurGridSPPM = CurGridSPPM->Next;
						}
					}
				}
			}
			//Compute Reflection Photon Ray
			BSDF CurBSDF;
			BWVector3D Wo, Wi;
			float BSDFPdf;
			BSDFSample BSDFSampleData;
			BXDF_TYPE BXDFType;
			BSDFSampleData.Component = RadicalInverse(HaltonDim, HaltonIndex);
			BSDFSampleData.Dir[0] = RadicalInverse(HaltonDim + 1, HaltonIndex);
			BSDFSampleData.Dir[1] = RadicalInverse(HaltonDim + 2, HaltonIndex);
			HaltonDim += 3;
			Wo = -PhotonRay._vector;

			Intersection.Material->CreateBSDF(Intersection, CurBSDF);
			Spectrum Fr = CurBSDF.Sample_F(Wo, Wi, BSDFPdf, BSDFSampleData, BXDFType);
			if (Fr.IsBlack() || BSDFPdf == 0.0f) return;

			Spectrum bnew = beta * Fr * AbsDot(Wi, Intersection.IntersectionNormal) / BSDFPdf;

			// Possibly terminate photon path with Russian roulette
			float q = 1 - bnew.y() / beta.y();
			q = q > 0 ? q : 0;
			if (RadicalInverse(HaltonDim++, HaltonIndex) < q) break;
			beta = bnew / (1 - q);
			PhotonRay._start = Intersection.IntersectionPoint;
			PhotonRay._vector = Wi;
			Points.push_back(PhotonRay._start);
		}
	}
	
}


template<typename SceneType>
void UpdateVisiblePointValueTask<SceneType>::Run()
{
	SPPMPixel &P = *SPPMPixels[Index];
	if (P.M > 0) {
		// Update pixel photon count, search radius, and $\tau$ from
		// photons
		float gamma = (float)2 / (float)3;
		float Nnew = P.N + gamma * P.M;
		float Rnew = P.Radius * std::sqrt(Nnew / (P.N + P.M));
		P.Tau = (P.Tau + P.VP.Beta * P.Phi) * (Rnew * Rnew) / (P.Radius * P.Radius);
		if (Index > 10000 && Index < 11000)
		{
			//std::cout << Index << " " << P.N << " " << Nnew << " " << P.Radius << " " << Rnew << std::endl;
		}
		P.N = Nnew;
		P.Radius = Rnew;
		P.M = 0;
		P.Phi = Spectrum(0.0f);
		
	}
	// Reset _VisiblePoint_ in pixel
	P.VP.Beta = 0.;
	delete P.VP.Bsdf;
	P.VP.Bsdf = nullptr;
}
