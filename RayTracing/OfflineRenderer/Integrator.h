#pragma once
#include "RTDirectionLight.h"
#include "RTPointLight.h"
#include "RTAreaLight.h"
#include "RTLightSample.h"
#include "BSDF.h"
class Sample;
class RNG;
template<typename SceneType>
class Integrator
{
public:
	virtual void Init(SceneType *InScene);
	virtual void RequestSample(Sample &InSample)
	{
		for (unsigned int i = 0; i < AllLights.size(); i++)
		{
			LightSampleOffsets.push_back(new LightSampleOffset(InSample, 1));
			BSDFSampleOffsets.push_back(new BSDFSampleOffset(InSample, 1));
		}
	}
	std::vector<RTLight*>& GetAllLight() { return AllLights; }
	virtual Spectrum Li(SceneType *InScene, IntersectionInfo *Intersction , Sample &InSample, RNG& Rng) { return Spectrum(); };
	Spectrum EstimateDirect(SceneType *InScene, IntersectionInfo *Intersction, RTLight *CurLight, BSDF &Bsdf, LightSample &LightSampleData, BSDFSample &BSDFSampleData)
	{
		//多重重要性采样
		// 对光源采样
		Spectrum L;
		BWVector3D LightDir;
		float LightPdf = 0.0f;
		float BSDFPdf = 0.0f;
		VisibleTester Test;
		Spectrum Li = CurLight->Sample_L(Intersction, LightSampleData, LightDir, LightPdf, Test);
		if (!Li.IsBlack() && LightPdf > 0.f && !Test.IsBlock(InScene))
		{
			Spectrum F = Bsdf.F(Intersction->InputRay._vector, LightDir);
			if (!F.IsBlack())
			{
				F *= AbsDot(LightDir, Intersction->IntersectionNormal);
				if (CurLight->IsDeltaLight())
				{
					L += F * Li / LightPdf;
				}
				else
				{
					BSDFPdf = Bsdf.Pdf(Intersction->InputRay._vector, LightDir);
					float Weight = PowerHeuristic(1, LightPdf, 1, BSDFPdf);
					L += F * Li * Weight / LightPdf;
				}
			}
		}
		// 对BSDF采样
		if (!CurLight->IsDeltaLight())
		{
			BSDFPdf = 0.0f;
			LightPdf = 0.0f;
			Spectrum Li;
			BXDF_TYPE SampleBXDFType;
			Spectrum F = Bsdf.Sample_F(Intersction->InputRay._vector, LightDir, BSDFPdf, BSDFSampleData, SampleBXDFType);
			F *= AbsDot(LightDir, Intersction->IntersectionNormal);
			if (!F.IsBlack() && BSDFPdf > 0.)
			{
				float Widght = 1.f;
				//if (!(SampleBXDFType & BSDF_SPECULAR))
				if (true)
				{
					LightPdf = CurLight->Pdf(Intersction->IntersectionPoint, LightDir);
					if (LightPdf == 0.f) return L;
					Widght = PowerHeuristic(1, BSDFPdf, 1, LightPdf);
				}
				BWVector3D PInLight;
				BWVector3D NInLight;
				RTAreaLight<typename SceneType::AreaLightType> *CurAreaLight = SampleAreaLight(Intersction, LightDir, PInLight, NInLight);
				VisibleTester VisibleTest;
				VisibleTest.SetRay(Intersction->IntersectionPoint, PInLight);
				if (CurAreaLight)
				{
					Li = CurAreaLight->L(Intersction, PInLight, NInLight);
				}
				else
				{
					Li = CurLight->L(Intersction, PInLight, NInLight); // 不是域光源 其他奇奇怪怪的光源
				}
				if (!Li.IsBlack() && !VisibleTest.IsBlock(InScene))
				{
					L += F * Li * Widght / BSDFPdf;
				}
			}
		}
		return L;
	}
	Spectrum SampleAllLight(SceneType *InScene, IntersectionInfo *Intersction, Sample &InSample, BSDF &Bsdf, RNG& Rng)
	{
		Spectrum L;
		for (int i = 0; i < AllLights.size(); i++)
		{
			RTLight *CurLight = AllLights[i];
			if (i > LightSampleOffsets.size())
			{
				LightSample LightSampleData(Rng);
				BSDFSample BSDFSampleData(Rng);
				L += EstimateDirect(InScene, Intersction, CurLight, Bsdf, LightSampleData, BSDFSampleData);
			}
			else
			{
				LightSampleOffset &CurLightSampleOffset = *LightSampleOffsets[i];
				BSDFSampleOffset &CurBSDFSampleOffset = *BSDFSampleOffsets[i];
				Spectrum Ld;
				for (int j = 0; j < CurLightSampleOffset.SampleNums; j++)
				{
					LightSample LightSampleData(InSample, CurLightSampleOffset, j);
					BSDFSample BSDFSampleData(InSample, CurBSDFSampleOffset, j);
					Ld += EstimateDirect(InScene, Intersction, CurLight, Bsdf, LightSampleData, BSDFSampleData);
				}
				L += Ld / CurLightSampleOffset.SampleNums;
			}
		}
		return L;
	}
	RTAreaLight<typename SceneType::AreaLightType>* SampleAreaLight(IntersectionInfo *Intersection, const BWVector3D &LightDir, BWVector3D &PInLight, BWVector3D &NInLight)
	{
		BWRay Ray(Intersection->IntersectionPoint, LightDir, FLT_MAX);
		RTAreaLight<typename SceneType::AreaLightType> *FinalLight = nullptr;
		float THit = FLT_MAX;
		for (auto AreaLight : AreaLights)
		{
			if (AreaLight->Intersection(Ray, PInLight, NInLight, THit) && THit > ESP && THit < Ray.Length - ESP)
			{
				FinalLight = AreaLight;
				Ray.Length = THit;
			}
		}
		return FinalLight;
	}
	~Integrator()
	{
		for (auto Ele : DirectionLights)
		{
			delete Ele;
		}
		for (auto Ele : PointLights)
		{
			delete Ele;
		}
		for (auto Ele : AreaLights  )
		{
			delete Ele;
		}
	}
protected:
	std::vector<RTDirectionLight< typename SceneType::DirectionLightType>*> DirectionLights;
	std::vector<RTPointLight< typename SceneType::PointLightType>*> PointLights;
	std::vector<RTAreaLight<typename SceneType::AreaLightType>*> AreaLights;
	std::vector<RTLight*> AllLights;
	std::vector<LightSampleOffset*>LightSampleOffsets;
	std::vector<BSDFSampleOffset *> BSDFSampleOffsets;
};

