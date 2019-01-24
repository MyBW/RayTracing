#pragma once
#include <vector>
#include <string>
#include "..\Math\BWPrimitive.h"
#include <functional>
class RTMaterial;
struct TriangleInfo
{
	std::vector<BWVector3D> P;
	std::vector<BWVector3D> N;
	std::vector<BWPoint2DD> UV;
	inline void Resize(int Size)
	{
		P.resize(Size);
		N.resize(Size);
		UV.resize(Size);
	}
	inline void ClearData()
	{
		P.clear();
		N.clear();
		UV.clear();	
	}
};
class Object
{
public:
	Object();
	bool LoadObjModel(const char* FileName , const std::string &Name);
	
	std::string& GetName() { return Name; }
	void SetPosition(float X, float Y, float Z);
	void SetRotation(float w, float x, float y, float z);
	void SetRoataion(const BWVector3D& axis, const Radian &angle);
	void SetScale(float x, float y, float z);
	int GetTriangleNum();
	bool GetTriangleInfoByIndex(int Index , TriangleInfo &Reslut);
	bool GetTriangleWorldInfoByIndex(int Index, TriangleInfo &Reslut);
	bool GetTriangleVertexIndex(int Index, int &P1, int &P2, int &P3);
	const BWMatrix4& GetModelMatrix() const;
	const std::vector<BWVector3D>& GetPosData() const;
	const std::vector<BWPoint2DD>& GetUVData() const;
	const std::vector<BWVector3D>& GetNormalData() const;
	const std::vector<unsigned int>& GetIndexData() const;
	const std::vector<BWPoint2DD>& GetLightmapUVData() const;
	const std::vector<BWVector3D>& GetWorldPosData() const;
	const std::vector<BWVector3D>& GetWorldNormalData() const;
	void GetAABB(BWVector3D &Min, BWVector3D &Max) const;
	void UpdateWorldInfor();
	const RTMaterial *GetMaterial() const  { return Material; } 
	bool GetIntersectionInfo(const BWRay& ray, std::function<void(const std::vector<BWPoint3DD>& P, const std::vector<BWPoint3DD>& N, const std::vector<BWPoint2DD>& UV, float t, float u, float v, const BWRay &Ray, const RTMaterial*, bool &IsBreak)> IntersectionCallBack);
	RTMaterial *Material;
private:
	void UpdateModelMatrix();
	void ComputeBoundingBox();
	void UpdateAABB();
	BWVector3D BoundingBoxMin;
	BWVector3D BoundingBoxMax;
	BWVector3D AABBMin;
	BWVector3D AABBMax;
	std::string Name;
	BWMatrix4 ModelMatrix;
	BWVector3D Position;
	BWVector3D Scale;
	BWQuaternion Orientation;

	std::vector<BWVector3D> PosData;
	std::vector<BWPoint2DD> UVData;
	std::vector<BWVector3D> NormalData;
	std::vector<unsigned int> IndexData;
	std::vector<BWPoint2DD> LightmapUV;
	std::vector<BWVector3D> WorldPosData;
	std::vector<BWVector3D> WorldNormalData;
};
