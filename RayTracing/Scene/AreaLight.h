#pragma once
#include "Light.h"
#include "..\Test\TestMaterial.h"

class AreaLight : public Light
{
public:
	AreaLight() = default;
	~AreaLight() { }
	typedef Object AreaLightShapeType;
	void SetEmitColor(const Spectrum &InEmitColor)
	{
		EmitColor = InEmitColor;
	}
	std::vector<AreaLightShapeType*>& GetAllObject() { return AllObjects; }
	void AddAreaLightShapeType(AreaLightShapeType* AreaLightShape)
	{
		if (AreaLightShape)
		{
			AreaLightShape->Material = new EmitMaterial(EmitColor);
			AllObjects.push_back(AreaLightShape);
		}
	}
	Spectrum& GetEmitColor() { return EmitColor; }
private:
	Spectrum EmitColor;
	std::vector<AreaLightShapeType*> AllObjects;
};
