#include"gtest/gtest.h"
#include "..\V2\Util\ColorSpace.hpp"
using namespace BlackWalnut;

TEST(RGBColorSpace, RGBXYZ)
{
	for (const RGBColorSpace &cs :
	{ *RGBColorSpace::ACES2065_1, *RGBColorSpace::Rec2020, *RGBColorSpace::sRGB }) {
		XYZ xyz = cs.ToXYZ({ 1, 1, 1 });
		RGB rgb = cs.ToRGB(xyz);
		EXPECT_LT(std::abs(1 - rgb[0]), 1e-4);
		EXPECT_LT(std::abs(1 - rgb[1]), 1e-4);
		EXPECT_LT(std::abs(1 - rgb[2]), 1e-4);
	}
}

TEST(RGBColorSpace, sRGB) {
	const RGBColorSpace &sRGB = *RGBColorSpace::sRGB;

	// Make sure the matrix values are sensible by throwing the x, y, and z
	// basis vectors at it to pull out columns.
	RGB rgb = sRGB.ToRGB({ 1, 0, 0 });
	EXPECT_LT(std::abs(3.2406 - rgb[0]), 1e-3);
	EXPECT_LT(std::abs(-.9689 - rgb[1]), 1e-3);
	EXPECT_LT(std::abs(.0557 - rgb[2]), 1e-3);

	rgb = sRGB.ToRGB({ 0, 1, 0 });
	EXPECT_LT(std::abs(-1.5372 - rgb[0]), 1e-3);
	EXPECT_LT(std::abs(1.8758 - rgb[1]), 1e-3);
	EXPECT_LT(std::abs(-.2040 - rgb[2]), 1e-3);

	rgb = sRGB.ToRGB({ 0, 0, 1 });
	EXPECT_LT(std::abs(-.4986 - rgb[0]), 1e-3);
	EXPECT_LT(std::abs(.0415 - rgb[1]), 1e-3);
	EXPECT_LT(std::abs(1.0570 - rgb[2]), 1e-3);
}

TEST(RGBColorSpace, StdIllumWhitesRGB) {
	XYZ xyz = SpectrumToXYZ(RGBColorSpace::sRGB->Illuminant);
	RGB rgb = RGBColorSpace::sRGB->ToRGB(xyz);
	EXPECT_GE(rgb.X, .99);
	EXPECT_LE(rgb.X, 1.01);
	EXPECT_GE(rgb.Y, .99);
	EXPECT_LE(rgb.Y, 1.01);
	EXPECT_GE(rgb.Z, .99);
	EXPECT_LE(rgb.Z, 1.01);
}