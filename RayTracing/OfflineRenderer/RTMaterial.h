#pragma once
#include "BSDF.h"

class RTMaterial
{
public:
	RTMaterial()
	{
		LightingModel.SetMaterial(this);
	}
	template<typename IntersectionInfoType>
	void InitRTMaterial(IntersectionInfoType& IntersectionInfo)
	{
		IntersectionNormal = IntersectionInfo.IntersectionNormal;
		IntersectionUV = IntersectionInfo.IntersectionUV;
	}
	BSDF& GetLightingModel() { return LightingModel; }
	Spectrum GetBaseColor()
	{ 
		Spectrum Color;  
		Color.SetValue(0, 1.0);
		return Color;
	}
	BWVector3D GetNormal() { return IntersectionNormal; }
private:
	BSDF LightingModel;
	BWVector3D IntersectionNormal;
	BWPoint2DD IntersectionUV;
};