#include "Light.h"

Light::Light()
{

}
Light::~Light()
{
}
void Light::SetPosition(BWVector3D &Pos)
{
	Position = Pos;
	UpdateMatrix();
}

void Light::SetRoataion(const BWVector3D& axis, const Radian &angle)
{
	BWQuaternion q;
	q.fromAngleAxis(angle, axis);
	q.normalize();
	Orientation = Orientation * q;
	UpdateMatrix();
}

void Light::SetRotation(float w, float x, float y, float z)
{
	Orientation.set(w, x, y, z);
	UpdateMatrix();
}

void Light::SetScale(float x, float y, float z)
{
	Scale.x = x;
	Scale.y = y;
	Scale.z = z;
	UpdateMatrix();
}

void Light::UpdateMatrix()
{
	LightMatrix.makeTransform(Position, Scale, Orientation);
}

