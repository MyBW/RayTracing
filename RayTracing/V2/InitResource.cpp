#pragma  once
#include "InitResource.hpp"
void InitResource()
{
	using namespace BlackWalnut;
	Spectra::Init();
	RGBToSpectrumTable::Init();
	RGBColorSpace::Init();
}