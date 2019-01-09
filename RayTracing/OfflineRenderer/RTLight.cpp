//#include "RTLight.h"
//#include "Sample.h"
//#include "RNG.h"
//#include <assert.h>
//LightSampleOffset::LightSampleOffset(Sample &InSample, int Num)
//{
//	SampleNums = Num;
//	LightComponentOffset = InSample.Add1D(SampleNums);
//	PosOffset = InSample.Add2D(SampleNums);
//}
//
//LightSample::LightSample(Sample &InSample, LightSampleOffset &SampleOffset, int LightSampleIndex)
//{
//	assert(LightSampleIndex < InSample.N1D[SampleOffset.LightComponentOffset]);
//	assert(LightSampleIndex < InSample.N2D[SampleOffset.PosOffset]);
//	LightComponent = InSample.N1Data[SampleOffset.LightComponentOffset][LightSampleIndex];
//	Pos[0] = InSample.N2Data[SampleOffset.PosOffset][2 * LightSampleIndex];
//	Pos[1] = InSample.N2Data[SampleOffset.PosOffset][2 * LightSampleIndex + 1];
//	assert(LightComponent<1.0f && LightComponent > 0.0f);
//	assert(Pos[0] < 1.0f && Pos[0] >= 0.0f);
//	assert(Pos[1] < 1.0f && Pos[1] >= 0.0f);
//}
//
//LightSample::LightSample(RNG &Rng)
//{
//	LightComponent = Rng.GetRandomFloat();
//	Pos[0] = Rng.GetRandomFloat();
//	Pos[1] = Rng.GetRandomFloat();
//}
