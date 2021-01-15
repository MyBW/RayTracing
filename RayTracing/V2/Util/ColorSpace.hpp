#pragma once
#include "..\Math\Geom.hpp"
#include "Color.hpp"
#include "Spectrum.hpp"
#include "..\V2\Math\Matrix.hpp"
namespace BlackWalnut
{
	class RGBColorSpace
	{
	public:
		static const RGBColorSpace *sRGB, *DCI_P3, *Rec2020, *ACES2065_1;

		RGBColorSpace(Vector2f InR, Vector2f InG, Vector2f InB, BaseSpectrum *InIlluminant,
			const RGBToSpectrumTable *InRGBToSpectrumTable);
		static void Init();
		RGB ToRGB(const XYZ& xyz) const
		{
			return Mul<RGB, 3, XYZ>(RGBFromXYZ, xyz);
		}
		XYZ ToXYZ(const RGB& rgb) const
		{
			return Mul<XYZ, 3, RGB>(XYZFromRGB, rgb);
		}

		BaseSpectrum *Illuminant;
		Vector2f R, G, B, W;
		Matrix3X3f XYZFromRGB;
		Matrix3X3f RGBFromXYZ;
	private:
		
		
		const RGBToSpectrumTable *RGBToSpectrumLookTable;
	};
}