template<typename SceneType>
void Integrator<SceneType>::Init(SceneType *InScene)
{
	std::vector<SceneType::DirectionLightType*>& SceneDirectionLights = InScene->GetAllDireciontLight();
	for (int i = 0; i < SceneDirectionLights.size(); i++)
	{
		DirectionLights.push_back(new RTDirectionLight<SceneType::DirectionLightType>());
		AllLights.push_back(DirectionLights[i]);
		DirectionLights[i]->SetLightSource(SceneDirectionLights[i]);
		DirectionLights[i]->SetRadiance(SceneDirectionLights[i]->GetRadiance());
	}
	std::vector<SceneType::PointLightType*>& ScenePointLights = InScene->GetAllPointLight();
	for (int i = 0; i < ScenePointLights.size(); i++)
	{
		PointLights.push_back(new RTPointLight<SceneType::PointLightType>());
		AllLights.push_back(PointLights[i]);
		PointLights[i]->SetLightSource(ScenePointLights[i]);
	}
	std::vector<SceneType::AreaLightType*>& SceneAreaLights = InScene->GetAllAreaLight();
	for (int i = 0; i < SceneAreaLights.size(); i++)
	{
		for (int n = 0 ;n < SceneAreaLights[i]->GetAllObject().size(); n++)
		{
			SceneType::AreaLightType::AreaLightShapeType* MeshData = SceneAreaLights[i]->GetAllObject()[n];
			int TriangleNum = MeshData->GetTriangleNum();
			const std::vector<BWVector3D> &PosData = MeshData->GetWorldPosData();
			const std::vector<BWVector3D> &NormalData = MeshData->GetWorldNormalData();
			const std::vector<BWPoint2DD> &UVData = MeshData->GetUVData();
			std::vector<BWVector3D> Pos;
			std::vector<BWVector3D> Normal;
			std::vector<BWPoint2DD> UV;
			
			for (int m = 0 ;m< TriangleNum ; m++)
			{
				TriganleLightShape*Mesh = new TriganleLightShape();
				Pos.resize(3);
				Normal.resize(3);
				UV.resize(3);
				Pos[0] = PosData[m * 3];
				Pos[1] = PosData[m * 3 + 1];
				Pos[2] = PosData[m * 3 + 2];
				Normal[0] = NormalData[m * 3];
				Normal[1] = NormalData[m * 3 + 1];
				Normal[2] = NormalData[m * 3 + 2];
				UV[0] = UVData[m * 2];
				UV[1] = UVData[m * 2 + 1];
				Mesh->SetTriangleShape(Pos, Normal, UV);
				AreaLights.push_back(new RTAreaLight<SceneType::AreaLightType>());
				AllLights.push_back(AreaLights[m]);
				AreaLights[m]->SetLightSource(SceneAreaLights[i]);
				AreaLights[m]->SetEmit(SceneAreaLights[i]->GetEmitColor());
				AreaLights[m]->SetShape(Mesh);
				Pos.clear();
				Normal.clear();
				UV.clear();
			}
		}
	}
}
