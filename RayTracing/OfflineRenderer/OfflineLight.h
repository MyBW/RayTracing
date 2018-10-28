#pragma once
class OfflineLight
{
public:
	template<typename IntersectionType>
	virtual Spectrum Sample_L(const IntersectionType &Intersection) = 0;
};