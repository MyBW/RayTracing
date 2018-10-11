#pragma once
#include "../Math/Math.h"
class Camera
{
public:
	void Init();
	void InitProjectMatrix();

	
	BWVector3D GetDirection() const;
	BWVector3D GetUp() const;
	BWVector3D GetRight() const;
	void Move(BWVector3D &Dir);
	void RotateCamera(const BWVector3D& Axis, const Radian& Angle);
	void RotateCamera(const BWQuaternion &Q);
	void SetFixedYawAxis(bool IsFixed, const BWVector3D& fiexAxis = BWVector3D::UNIT_Y);
	void Yaw(const Radian& Angle);
	void Pitch(const Radian& Angle);
	void UpdateViewMatrix();
	void UpateProjectMatrix();
	void CalcProjectionParameters(float &left, float &right, float &bottom, float &top);
	BWMatrix4& GetViewMatrix();
	BWMatrix4& GetProjectMatrix();
private:
	BWMatrix4 ProjectMatrix;
	BWMatrix4 ViewMatrix;
	bool IsYawFixed;
	BWVector3D YawFixedAxis;
	BWQuaternion CameraOrientation;
	BWVector3D CameraPosition;
	float FarDist;
	float NearDist;
	float INFINITE_FAR_PLANE_ADJUST = 0.00001;
	Radian mFOVy;
	float mAspect; // ¿í¸ß±È
	float mOrhtoHeight;
	BWVector3D mFrustumOffset;

};
