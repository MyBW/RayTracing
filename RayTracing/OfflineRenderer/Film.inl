#pragma once
template<typename CameraType>
BWRay Film<CameraType>::GetRayFromCamera(int WIndex, int HIndex)
{
	if (WIndex < 0 || WIndex >= Width || HIndex < 0 || HIndex >= Height)
	{
		return BWRay();
	}
	float X = float(WIndex) / Width;
	float Y = float(HIndex) / Height;
	BWRay Ray;
	Ray._start = Camera->GetPosition();
	Ray._vector = Camera->GetViewportPositionInWorldSpace(X, Y);
	return Ray;
}
template<typename CameraType>
BWRay Film<CameraType>::GetRayFromCamera(int PixelIndex)
{
	int WIndex = PixelIndex % Width;
	int HIndex = PixelIndex / Height;
	return GetRayFromCamera(WIndex, HIndex);
}

template<typename CameraType>
void Film<CameraType>::SetSpectrum(int WIndex, int HIndex, const Spectrum* Color)
{
	if (WIndex < 0 || WIndex >= Width || HIndex < 0 || HIndex >= Height)
	{
		return;
	}
	FinalColor[WIndex * HIndex] = *Color;
}

template<typename CameraType>
void Film<CameraType>::SetSpectrum(int PixelInex, const Spectrum* Color)
{
	if (PixelInex < Width * Height && PixelInex > -1)
	{
		int WIndex = PixelIndex % Width;
		int HIndex = PixelIndex / Height;
		SetSpectrum(WIndex, HIndex, Color);
	}
}
template<typename CameraType>
void Film<CameraType>::InitFilm(CameraType *Camera, int Width, int Height)
{

	if (Width <= 0 || Height <= 0 || !Camera)
	{
		return;
	}
	if (FinalColor) delete[] FinalColor;
	this->Width = Width;
	this->Height = Height;
	this->Camera = Camera;
	FinalColor = new Spectrum[Width * Height];
}

