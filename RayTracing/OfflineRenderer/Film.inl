#pragma once
template<typename CameraType>
BWRay Film<CameraType> ::GetRayFromCameraWithNormal(float X, float Y)
{
	BWRay Ray;
	Ray._start = Camera->GetPosition();
	Ray._vector =  GetDirection(Ray._start , Camera->GetViewportPositionInWorldSpace(X, Y));
	return Ray;
}
template<typename CameraType>
BWRay Film<CameraType>::GetRayFromCamera(float WIndex, float HIndex)
{
	if (WIndex < 0 || WIndex >= Width || HIndex < 0 || HIndex >= Height)
	{
		return BWRay();
	}
	float X = float(WIndex) / Width;
	float Y = float(HIndex) / Height;
	return GetRayFromCameraWithNormal(X, Y);
}
template<typename CameraType>
BWRay Film<CameraType>::GetRayFromCamera(int PixelIndex)
{
	int WIndex = PixelIndex % Width;
	int HIndex = PixelIndex / Width;
	return GetRayFromCamera(WIndex, HIndex);
}

template<typename CameraType>
void Film<CameraType>::SetSpectrum(float WIndex, float HIndex, const Spectrum* Color)
{
	if (WIndex < 0.0f || WIndex >= Width || HIndex < 0.0f || HIndex >= Height)
	{
		return;
	}
	FinalColor[int(WIndex) + int(HIndex) * int(Width) ] = *Color;
}

template<typename CameraType>
void Film<CameraType>::SetSpectrum(int PixelInex, const Spectrum* Color)
{
	if (PixelInex < Width * Height && PixelInex > -1)
	{
		int WIndex = PixelInex % Width;
		int HIndex = PixelInex / Width;
		SetSpectrum(WIndex, HIndex, Color);
	}
}
template<typename CameraType>
Spectrum* Film<CameraType>::GetSpectrum(int PixelIndex)
{
	if (PixelIndex < Width * Height && PixelIndex > -1)
	{
		return &FinalColor[PixelIndex];
	}
	return nullptr;
}

template<typename CameraType>
void Film<CameraType>::InitFilm(CameraType *Camera, int Width, int Height)
{

	if (Width <= 0 || Height <= 0 || !Camera || Camera == this->Camera)
	{
		return;
	}
	if (FinalColor) delete[] FinalColor;
	this->Width = Width;
	this->Height = Height;
	this->Camera = Camera;
	FinalColor = new Spectrum[Width * Height];
}

