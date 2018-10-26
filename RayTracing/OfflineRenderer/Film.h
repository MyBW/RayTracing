#pragma once
#include "BWPrimitive.h"
#include "OfflineRenderer.h"
template<typename CameraType>
class Film
{
public:
	Film():Width(0), Height(0),FinalColor(nullptr) { }
	void InitFilm(CameraType *Camera, int Width, int Height);
	void SetSpectrum(int WIndex, int HIndex, const Spectrum* Color);
	void SetSpectrum(int PixelInex, const Spectrum* Color);
	Spectrum* GetSpectrum(int PixelIndxe);
	int GetWidth() const { return Width; }
	int GetHeight() const { return Height; }
	BWRay GetRayFromCamera(int WIndex, int HIndex);
	BWRay GetRayFromCamera(int PixelIndex);
	void *GetData() { return FinalColor; }
private:
	Spectrum *FinalColor;
	int Width;
	int Height;
	CameraType *Camera;
};



#include "Film.inl"