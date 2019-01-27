#pragma once

#include <vector>
void ConcentricSampleDisk(float u1, float u2, float *dx, float *dy);
void UniformSampleTriangle(float ud1, float ud2, float *u, float *v);
BWVector3D ConsineSampleHemisphere(float U1, float U2);
float CosineHemispherePdf(float CosTheta);
inline float PowerHeuristic(int nf, float fPdf, int ng, float gPdf) 
{
	float f = nf * fPdf, g = ng * gPdf;
	return (f*f) / (f*f + g*g);
}

class Distribution1D
{
public:
	void ResetDistributionData(const std::vector<float> &InData);
	int SampleDistribute(float u, float &Pdf);
private:
	float funcInt;
	std::vector<float> Func;
	std::vector<float> CDF;
};

// Low Discrepancy Declarations
// Low Discrepancy Static Functions
template <int base>
static float RadicalInverseSpecialized(unsigned long int a) {
	const float invBase = (float)1 / (float)base;
	unsigned long int reversedDigits = 0;
	float invBaseN = 1;
	while (a) {
		unsigned long int next = a / base;
		unsigned long int digit = a - next * base;
		reversedDigits = reversedDigits * base + digit;
		invBaseN *= invBase;
		a = next;
	}
	return std::min(reversedDigits * invBaseN, OneMinusEpsilon);
}

// Low Discrepancy Inline Functions
inline unsigned long int ReverseBits32(unsigned long int n) {
	n = (n << 16) | (n >> 16);
	n = ((n & 0x00ff00ff) << 8) | ((n & 0xff00ff00) >> 8);
	n = ((n & 0x0f0f0f0f) << 4) | ((n & 0xf0f0f0f0) >> 4);
	n = ((n & 0x33333333) << 2) | ((n & 0xcccccccc) >> 2);
	n = ((n & 0x55555555) << 1) | ((n & 0xaaaaaaaa) >> 1);
	return n;
}

inline unsigned long int ReverseBits64(unsigned long int n) {
	unsigned long int n0 = ReverseBits32((unsigned int)n);
	unsigned long int n1 = ReverseBits32((unsigned int)(n >> 32));
	return (n0 << 32) | n1;
}
float RadicalInverse(int baseIndex, unsigned long int a);