#pragma once
#include "BWPrimitive.h"
#include "RTRenderer.h"
template<typename CameraType>
class Film
{
public:
	Film():Width(0), Height(0),FinalColor(nullptr) 
	{
		FilmBounds.SetMin(0, 0);
		FilmBounds.SetMin(1, 0);
		FilmBounds.SetMax(0, 0);
		FilmBounds.SetMax(1, 0);
	}
	void InitFilm(CameraType *Camera, int Width, int Height);
	void SetSpectrum(float WIndex, float HIndex, const Spectrum* Color);
	void SetSpectrum(int PixelInex, const Spectrum* Color);
	Spectrum* GetSpectrum(int PixelIndxe);
	int GetWidth() const { return Width; }
	int GetHeight() const { return Height; }
	BWRay GetRayFromCamera(float WIndex, float HIndex);
	BWRay GetRayFromCamera(int PixelIndex);
	void *GetData() { return FinalColor; }
	CameraType* GetCamera() { return Camera; }
	Bounds2i FilmBounds;
private:
	// X Y 是[0~1]的数据
	BWRay GetRayFromCameraWithNormal(float X, float Y);
	Spectrum *FinalColor;
	int Width;
	int Height;
	
	CameraType *Camera;
};



#include "Film.inl"