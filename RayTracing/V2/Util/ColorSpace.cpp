#include "ColorSpace.hpp"
using namespace BlackWalnut;
BlackWalnut::RGBColorSpace::RGBColorSpace(Vector2f InR, Vector2f InG, Vector2f InB, BaseSpectrum *InIlluminant, const RGBToSpectrumTable *InRGBToSpectrumTable)
	:R(InR),G(InG),B(InB),Illuminant(InIlluminant),RGBToSpectrumLookTable(InRGBToSpectrumTable)
{
			
}

