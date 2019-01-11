#pragma once
#include "BWPrimitive.h"
#include "Sample.h"
#include "RNG.h"
class LightSampleOffset
{
public:
	LightSampleOffset():SampleNums(0),ComponentOffset(0),PosOffset(0){ }
	LightSampleOffset(Sample &InSample, int Num)
	{
		SampleNums = Num;
		ComponentOffset = InSample.Add1D(SampleNums);
		PosOffset = InSample.Add2D(SampleNums);
	}

	int SampleNums;
	int ComponentOffset;
	int PosOffset;
};
class LightSample
{
public:
	LightSample(Sample &InSample, LightSampleOffset &SampleOffset, int LightSampleIndex)
	{
		assert(LightSampleIndex < InSample.N1D[SampleOffset.ComponentOffset]);
		assert(LightSampleIndex < InSample.N2D[SampleOffset.PosOffset]);
		ShapeComponent = InSample.N1Data[SampleOffset.ComponentOffset][LightSampleIndex];
		Pos[0] = InSample.N2Data[SampleOffset.PosOffset][2 * LightSampleIndex];
		Pos[1] = InSample.N2Data[SampleOffset.PosOffset][2 * LightSampleIndex + 1];
		assert(ShapeComponent<1.0f && ShapeComponent > 0.0f);
		assert(Pos[0] < 1.0f && Pos[0] >= 0.0f);
		assert(Pos[1] < 1.0f && Pos[1] >= 0.0f);
	}

	LightSample(RNG &Rng)
	{
		ShapeComponent = Rng.GetRandomFloat();
		Pos[0] = Rng.GetRandomFloat();
		Pos[1] = Rng.GetRandomFloat();
	}
	float ShapeComponent;
	float Pos[2];
};

template<typename IntersectionType>
class RTLight
{
public:
	virtual bool IsDeltaLight() = 0;
	virtual Spectrum L(const IntersectionType *Intersection, const BWVector3D &PInLight, const BWVector3D &NInLight) = 0;
	virtual Spectrum Sample_L(const IntersectionType *Intersection , const LightSample &InLightSample, BWVector3D &LightDir , float &Pdf, VisibleTester &VisibleTest) = 0;
	virtual float Pdf(const BWVector3D &P, const BWVector3D &Wi) = 0;
	virtual Spectrum Power() = 0;
};

