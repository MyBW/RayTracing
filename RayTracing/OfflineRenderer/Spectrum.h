#pragma once
#include "RTMath.h"
#include "Error.h"


enum SpectrumType { SPECTRUM_REFLECTANCE, SPECTRUM_ILLUMINANT };
static const int SampledLambdaStart = 400;
static const int SampledLambdaEnd = 700;
static const int SpectralSamples = 30;

static const int CIESamples = 471;
extern const float CIE_X[CIESamples];
extern const float CIE_Y[CIESamples];
extern const float CIE_Z[CIESamples];
extern const float CIE_Lambda[CIESamples];

static const float CIE_Y_integral = 106.856895;
static const int nRGB2SpectSamples = 32;
extern const float RGB2SpectLambda[nRGB2SpectSamples];
extern const float RGBRefl2SpectWhite[nRGB2SpectSamples];
extern const float RGBRefl2SpectCyan[nRGB2SpectSamples];
extern const float RGBRefl2SpectMagenta[nRGB2SpectSamples];
extern const float RGBRefl2SpectYellow[nRGB2SpectSamples];
extern const float RGBRefl2SpectRed[nRGB2SpectSamples];
extern const float RGBRefl2SpectGreen[nRGB2SpectSamples];
extern const float RGBRefl2SpectBlue[nRGB2SpectSamples];
extern const float RGBIllum2SpectWhite[nRGB2SpectSamples];
extern const float RGBIllum2SpectCyan[nRGB2SpectSamples];
extern const float RGBIllum2SpectMagenta[nRGB2SpectSamples];
extern const float RGBIllum2SpectYellow[nRGB2SpectSamples];
extern const float RGBIllum2SpectRed[nRGB2SpectSamples];
extern const float RGBIllum2SpectGreen[nRGB2SpectSamples];
extern const float RGBIllum2SpectBlue[nRGB2SpectSamples];

extern float AverageSpectrumSamples(const float *Lambda, const float *Val, int N, float LambdaStart, float LambdaEnd);

inline void XYZToRGB(const float xyz[3], float rgb[3]) {
	rgb[0] = 3.240479f*xyz[0] - 1.537150f*xyz[1] - 0.498535f*xyz[2];
	rgb[1] = -0.969256f*xyz[0] + 1.875991f*xyz[1] + 0.041556f*xyz[2];
	rgb[2] = 0.055648f*xyz[0] - 0.204043f*xyz[1] + 1.057311f*xyz[2];
}


inline void RGBToXYZ(const float rgb[3], float xyz[3]) {
	xyz[0] = 0.412453f*rgb[0] + 0.357580f*rgb[1] + 0.180423f*rgb[2];
	xyz[1] = 0.212671f*rgb[0] + 0.715160f*rgb[1] + 0.072169f*rgb[2];
	xyz[2] = 0.019334f*rgb[0] + 0.119193f*rgb[1] + 0.950227f*rgb[2];
}





