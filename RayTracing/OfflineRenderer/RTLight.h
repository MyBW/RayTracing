#pragma once
template<typename IntersectionType>
class RTLight
{
public:
	virtual Spectrum Le(const IntersectionType *Intersection) = 0;
	virtual Spectrum Sample_L(const IntersectionType *Intersection , BWVector3D &LightDir) = 0;
};