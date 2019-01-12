#pragma once
#include "Light.h"

class AreaLight : public Light
{
public:
	typedef Object AreaLightShapeType;
	std::vector<AreaLightShapeType*>& GetAllObject() { return AllObjects; }
	void AddAreaLightShapeType(AreaLightShapeType* AreaLightShape)
	{
		if (AreaLightShape) AllObjects.push_back(AreaLightShape);
	}
private:
	std::vector<AreaLightShapeType*> AllObjects;
};
