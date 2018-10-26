#pragma once

class LightSampler
{
public:
	template<typename DirectionalLightType, typename IntersectionType>
	static Spectrum GetDirectionalLightLighting(DirectionalLightType *L, IntersectionType *Intersection);
};



#include "LightSampler.inl"