template<int Samples>
class CoefficientSpectrum
{
public:
	CoefficientSpectrum(float v)
	{
		for (auto &i : C)
			i = v;
		Error(HasNaNs());
	}
	CoefficientSpectrum(const CoefficientSpectrum<Samples>& V)
	{
		for (int i = 0; i < Samples; i++)
		{
			C[i] = V.C[i];
		}
	}
	bool HasNaNs() const 
	{
		for (auto &i : C)
		{
			if (isnan(i)) return true;
		}
		return false;
	}
	CoefficientSpectrum operator+(const CoefficientSpectrum &CS) const
	{
		CoefficientSpectrum Rt;
		for (int i = 0; i < Samples ; i++)
		{
			Rt.C[i] = CS.C[i] + C[i];
		}
		Error(Rt.HasNaNs());
		return Rt;
	}
	CoefficientSpectrum operator-(const CoefficientSpectrum &CS) const
	{
		CoefficientSpectrum Rt;
		for (int i = 0; i < Samples; i++)
		{
			Rt.C[i] = CS.C[i] - C[i];
		}
		Error(Rt.HasNaNs());
		return Rt;
	}
	CoefficientSpectrum& operator+=(const CoefficientSpectrum &CS) 
	{
		for (int i = 0 ;i < Samples; i++)
		{
			C[i] += CS.C[i];
		}
		Error(HasNaNs());
		return *this;
	}
	CoefficientSpectrum& operator-=(const CoefficientSpectrum &CS)
	{
		for (int i = 0; i < Samples; i++)
		{
			C[i] -= CS.C[i];
		}
		Error(HasNaNs());
		return this;
	}
	CoefficientSpectrum& operator*(const float a) const
	{
		CoefficientSpectrum ret = *this;
		for (int i = 0; i < Samples; ++i)
			ret.C[i] *= a;
		Error(!ret.HasNaNs());
		return ret;
	}
	CoefficientSpectrum &operator*=(float a) {
		for (int i = 0; i < Samples; ++i)
			C[i] *= a;
		Error(!HasNaNs());
		return *this;
	}
	friend inline CoefficientSpectrum operator*(float a, const CoefficientSpectrum &s) 
	{
		Error(!isnan(a) && !s.HasNaNs());
		return s * a;
	}
	CoefficientSpectrum Clamp(float low = 0, float high = INFINITY) const 
	{
		CoefficientSpectrum ret(Samples);
		for (int i = 0; i < Samples; ++i)
			ret.C[i] = ::Clamp(C[i], low, high);
		Error(!ret.HasNaNs());
		return ret;
	}
protected:
	float C[Samples];
};

class RGBSpectrum :public CoefficientSpectrum<3>
{
public:
	RGBSpectrum(float V = 0.0f) :CoefficientSpectrum(V) { }
	void ToRGB(float *RGB)
	{
		RGB[0] = C[0];
		RGB[1] = C[1];
		RGB[2] = C[2];
	}
	static RGBSpectrum FromRGB(const float rgb[3], SpectrumType type = SPECTRUM_REFLECTANCE)
	{
		RGBSpectrum s;
		s.C[0] = rgb[0];
		s.C[1] = rgb[1];
		s.C[2] = rgb[2];
		Error(!s.HasNaNs());
		return s;
	}
};


