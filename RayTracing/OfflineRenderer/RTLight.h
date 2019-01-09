#pragma once
#include "BWPrimitive.h"
#include "Sample.h"
#include "RNG.h"
class LightSampleOffset
{
public:
	LightSampleOffset():SampleNums(0),LightComponentOffset(0),PosOffset(0){ }
	LightSampleOffset(Sample &InSample, int Num)
	{
		SampleNums = Num;
		LightComponentOffset = InSample.Add1D(SampleNums);
		PosOffset = InSample.Add2D(SampleNums);
	}

	int SampleNums;
	int LightComponentOffset;
	int PosOffset;
};
class LightSample
{
public:
	LightSample(Sample &InSample, LightSampleOffset &SampleOffset, int LightSampleIndex)
	{
		assert(LightSampleIndex < InSample.N1D[SampleOffset.LightComponentOffset]);
		assert(LightSampleIndex < InSample.N2D[SampleOffset.PosOffset]);
		LightComponent = InSample.N1Data[SampleOffset.LightComponentOffset][LightSampleIndex];
		Pos[0] = InSample.N2Data[SampleOffset.PosOffset][2 * LightSampleIndex];
		Pos[1] = InSample.N2Data[SampleOffset.PosOffset][2 * LightSampleIndex + 1];
		assert(LightComponent<1.0f && LightComponent > 0.0f);
		assert(Pos[0] < 1.0f && Pos[0] >= 0.0f);
		assert(Pos[1] < 1.0f && Pos[1] >= 0.0f);
	}

	LightSample(RNG &Rng)
	{
		LightComponent = Rng.GetRandomFloat();
		Pos[0] = Rng.GetRandomFloat();
		Pos[1] = Rng.GetRandomFloat();
	}
	float LightComponent;
	float Pos[2];
};

template<typename IntersectionType>
class RTLight
{
public:
	virtual BWVector3D GetLightDir(const IntersectionType *Intersection) = 0;
	virtual bool IsDeltaLight() = 0;
	virtual Spectrum Le(const IntersectionType *Intersection) = 0;
	virtual Spectrum Sample_L(const IntersectionType *Intersection , const LightSample &InLightSample, BWVector3D &LightDir , float &pdf) = 0;
	virtual float Pdf(const BWVector3D &P, const BWVector3D &Wi) = 0;
};

