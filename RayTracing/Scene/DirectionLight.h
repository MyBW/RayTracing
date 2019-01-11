#pragma once
#include "Light.h"
#include "..\OfflineRenderer\RTRenderer.h"
class DirectionLight : public Light
{
public:
	void SetDirection(BWVector3D &Dir)
	{
		LightDir = Dir;
	}
	const BWVector3D& GetDirection()
	{
		return LightDir;
	}
	Spectrum& GetRadiance() { return LightRadiance; }
	Spectrum LightRadiance;
public:
	BWVector3D LightDir;
	
};