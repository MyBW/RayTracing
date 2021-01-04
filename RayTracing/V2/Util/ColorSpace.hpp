#pragma once
#include "..\Math\Geom.hpp"
#include "Color.hpp"
#include "Spectrum.hpp"
namespace BlackWalnut
{
	class RGBColorSpace
	{
	public:
		RGBColorSpace(Vector2f InR, Vector2f InG, Vector2f InB, BaseSpectrum *InIlluminant, 
			const RGBToSpectrumTable *InRGBToSpectrumTable);
		BaseSpectrum *Illuminant;
		Vector2f R, G, B, W;
	private:
		
		
		const RGBToSpectrumTable *RGBToSpectrumLookTable;
	};
}