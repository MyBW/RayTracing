#pragma once

class RTMaterial
{
public:
	BSDF& GetLightingModel() { return LightingModel; }
private:
	BSDF LightingModel;
};