class SampledSpectrum : public CoefficientSpectrum<SpectralSamples>
{
public:
	SampledSpectrum(float V = 0.0f):CoefficientSpectrum(V)
	{

	}
	SampledSpectrum(const CoefficientSpectrum<SpectralSamples>&V):CoefficientSpectrum(V)
	{
		
	}
	static void Init()
	{
		for (int i  = 0 ; i < SpectralSamples ; i++)
		{
			float wl0 = Lerp(float(i) / float(SpectralSamples), float(SampledLambdaStart), float(SampledLambdaEnd));
			float wl1 = Lerp(float(i + 1) / float(SpectralSamples), SampledLambdaStart, SampledLambdaEnd);
			X.C[i] = AverageSpectrumSamples(CIE_Lambda, CIE_X, i, wl0, wl1);
			Y.C[i] = AverageSpectrumSamples(CIE_Lambda, CIE_Y, i, wl0, wl1);
			X.C[i] = AverageSpectrumSamples(CIE_Lambda, CIE_Z, i, wl0, wl1);
		}

		// Compute RGB to spectrum functions for _SampledSpectrum_
		for (int i = 0; i < SpectralSamples; ++i) {
			float wl0 = Lerp(float(i) / float(SpectralSamples),
				SampledLambdaStart, SampledLambdaEnd);
			float wl1 = Lerp(float(i + 1) / float(SpectralSamples),
				SampledLambdaStart, SampledLambdaEnd);
			rgbRefl2SpectWhite.C[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBRefl2SpectWhite,
				nRGB2SpectSamples, wl0, wl1);
			rgbRefl2SpectCyan.C[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBRefl2SpectCyan,
				nRGB2SpectSamples, wl0, wl1);
			rgbRefl2SpectMagenta.C[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBRefl2SpectMagenta,
				nRGB2SpectSamples, wl0, wl1);
			rgbRefl2SpectYellow.C[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBRefl2SpectYellow,
				nRGB2SpectSamples, wl0, wl1);
			rgbRefl2SpectRed.C[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBRefl2SpectRed,
				nRGB2SpectSamples, wl0, wl1);
			rgbRefl2SpectGreen.C[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBRefl2SpectGreen,
				nRGB2SpectSamples, wl0, wl1);
			rgbRefl2SpectBlue.C[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBRefl2SpectBlue,
				nRGB2SpectSamples, wl0, wl1);

			rgbIllum2SpectWhite.C[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBIllum2SpectWhite,
				nRGB2SpectSamples, wl0, wl1);
			rgbIllum2SpectCyan.C[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBIllum2SpectCyan,
				nRGB2SpectSamples, wl0, wl1);
			rgbIllum2SpectMagenta.C[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBIllum2SpectMagenta,
				nRGB2SpectSamples, wl0, wl1);
			rgbIllum2SpectYellow.C[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBIllum2SpectYellow,
				nRGB2SpectSamples, wl0, wl1);
			rgbIllum2SpectRed.C[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBIllum2SpectRed,
				nRGB2SpectSamples, wl0, wl1);
			rgbIllum2SpectGreen.C[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBIllum2SpectGreen,
				nRGB2SpectSamples, wl0, wl1);
			rgbIllum2SpectBlue.C[i] = AverageSpectrumSamples(RGB2SpectLambda, RGBIllum2SpectBlue,
				nRGB2SpectSamples, wl0, wl1);
		}
	}

	void ToXYZ(float xyz[3]) const {
		xyz[0] = xyz[1] = xyz[2] = 0.f;
		for (int i = 0; i < SpectralSamples; ++i) {
			xyz[0] += X.C[i] * C[i];
			xyz[1] += Y.C[i] * C[i];
			xyz[2] += Z.C[i] * C[i];
		}
		float scale = float(SampledLambdaEnd - SampledLambdaStart) /
			float(CIE_Y_integral * SpectralSamples);
		xyz[0] *= scale;
		xyz[1] *= scale;
		xyz[2] *= scale;
	}
	float y() const {
		float yy = 0.f;
		for (int i = 0; i < SpectralSamples; ++i)
			yy += Y.C[i] * C[i];
		return yy * float(SampledLambdaEnd - SampledLambdaStart) /
			float(CIE_Y_integral * SpectralSamples);
	}
	void ToRGB(float rgb[3]) const {
		float xyz[3];
		ToXYZ(xyz);
		XYZToRGB(xyz, rgb);
	}
	RGBSpectrum ToRGBSpectrum() const;
	static SampledSpectrum FromRGB(const float rgb[3], SpectrumType type = SPECTRUM_REFLECTANCE);
	static SampledSpectrum FromXYZ(const float xyz[3], SpectrumType type = SPECTRUM_REFLECTANCE) 
	{
		float rgb[3];
		XYZToRGB(xyz, rgb);
		return FromRGB(rgb, type);
	}
private:
	static SampledSpectrum X, Y, Z;
	static SampledSpectrum rgbRefl2SpectWhite, rgbRefl2SpectCyan;
	static SampledSpectrum rgbRefl2SpectMagenta, rgbRefl2SpectYellow;
	static SampledSpectrum rgbRefl2SpectRed, rgbRefl2SpectGreen;
	static SampledSpectrum rgbRefl2SpectBlue;
	static SampledSpectrum rgbIllum2SpectWhite, rgbIllum2SpectCyan;
	static SampledSpectrum rgbIllum2SpectMagenta, rgbIllum2SpectYellow;
	static SampledSpectrum rgbIllum2SpectRed, rgbIllum2SpectGreen;
	static SampledSpectrum rgbIllum2SpectBlue;
};


