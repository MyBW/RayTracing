#pragma once
template<typename IntersectionType>
class RTLight
{
public:
	virtual Spectrum Sample_L(const IntersectionType *Intersection) = 0;
};