#pragma once
class Integrator
{
public:
	template<typename SceneType, typename IntersectionType >
	virtual Spectrum Li(SceneType *InScene, IntersectionType *Intersction) = 0;
private:
};