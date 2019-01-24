#pragma once
#include "Integrator.h"
#include "Montecarlo.h"
#include "RTLightSample.h"
template<typename SceneType, typename IntersectionType>
class DirectLightingIntegrator : public Integrator<SceneType, IntersectionType>
{
public:
	RTAreaLight<typename SceneType::AreaLightType, IntersectionType>* SampleAreaLight(IntersectionType *Intersection, const BWVector3D &LightDir , BWVector3D &PInLight, BWVector3D &NInLight)
	{
		BWRay Ray(Intersection->IntersectionPoint , LightDir, FLT_MAX);
		RTAreaLight<typename SceneType::AreaLightType, IntersectionType> *FinalLight  = nullptr;
		float THit = FLT_MAX;
		for (auto AreaLight : AreaLights)
		{
			if (AreaLight->Intersection(Ray, PInLight, NInLight, THit) &&  THit > ESP && THit < Ray.Length - ESP)
			{
				FinalLight = AreaLight;
				Ray.Length = THit;
			}
		}
		return FinalLight;
	}
	void RequestSample(Sample &InSample)
	{
		for (unsigned int i = 0; i < AllLights.size(); i++)
		{
			LightSampleOffsets.push_back(new LightSampleOffset(InSample, 1));
			BSDFSampleOffsets.push_back(new BSDFSampleOffset(InSample, 1));
		}
	}
	Spectrum TestLi(SceneType *InScene, IntersectionType *Intersction, RTLight<IntersectionType> *CurLight, BSDF &Bsdf, LightSample &LightSampleData, BSDFSample &BSDFSampleData)
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
				RTAreaLight<typename SceneType::AreaLightType, IntersectionType> *CurAreaLight = SampleAreaLight(Intersction, LightDir, PInLight, NInLight);
				VisibleTester VisibleTest;
				VisibleTest.SetRay(Intersction->IntersectionPoint, PInLight);
				if (CurAreaLight)
				{
					Li = CurAreaLight->L(Intersction, PInLight, NInLight);
				}
				else
				{
					Li = CurLight->L(Intersction , PInLight, NInLight); // 不是域光源 其他奇奇怪怪的光源
				}
				if (!Li.IsBlack() && !VisibleTest.IsBlock(InScene))
				{
					L += F * Li * Widght / BSDFPdf;
				}
			}
		}
		return L;
	}
	Spectrum Li(SceneType *InScene, IntersectionType *Intersction, Sample &InSample , RNG &Rng) override
	{
		Spectrum L;
		BSDF Bsdf;
		Intersction->Material->CreateBSDF(*Intersction, Bsdf);
		L = Bsdf.Le(Intersction->InputRay._vector);
		if (!L.IsBlack()) return L;
		for (int i = 0 ;i < AllLights.size() ; i++)
		{
			RTLight<IntersectionType> *CurLight = AllLights[i];
			if (i > LightSampleOffsets.size())
			{
				LightSample LightSampleData(Rng);
				BSDFSample BSDFSampleData(Rng);
				L += TestLi(InScene, Intersction, CurLight, Bsdf, LightSampleData, BSDFSampleData);
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
					Ld += TestLi(InScene, Intersction, CurLight, Bsdf, LightSampleData, BSDFSampleData);
				}
				L += Ld / CurLightSampleOffset.SampleNums;
			}
		}
		return L;
	}
	
private:
	std::vector<LightSampleOffset*>LightSampleOffsets;
	std::vector<BSDFSampleOffset *> BSDFSampleOffsets;
};