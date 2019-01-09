#pragma once
#include "BSDF.h"
class IntersectionInfo;
class RTMaterial
{
public:
	RTMaterial()
	{
	}	
	virtual void CreateBSDF(const IntersectionInfo& Intersection , BSDF &Bsdf) const ;
private:
};



