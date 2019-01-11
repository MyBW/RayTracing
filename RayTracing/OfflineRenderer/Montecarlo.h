#pragma once

void ConcentricSampleDisk(float u1, float u2, float *dx, float *dy);

BWVector3D ConsineSampleHemisphere(float U1, float U2);

inline float PowerHeuristic(int nf, float fPdf, int ng, float gPdf) 
{
	float f = nf * fPdf, g = ng * gPdf;
	return (f*f) / (f*f + g*g);
}

class Distribution1D
{
public:
	void ResetDistributionData(const std::vector<float> &Data);
	int SampleDistribute(float u, float &Pdf);
};