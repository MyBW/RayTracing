#pragma once
#include "Integrator.h"
#include "Montecarlo.h"
#include "RTLightSample.h"
template<typename SceneType>
class PathTracingIntegrator : public Integrator<SceneType>
{
public:
	Spectrum Li(SceneType *InScene, IntersectionInfo *Intersction, Sample &InSample, RNG &Rng) override
	{
		Spectrum L;
		BSDF Bsdf;
		Intersction->Material->CreateBSDF(*Intersction, Bsdf);
		L = Bsdf.Le(Intersction->InputRay._vector);
		// if hit the area light. then compute the emit light and return.
		if (!L.IsBlack()) return L;
		//uniform sample all lights.
		L += SampleAllLight(InScene, Intersction, InSample, Bsdf, Rng);

		return L;
	}
};