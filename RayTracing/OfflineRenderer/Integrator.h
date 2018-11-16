#pragma once
template<typename SceneType, typename IntersectionType>
class Integrator
{
public:
	virtual Spectrum Li(SceneType *InScene, IntersectionType *Intersction) { };
private:
};