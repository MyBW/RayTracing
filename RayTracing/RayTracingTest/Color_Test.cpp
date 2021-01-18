#include"gtest/gtest.h"
#include "..\V2\Util\ColorSpace.hpp"
#include "..\V2\Util\Rng.hpp"
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


TEST(RGBColorSpace, StdIllumWhiteRec2020) {
	XYZ xyz = SpectrumToXYZ(RGBColorSpace::Rec2020->Illuminant);
	RGB rgb = RGBColorSpace::Rec2020->ToRGB(xyz);
	EXPECT_GE(rgb.X, .99);
	EXPECT_LE(rgb.X, 1.01);
	EXPECT_GE(rgb.Y, .99);
	EXPECT_LE(rgb.Y, 1.01);
	EXPECT_GE(rgb.Z, .99);
	EXPECT_LE(rgb.Z, 1.01);
}

TEST(RGBColorSpace, StdIllumWhiteACES2065_1) {
	XYZ xyz = SpectrumToXYZ(RGBColorSpace::ACES2065_1->Illuminant);
	RGB rgb = RGBColorSpace::ACES2065_1->ToRGB(xyz);
	EXPECT_GE(rgb.X, .99);
	EXPECT_LE(rgb.X, 1.01);
	EXPECT_GE(rgb.Y, .99);
	EXPECT_LE(rgb.Y, 1.01);
	EXPECT_GE(rgb.Z, .99);
	EXPECT_LE(rgb.Z, 1.01);
}

TEST(RGBSpectrum, MaxValue) {
	RNG rng;
	for (const auto &cs :
	{ *RGBColorSpace::sRGB, *RGBColorSpace::Rec2020, *RGBColorSpace::ACES2065_1 }) {
		for (int i = 0; i < 100; ++i) {
			RGB rgb(rng.Uniform<float>(), rng.Uniform<float>(), rng.Uniform<float>());
			RGBReflectanceSpectrum rs(cs, rgb);

			float m = rs.MaxValue();
			float sm = 0;
			for (float lambda = 360; lambda <= 830; lambda += 1. / 16.)
				sm = std::max(sm, rs(lambda));
			EXPECT_LT(std::abs((sm - m) / sm), 1e-4)
			<< "sampled " << sm << " MaxValue " << m;// << " for " << rs;
		}
	}
}

TEST(RGBSpectrum, RoundTripsRGB) {
	RNG rng;
	const RGBColorSpace &cs = *RGBColorSpace::sRGB;

	for (int i = 0; i < 100; ++i) {
		RGB rgb(rng.Uniform<float>(), rng.Uniform<float>(), rng.Uniform<float>());
		RGBReflectanceSpectrum rs(cs, rgb);

		DenselySampledSpectrum rsIllum = DenselySampledSpectrum::SampleFunction(
			[&](float lambda) { return rs(lambda) * (*cs.Illuminant)(lambda); });
		XYZ xyz = SpectrumToXYZ(&rsIllum);
		RGB rgb2 = cs.ToRGB(xyz);

		// Some error comes from the fact that piecewise linear (at 5nm)
		// CIE curves were used for the optimization while we use piecewise
		// linear at 1nm spacing converted to 1nm constant / densely
		// sampled.
		float eps = .01;
		/*EXPECT_LT(std::abs(rgb.X - rgb2.X), eps) << rgb << " vs " << rgb2;
		EXPECT_LT(std::abs(rgb.Y - rgb2.Y), eps) << rgb << " vs " << rgb2;
		EXPECT_LT(std::abs(rgb.Z - rgb2.Z), eps) << rgb << " vs " << rgb2;*/

		EXPECT_LT(std::abs(rgb.X - rgb2.X), eps) ;//<< rgb << " vs " << rgb2;
		EXPECT_LT(std::abs(rgb.Y - rgb2.Y), eps) ;//<< rgb << " vs " << rgb2;
		EXPECT_LT(std::abs(rgb.Z - rgb2.Z), eps) ;//<< rgb << " vs " << rgb2;
	}
}

TEST(RGBSpectrum, RoundTripRec2020) {
	RNG rng;
	const RGBColorSpace &cs = *RGBColorSpace::Rec2020;

	for (int i = 0; i < 100; ++i) {
		RGB rgb(.1 + .7 * rng.Uniform<float>(), .1 + .7 * rng.Uniform<float>(),
			.1 + .7 * rng.Uniform<float>());
		RGBReflectanceSpectrum rs(cs, rgb);

		DenselySampledSpectrum rsIllum = DenselySampledSpectrum::SampleFunction(
			[&](float lambda) { return rs(lambda) * (*cs.Illuminant)(lambda); });
		XYZ xyz = SpectrumToXYZ(&rsIllum);
		RGB rgb2 = cs.ToRGB(xyz);

		float eps = .01;
		EXPECT_LT(std::abs(rgb.X - rgb2.X), eps);
			//<< rgb << " vs " << rgb2 << " xyz " << xyz;
		EXPECT_LT(std::abs(rgb.Y - rgb2.Y), eps);
			//<< rgb << " vs " << rgb2 << " xyz " << xyz;
		EXPECT_LT(std::abs(rgb.Z - rgb2.Z), eps);
			//<< rgb << " vs " << rgb2 << " xyz " << xyz;
	}
}

TEST(RGBSpectrum, RoundTripACES) {
	RNG rng;
	const RGBColorSpace &cs = *RGBColorSpace::ACES2065_1;

	for (int i = 0; i < 100; ++i) {
		RGB rgb(.3 + .4 * rng.Uniform<float>(), .3 + .4 * rng.Uniform<float>(),
			.3 + .4 * rng.Uniform<float>());
		RGBReflectanceSpectrum rs(cs, rgb);

		DenselySampledSpectrum rsIllum = DenselySampledSpectrum::SampleFunction(
			[&](float lambda) { return rs(lambda) * (*cs.Illuminant)(lambda); });
		XYZ xyz = SpectrumToXYZ(&rsIllum);
		RGB rgb2 = cs.ToRGB(xyz);

		float eps = .01;
		EXPECT_LT(std::abs(rgb.X - rgb2.X), eps);
			//<< rgb << " vs " << rgb2 << " xyz " << xyz;
		EXPECT_LT(std::abs(rgb.Y - rgb2.Y), eps);
			//<< rgb << " vs " << rgb2 << " xyz " << xyz;
		EXPECT_LT(std::abs(rgb.Z - rgb2.Z), eps);
			//<< rgb << " vs " << rgb2 << " xyz " << xyz;
	}
}

TEST(SRGB, LUTAccuracy) {
	const int n = 1024 * 1024;
	double sumErr = 0, maxErr = 0;
	RNG rng;
	for (int i = 0; i < n; ++i) {
		float v = (i + rng.Uniform<float>()) / n;
		float lut = LinearToSRGB(v);
		float precise = LinearToSRGBFull(v);
		double err = std::abs(lut - precise);
		sumErr += err;
		maxErr = std::max(err, maxErr);
	}
	// These bounds were measured empirically.
	EXPECT_LT(sumErr / n, 6e-6);  // average error
	EXPECT_LT(maxErr, 0.0015);
}

TEST(SRGB, 8ToLinearTable) {
	for (int v = 0; v < 255; ++v) {
		float err = std::abs(SRGBToLinear(v / 255.f) - SRGB8ToLinear(v));
		EXPECT_LT(err, 1e-6);
	}
}