#pragma once
template<typename IntersectionType>
class OfflineLight
{
public:
	virtual Spectrum Sample_L(const IntersectionType *Intersection) = 0;
};