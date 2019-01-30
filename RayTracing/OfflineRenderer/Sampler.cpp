#include"Sampler.h"
#include "RNG.h"
#include "Sample.h"


//int Random::GetMoreSamples(std::vector<Sample*>& Samples, RNG *InRNG)
//{
//	if (ImageSamplePos == nullptr)
//	{
//		ImageSamplePos = new float[SamplesPerPixel * 2];
//		for (int i = 0; i < SamplesPerPixel * 2; i += 2)
//		{
//			ImageSamplePos[i] = InRNG->GetRandomFloat();
//			ImageSamplePos[i + 1] = InRNG->GetRandomFloat();
//		}
//	}
//	if (CurrPixelSampleNum == SamplesPerPixel)
//	{
//		CurrSamplePos++;
//		if (CurrSamplePos > EndPixelIndex)
//		{
//			return 0;
//		}
//		for (int i = 0; i < SamplesPerPixel * 2; i+=2)
//		{
//			ImageSamplePos[i] = InRNG->GetRandomFloat();
//			ImageSamplePos[i + 1] = InRNG->GetRandomFloat();
//		}
//		ResetSamplePosition(CurrSamplePos);
//		CurrPixelSampleNum = 0;
//	}
//	for (size_t i = 0; i < Samples.size(); i++)
//	{
//		Samples[i]->ImageX = ImageSamplePos[CurrPixelSampleNum * 2] + CurrSamplePosX;
//		Samples[i]->ImageY = ImageSamplePos[CurrPixelSampleNum * 2 + 1] + CurrSamplePosY;
//		int OffSet = 0;
//		for (int k = 0; k < Samples[i]->N1D.size(); k++)
//		{
//			for (int j = 0; j < Samples[i]->N1D[k]; j++)
//			{
//				*(Samples[i]->N1Data[k] + j) = InRNG->GetRandomFloat();
//			}
//		}
//		for (int k = 0; k < Samples[i]->N2D.size(); k++)
//		{
//			for (int j = 0; j < Samples[i]->N2D[k]; j+=2)
//			{
//				*(Samples[i]->N2Data[k]+ j) = InRNG->GetRandomFloat();
//				*(Samples[i]->N2Data[k]+ j + 1) = InRNG->GetRandomFloat();
//			}
//		}
//		CurrPixelSampleNum++;
//	}
//	return SamplesPerPixel;
//}




int Random::GetMoreSamples(std::vector<Sample*>& Samples, RNG *InRNG)
{
	if (ImageSamplePos == nullptr)
	{
		ImageSamplePos = new float[SamplesPerPixel * 2];
		for (int i = 0; i < SamplesPerPixel * 2; i += 2)
		{
			ImageSamplePos[i] = InRNG->GetRandomFloat();
			ImageSamplePos[i + 1] = InRNG->GetRandomFloat();
		}
	}
	if (CurrPixelSampleNum == SamplesPerPixel)
	{
		if (!PixelArea.IncreasePosInArea(CurrentPixelPos)) return 0;
		for (int i = 0; i < SamplesPerPixel * 2; i += 2)
		{
			ImageSamplePos[i] = InRNG->GetRandomFloat();
			ImageSamplePos[i + 1] = InRNG->GetRandomFloat();
		}
		CurrPixelSampleNum = 0;
	}
	for (size_t i = 0; i < Samples.size(); i++)
	{
		Samples[i]->ImageX = ImageSamplePos[CurrPixelSampleNum * 2] + CurrentPixelPos[0];
		Samples[i]->ImageY = ImageSamplePos[CurrPixelSampleNum * 2 + 1] + CurrentPixelPos[1];
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
			for (int j = 0; j < Samples[i]->N2D[k]; j += 2)
			{
				*(Samples[i]->N2Data[k] + j) = InRNG->GetRandomFloat();
				*(Samples[i]->N2Data[k] + j + 1) = InRNG->GetRandomFloat();
			}
		}
		CurrPixelSampleNum++;
	}
	return SamplesPerPixel;
}

int Random::GetOneSample(Sample& InSample, RNG *InRNG)
{
	if (!PixelArea.IsInTheBound(CurrentPixelPos)) return 0;
	InSample.ImageX = InRNG->GetRandomFloat() + CurrentPixelPos[0];
	InSample.ImageY = InRNG->GetRandomFloat() + CurrentPixelPos[1];
	int OffSet = 0;
	for (int k = 0; k < InSample.N1D.size(); k++)
	{
		for (int j = 0; j < InSample.N1D[k]; j++)
		{
			*(InSample.N1Data[k] + j) = InRNG->GetRandomFloat();
		}
	}
	for (int k = 0; k < InSample.N2D.size(); k++)
	{
		for (int j = 0; j < InSample.N2D[k]; j += 2)
		{
			*(InSample.N2Data[k] + j) = InRNG->GetRandomFloat();
			*(InSample.N2Data[k] + j + 1) = InRNG->GetRandomFloat();
		}
	}
	PixelArea.IncreasePos(CurrentPixelPos);
	return 1;
}
