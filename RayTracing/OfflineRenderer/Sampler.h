#pragma once
class Sample;
class RNG;
class Sampler
{
public:
	Sampler(int StartPixelIndex, int EndPixelIndex, int SPP, int InFilmWidth, int InFilmHeight):
		StartPixelIndex(StartPixelIndex),EndPixelIndex(EndPixelIndex),SamplesPerPixel(SPP),FilmWidth(InFilmWidth),FilmHeight(InFilmHeight)
	{

	}
	virtual int GetMaxSampleCount() = 0;
	virtual Sampler* GetSubSampler(int SubStartPixelIndex, int SubEndPixelIndex) = 0;
	virtual int GetMoreSamples(Sample *InSample, RNG *InRNG) = 0;
protected:
	int StartPixelIndex;
	int EndPixelIndex;
	int SamplesPerPixel;
	int FilmWidth;
	int FilmHeight;
};

class Random : public Sampler
{
public:
	Random(int StartPixelIndex, int EndPixelIndex, int SPP , int InFilmWidth, int InFilmHeight) :Sampler(StartPixelIndex, EndPixelIndex, SPP, InFilmWidth, InFilmHeight)
		,CurrSamplePos(StartPixelIndex),CurrPixelSampleNum(0)
	{
		ImageSamplePos = new float[SamplesPerPixel * 2];
	}
	~Random()
	{
		delete [] ImageSamplePos;
	}
	int GetMaxSampleCount() override { return SamplesPerPixel; }
	Sampler* GetSubSampler(int SubStartPixelIndex, int SubEndPixelIndex)
	{
		return new Random(SubEndPixelIndex, SubEndPixelIndex, SamplesPerPixel, FilmWidth, FilmHeight);
	}
	int GetMoreSamples(Sample *InSample, RNG *InRNG);
	void ResetSamplePosition(int SamplePos)
	{
		CurrSamplePos = SamplePos;
		CurrSamplePosX = CurrSamplePos % FilmWidth;
		CurrSamplePosY = CurrSamplePos / FilmWidth;
	}
private:
	int CurrSamplePos;
	int CurrSamplePosX;
	int CurrSamplePosY;
	int CurrPixelSampleNum;
	float *ImageSamplePos;
};