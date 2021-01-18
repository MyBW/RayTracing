#include "ColorSpace.hpp"
using namespace BlackWalnut;

const RGBColorSpace *RGBColorSpace::sRGB = nullptr;
const RGBColorSpace *RGBColorSpace::DCI_P3 = nullptr;
const RGBColorSpace *RGBColorSpace::Rec2020 = nullptr;
const RGBColorSpace *RGBColorSpace::ACES2065_1 = nullptr;


BlackWalnut::RGBColorSpace::RGBColorSpace(Vector2f InR, Vector2f InG, Vector2f InB, BaseSpectrum *InIlluminant, const RGBToSpectrumTable *InRGBToSpectrumTable)
	:R(InR),G(InG),B(InB),Illuminant(InIlluminant),RGBToSpectrumLookTable(InRGBToSpectrumTable)
{
	CHECK(InIlluminant);
	XYZ TmpW = SpectrumToXYZ(Illuminant);
	W = TmpW.xy();
	XYZ XYZR = XYZ::FromxyY(R);
	XYZ XYZG = XYZ::FromxyY(G);
	XYZ XYZB = XYZ::FromxyY(B);
	Matrix3X3f rgb = { XYZR.X, XYZG.X, XYZB.X, XYZR.Y, XYZG.Y, XYZB.Y, XYZR.Z, XYZG.Z, XYZB.Z };
	XYZ C = Mul<XYZ, 3, XYZ>(Inverse(rgb),TmpW) ;
	Matrix3X3f Tmp;
	Tmp.MakeZero();
	Tmp[0][0] = C[0];
	Tmp[1][1] = C[1];
	Tmp[2][2] = C[2];
	XYZFromRGB = rgb * Tmp;
	RGBFromXYZ = Inverse(XYZFromRGB);
}

BlackWalnut::RGBSigmoidPolynomial BlackWalnut::RGBColorSpace::ToRGBCoeffs(const RGB &Rgb) const
{
	CHECK(Rgb.X >= 0 && Rgb.Y >= 0 && Rgb.Z >= 0);
	return (*RGBToSpectrumLookTable)(ClampZero(Rgb));
}

void RGBColorSpace::Init()
{
	sRGB = new RGBColorSpace(Vector2f(.64, .33), Vector2f(.3, .6), Vector2f(.15, .06),
		GetNamedSpectrum("stdillum-D65"), RGBToSpectrumTable::sRGB);
	// P3-D65 (display)
	DCI_P3 = new RGBColorSpace(
		Vector2f(.68, .32), Vector2f(.265, .690), Vector2f(.15, .06),
		GetNamedSpectrum("stdillum-D65"), RGBToSpectrumTable::DCI_P3);

	// ITU-R Rec BT.2020
	Rec2020 = new RGBColorSpace(
		Vector2f(.708, .292), Vector2f(.170, .797), Vector2f(.131, .046),
		GetNamedSpectrum("stdillum-D65"), RGBToSpectrumTable::Rec2020);

	ACES2065_1 = new RGBColorSpace(
		Vector2f(.7347, .2653), Vector2f(0., 1.), Vector2f(.0001, -.077),
		GetNamedSpectrum("illum-acesD60"), RGBToSpectrumTable::ACES2065_1);
}