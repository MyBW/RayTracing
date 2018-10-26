#pragma once
#include <string>
#include "BWPrimitive.h"

class Light
{
public:
	Light();
	~Light();
	void SetPosition(BWVector3D &Pos);
	void SetRoataion(const BWVector3D& axis, const Radian &angle);
	void SetRotation(float w, float x, float y, float z);
	void SetScale(float x, float y, float z);
	BWVector3D& GetPosition() { return Position; }
	std::string& GetName() { return Name; }
	void SetName(std::string &Name) { this->Name = Name; }
private:
	void UpdateMatrix();
	std::string Name;

	BWMatrix4 LightMatrix;
	BWVector3D Position;
	BWVector3D Scale;
	BWQuaternion Orientation;
};
