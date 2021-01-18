#include"gtest/gtest.h"
#include "..\V2\Util\Spectrum.hpp"
#include "..\V2\Util\Rng.hpp"
#include "..\V2\Util\ColorSpace.hpp"
using namespace BlackWalnut;
TEST(Spectrum, Blackbody) {
	// Relative error.
	auto err = [](float val, float ref) { return std::abs(val - ref) / ref; };

	// Planck's law.
	// A few values via
	// http://www.spectralcalc.com/blackbody_calculator/blackbody.php
	// lambda, T, expected radiance
	float v[][3] = {
		{ 483, 6000, 3.1849e13 },
		{ 600, 6000, 2.86772e13 },
		{ 500, 3700, 1.59845e12 },
		{ 600, 4500, 7.46497e12 },
	};
	int n = 4;
	for (int i = 0; i < n; ++i) {
		float lambda = v[i][0], T = v[i][1], LeExpected = v[i][2];
		EXPECT_LT(err(Blackbody(lambda, T), LeExpected), .001);
	}

	// Use Wien's displacement law to compute maximum wavelength for a few
	// temperatures, then confirm that the value returned by Blackbody() is
	// consistent with this.
	for (float T : {2700, 3000, 4500, 5600, 6000}) {
		float lambdaMax = 2.8977721e-3 / T * 1e9;
		float lambda[3] = { float(.999 * lambdaMax), lambdaMax, float(1.001 * lambdaMax) };
		EXPECT_LT(Blackbody(lambda[0], T), Blackbody(lambda[1], T));
		EXPECT_GT(Blackbody(lambda[1], T), Blackbody(lambda[2], T));
	}
}



TEST(Spectrum, XYZ) {
	{
		// Make sure the integral of all matching function sample values is
		// basically one in x, y, and z.
		float xx = 0, yy = 0, zz = 0;
		for (float lambda = 360; lambda < 831; ++lambda) {
			xx += X()(lambda);
			yy += Y()(lambda);
			zz += Z()(lambda);
		}
		static constexpr float CIE_Y_integral = 106.856895;
		xx /= CIE_Y_integral;
		yy /= CIE_Y_integral;
		zz /= CIE_Y_integral;
		EXPECT_LT(std::abs(1 - xx), .005) << xx;
		EXPECT_LT(std::abs(1 - yy), .005) << yy;
		EXPECT_LT(std::abs(1 - zz), .005) << zz;
	}
	{
		// Make sure the xyz of a constant spectrum are basically one.
		std::array<float, 3> xyzSum = { 0 };
		int n = 100;
		for (float u : Stratified1D(n)) {
			SampledWavelengths lambda = SampledWavelengths::SampleUniform(u, 360, 830);
			XYZ xyz = SampledSpectrum(1.).ToXYZ(lambda);
			for (int c = 0; c < 3; ++c)
				xyzSum[c] += xyz[c];
		}
		for (int c = 0; c < 3; ++c)
			xyzSum[c] /= n;

		EXPECT_LT(std::abs(1 - xyzSum[0]), .035) << xyzSum[0];
		EXPECT_LT(std::abs(1 - xyzSum[1]), .035) << xyzSum[1];
		EXPECT_LT(std::abs(1 - xyzSum[2]), .035) << xyzSum[2];
	}
}

TEST(Spectrum, MaxValue) {
	EXPECT_EQ(2.5, ConstantSpectrum(2.5).MaxValue());
	std::vector<float> L, V;
	L.push_back(300); 
	L.push_back(380);
	L.push_back(510);
	L.push_back(662);
	L.push_back(700);
	V.push_back(1.5);
	V.push_back(2.6);
	V.push_back(10.1);
	V.push_back(5.3);
	V.push_back(7.7);

	EXPECT_EQ(float(10.1),
		PiecewiseLinearSpectrum(L, V)
		.MaxValue());

	EXPECT_GT(BlackbodySpectrum(5000).MaxValue(), .9999);
	EXPECT_LT(BlackbodySpectrum(5000).MaxValue(), 1.0001);

	BlackbodySpectrum bb(5000);
	EXPECT_GT(DenselySampledSpectrum(bb).MaxValue(), .9999);
	EXPECT_LT(DenselySampledSpectrum(bb).MaxValue(), 1.0001);

	RNG rng;
	for (int i = 0; i < 20; ++i) {
		RGB rgb(rng.Uniform<float>(), rng.Uniform<float>(), rng.Uniform<float>());
		RGBReflectanceSpectrum sr(*RGBColorSpace::sRGB, rgb);
		float m = sr.MaxValue() * 1.00001f;
		for (float lambda = 360; lambda < 830; lambda += .92)
			EXPECT_LE(sr(lambda), m);

		RGBSpectrum si(*RGBColorSpace::sRGB, rgb);
		m = si.MaxValue() * 1.00001f;
		for (float lambda = 360; lambda < 830; lambda += .92)
			EXPECT_LE(si(lambda), m);
	}
}

TEST(Spectrum, SamplingPdfY) {
	// Make sure we can integrate the y matching curve correctly
	float ysum = 0;
	int n = 1000;
	for (float u : Stratified1D(n)) {
		float lambda = SampleXYZMatching(u);
		float pdf = XYZMatchingPDF(lambda);
		if (pdf > 0)
			ysum += Y()(lambda) / pdf;
	}
	float yint = ysum / n;

	EXPECT_LT(std::abs((yint - CIE_Y_integral) / CIE_Y_integral), 1e-3)
		<< yint << " vs. " << CIE_Y_integral;
}

TEST(Spectrum, SamplingPdfXYZ) {
	// Make sure we can integrate the sum of the x+y+z matching curves correctly
	float impSum = 0, unifSum = 0;
	int n = 10000;
	for (float u : Stratified1D(n)) {
		{
			// Uniform
			float lambda = Lerp(u, Lambda_min, Lambda_max);
			float pdf = 1. / (Lambda_max - Lambda_min);
			unifSum +=
				(X()(lambda) + Y()(lambda) + Z()(lambda)) /
				pdf;
		}

		float lambda = SampleXYZMatching(u);
		float pdf = XYZMatchingPDF(lambda);
		if (pdf > 0)
			impSum +=
			(X()(lambda) + Y()(lambda) + Z()(lambda)) /
			pdf;
	}
	float impInt = impSum / n, unifInt = unifSum / n;

	EXPECT_LT(std::abs((impInt - unifInt) / unifInt), 1e-3)
		<< impInt << " vs. " << unifInt;
}
