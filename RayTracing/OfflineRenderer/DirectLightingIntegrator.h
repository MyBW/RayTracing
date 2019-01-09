#pragma once
#include "Integrator.h"

template<typename SceneType, typename IntersectionType>
class DirectLightingIntegrator : public Integrator<SceneType, IntersectionType>
{
public:
	Spectrum TestLi(SceneType *InScene, IntersectionType *Intersction, RTLight<IntersectionType> *CurLight, BSDF &Bsdf, LightSample &LightSampleData, BSDFSample &BSDFSampleData)
	{
		//多重重要性采样
		// 对光源采样
		Spectrum L;
		BWVector3D LightDir;
		float LightPdf;
		float BSDFPdf;
		Spectrum Li = CurLight->Sample_L(Intersction, LightSampleData, LightDir, LightPdf);
		if (!Li.IsBlack() && LightPdf > 0.f)
		{
			Spectrum F = Bsdf.F(Intersction->InputRay._vector, LightDir);
			if (CurLight->IsDeltaLight())
			{
				L += Li * F * AbsDot(LightDir, Intersction->IntersectionNormal) / LightPdf;
			}
			else
			{
				BSDFPdf = Bsdf.Pdf(Intersction->InputRay._vector, LightDir);
				float Widght = PowerHeuristic(1, LightPdf, 1, BSDFPdf);
				L += Li * F * AbsDot(LightDir, Intersction->IntersectionNormal) * Widght / LightPdf;
			}
		}

		// 对BSDF采样
		if (!CurLight->IsDeltaLight())
		{
			BXDF_TYPE SampleBXDFType;
			Spectrum F = Bsdf.Sample_F(Intersction->InputRay._vector, LightDir, BSDFPdf, BSDFSampleData, SampleBXDFType);
			if (!F.IsBlack() && BSDFPdf > 0.)
			{
				float Widght = 1.f;
				if (!(SampleBXDFType & BSDF_SPECULAR))
				{
					LightPdf = CurLight->Pdf(Intersction->IntersectionPoint, LightDir);
					if (LightPdf == 0.f) continue;
					Widght = PowerHeuristic(1, BSDFPdf, 1, LightPdf);
				}
				/*
				Intersection lightIsect;
				Spectrum Li(0.f);
				RayDifferential ray(p, wi, rayEpsilon, INFINITY, time);
				if (scene->Intersect(ray, &lightIsect)) {
				if (lightIsect.primitive->GetAreaLight() == light)
				Li = lightIsect.Le(-wi);
				}
				else
				Li = light->Le(ray);
				if (!Li.IsBlack()) {
				Li *= renderer->Transmittance(scene, ray, NULL, rng, arena);
				Ld += f * Li * AbsDot(wi, n) * weight / bsdfPdf;
				}
				*/
				Li = CurLight->Le(Intersction);
				if (!Li.IsBlack())
				{
					L = F * Li * AbsDot(LightDir, Intersction->IntersectionNormal) * Widght / BSDFPdf;
				}
			}
		}
		return L;
	}
	Spectrum Test(SceneType *InScene, IntersectionType *Intersction, Sample &InSample , RNG &Rng)
	{
		Spectrum L;
		BSDF Bsdf;
		Intersction->Material->CreateBSDF(*Intersction, Bsdf);
		for (int i = 0 ;i < AllLights.size() ; i++)
		{
			RTLight *CurLight = AllLights[i];
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
				for (int j = 0; j < CurLightSampleOffset.SampleNums; j++)
				{
					LightSample LightSampleData(InSample, CurLightSampleOffset, j);
					BSDFSample BSDFSampleData(InSample, CurBSDFSampleOffset, j);
					L += TestLi(InScene, Intersction, CurLight, Bsdf, LightSampleData, BSDFSampleData);
				}
			}
		}
		return L;
	}
	Spectrum Li(SceneType *InScene, IntersectionType *Intersction, Sample &InSample) override
	{
		//计算积分
		Spectrum Color;
		BWVector3D LightDir;
		for (int i = 0;i< AllLights.size() ;i++)
		{
			Spectrum LightColor;
			BSDF Bsdf;
			Intersction->Material->CreateBSDF(*Intersction, Bsdf);
			LightColor = AllLights[i]->Le(Intersction);
			LightDir = AllLights[i]->GetLightDir(Intersction);
			Color += LightColor * Bsdf.F(Intersction->InputRay._vector, LightDir);
		}
		return Color;
	}
private:
	std::vector<LightSampleOffset*>LightSampleOffsets;
	std::vector<BSDFSampleOffset *> BSDFSampleOffsets;
};