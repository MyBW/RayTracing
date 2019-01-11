#include"Sampler.h"
#include "RNG.h"
#include "Sample.h"


int Random::GetMoreSamples(std::vector<Sample*>& Samples, RNG *InRNG)
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
	for (size_t i = 0; i < Samples.size(); i++)
	{
		Samples[i]->ImageX = ImageSamplePos[CurrPixelSampleNum * 2] + CurrSamplePosX;
		Samples[i]->ImageY = ImageSamplePos[CurrPixelSampleNum * 2 + 1] + CurrSamplePosY;
		int OffSet = 0;
		for (int k = 0; k < Samples[i]->N1D.size(); k++)
		{
			for (int j = 0; j < Samples[i]->N1D[k]; j++)
			{
				*(Samples[i]->N1Data[k] + j) = InRNG->GetRandomFloat();
			}
		}
		for (int k = 0; k < Samples[i]->N2D.size(); k++)
		{
			for (int j = 0; j < Samples[i]->N2D[k]; j+=2)
			{
				*(Samples[i]->N2Data[k]+ j) = InRNG->GetRandomFloat();
				*(Samples[i]->N2Data[k]+ j + 1) = InRNG->GetRandomFloat();
			}
		}
	}
	
	CurrPixelSampleNum++;
	return 1;
}