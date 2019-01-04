#include"Sampler.h"
#include "RNG.h"
#include "Sample.h"


int Random::GetMoreSamples(Sample *InSample, RNG *InRNG)
{
	if (ImageSamplePos == nullptr)
	{
		ImageSamplePos = new float[SamplesPerPixel * 2];
		for (int i = 0; i < SamplesPerPixel * 2; i += 2)
		{
			ImageSamplePos[i * 2] = InRNG->GetRandomFloat();
			ImageSamplePos[i * 2 + 1] = InRNG->GetRandomFloat();
		}
	}
	if (CurrPixelSampleNum == SamplesPerPixel)
	{
		CurrSamplePos++;
		if (CurrSamplePos > EndPixelIndex)
		{
			return 0;
		}
		for (int i = 0; i < SamplesPerPixel * 2; i+=2)
		{
			ImageSamplePos[i * 2] = InRNG->GetRandomFloat();
			ImageSamplePos[i * 2 + 1] = InRNG->GetRandomFloat();
		}
		ResetSamplePosition(CurrSamplePos);
		CurrPixelSampleNum = 0;
	}
	InSample->ImageX = ImageSamplePos[CurrPixelSampleNum * 2] + CurrSamplePosX;
	InSample->ImageY = ImageSamplePos[CurrPixelSampleNum * 2 + 1] + CurrSamplePosY;
	InSample->AllocDataMemory();
	for (int i = 0; i < InSample->N1D.size(); i++)
	{
		for (int j = 0; j < InSample->N1D[i]; j++)
		{
			*(InSample->N1Data[j]) = InRNG->GetRandomFloat();
		}
	}
	for (int i = 0; i < InSample->N2D.size(); i++)
	{
		for (int j = 0; j < InSample->N2D[i]; j++)
		{
			*(InSample->N2Data[j * 2]) = InRNG->GetRandomFloat();
			*(InSample->N2Data[j * 2 + 1]) = InRNG->GetRandomFloat();
		}
	}
	CurrPixelSampleNum++;
	return 1;
}