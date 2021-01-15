#pragma  once
#include "InitResource.hpp"
void InitResource()
{
	Spectra::Init();
	RGBToSpectrumTable::Init();
	RGBColorSpace::Init();
}