#pragma once

class Sampler
{
public:
	Sampler(int StartPixelIndex, int EndPixelIndex, int SPP):StartPixelIndex(StartPixelIndex),EndPixelIndex(EndPixelIndex),SamplesPerPixel(SPP)
	{

	}
	virtual int GetMaxSampleCount() = 0;
	virtual Sampler* GetSubSampler(int SubStartPixelIndex, int SubEndPixelIndex) = 0;
	virtual int GetMoreSamples(Sample *InSample, RNG &InRNG) = 0;
	int StartPixelIndex;
	int EndPixelIndex;
	int SamplesPerPixel;
};

class Random : public Sampler
{
public:
	Random(int StartPixelIndex, int EndPixelIndex, int SPP) :Sampler(StartPixelIndex, EndPixelIndex, SPP)
	{
		CurrSamplePos = StartPixelIndex;
	}
	int GetMaxSampleCount() override { return 1; }
	Sampler* GetSubSampler(int SubStartPixelIndex, int SubEndPixelIndex)
	{
		return new Random(SubEndPixelIndex, SubEndPixelIndex, SamplesPerPixel);
	}
	int GetMoreSamples(Sample *InSample, RNG &InRNG)
	{
		if (CurrSamplePos > EndPixelIndex)
		{
			return 0;
		}
		InSample->ImageX = CurrSamplePos + InRNG.GetRandomFloat();
		InSample->ImageY = CurrSamplePos + InRNG.GetRandomFloat();
		InSample->AllocDataMemory();
		for (int i = 0; i < InSample->N1D.size();i++)
		{
			for (int j = 0; j < InSample->N1D[i];j++)
			{
				InSample->N1Data[j] = InRNG.GetRandomFloat();
			}
		}
		for (int i = 0; i < InSample->N2D.size();i++)
		{
			for (int j = 0; j < InSample->N2D[i]; j++)
			{
				InSample->N2Data[j * 2] = InRNG.GetRandomFloat();
				InSample->N2Data[j * 2 + 1] = InRNG.GetRandomFloat();
			}
		}
		return 1;
	}
private:
	int CurrSamplePos;
};