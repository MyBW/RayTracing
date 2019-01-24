#pragma once

#include <vector>
void ConcentricSampleDisk(float u1, float u2, float *dx, float *dy);
void UniformSampleTriangle(float ud1, float ud2, float *u, float *v);
BWVector3D ConsineSampleHemisphere(float U1, float U2);

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