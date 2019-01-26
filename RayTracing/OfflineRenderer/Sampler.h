#pragma once
#include <vector>
#include "BWPrimitive.h"
class Sample;
class RNG;
class Sampler
{
public:
	Sampler(const Bounds2i &InPixelArea, int InSPP) :PixelArea(InPixelArea), SamplesPerPixel(InSPP) { }
	virtual Sampler* GetSubSampler(const Bounds2i &SubSamplerPixleArea) = 0;
	virtual int GetMaxSampleCount() = 0;
	virtual int GetMoreSamples(std::vector<Sample*>& Samples, RNG *InRNG) = 0;
	virtual int GetOneSample(Sample& InSample, RNG *InRng) { return 0; }
protected:
	Bounds2i PixelArea;
	int SamplesPerPixel;
};

class Random : public Sampler
{
public:
	Random(const Bounds2i& PixelArea, int SPP) :Sampler(PixelArea, SPP), ImageSamplePos(nullptr)
	{
		CurrentPixelPos = PixelArea.GetMin();
	}
	~Random()
	{
		delete [] ImageSamplePos;
	}
	int GetMaxSampleCount() override { return SamplesPerPixel; }
	Sampler* GetSubSampler(const Bounds2i &SubSamplerPixleArea) override
	{
		return new Random(SubSamplerPixleArea, SamplesPerPixel);
	}
	int GetMoreSamples(std::vector<Sample *>& Samples, RNG *InRNG) override;
	int GetOneSample(Sample& InSample, RNG *InRng) override;
private:
	int CurrPixelSampleNum;
	float *ImageSamplePos;
	std::vector<int> CurrentPixelPos;
};