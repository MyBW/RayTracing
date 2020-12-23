#pragma once
#include <vector>
#include <algorithm>
namespace BlackWalnut
{
	const int NSpectrumSamples = 4;
	constexpr float Lambda_min = 360, Lambda_max = 830;
	class SampledSpectrum
	{
	public:
		SampledSpectrum(const std::vector<float>& InValues)
		{
			for (int i =  0 ;i < NSpectrumSamples; i++)
			{
				Values[i] = InValues[i];
			}
		}
		SampledSpectrum operator+(const SampledSpectrum &S) const
		{
			SampledSpectrum Ret = *this;
			return Ret += S;
		}
		SampledSpectrum operator-(const SampledSpectrum &S) const
		{
			SampledSpectrum Ret = *this;
			return Ret -= S;
		}
		SampledSpectrum operator*(const SampledSpectrum &S) const
		{
			SampledSpectrum Ret = *this;
			return Ret *= S;
		}
		SampledSpectrum operator/(const SampledSpectrum &S) const
		{
			SampledSpectrum Ret = *this;
			return Ret /= S;
		}
		SampledSpectrum& operator+=(const SampledSpectrum &S) 
		{
			for (int i = 0; i < NSpectrumSamples; i++)
			{
				Values[i] += S.Values[i];
			}
			return *this;
		}
		SampledSpectrum& operator-=(const SampledSpectrum &S)
		{
			for (int i = 0; i < NSpectrumSamples; i++)
			{
				Values[i] -= S.Values[i];
			}
			return *this;
		}
		SampledSpectrum& operator/=(const SampledSpectrum &S)
		{
			for (int i = 0; i < NSpectrumSamples; i++)
			{
				Values[i] /= S.Values[i];
			}
			return *this;
		}
		SampledSpectrum& operator*=(const SampledSpectrum &S)
		{
			for (int i = 0; i < NSpectrumSamples; i++)
			{
				Values[i] *= S.Values[i];
			}
			return *this;
		}
		bool operator==(const SampledSpectrum& S) const
		{
			for (int i = 0; i < NSpectrumSamples; i++)
			{
				if (Values[i] != S.Values[i]) return false;
			}
			return true;
		}
		bool operator!=(const SampledSpectrum& S) const
		{
			return !(*this == S);
		}

		bool HasNaNs() const
		{
			for (int i = 0; i < NSpectrumSamples; i++)
			{
				if (std::isnan(Values[i])) return true;
			}
			return false;
		}
		float MinComponentValue() const 
		{
			float Min = Values[0];
			for (int i = 1; i < NSpectrumSamples; i++)
			{
				Min = std::min(Min, Values[i]);
			}
			return Min;
		}
		float MaxComponentValue() const
		{
			float Max = Values[0];
			for (int i = 1; i < NSpectrumSamples; i++)
			{
				Max = std::max(Max, Values[i]);
			}
			return Max;
		}
		float Average() const
		{
			float Sum = Values[0];
			for (int i = 1; i < NSpectrumSamples; i++)
			{
				Sum += Values[i];
			}
			return Sum / NSpectrumSamples;
		}
	private:
		std::vector<float> Values;
	};

	class SampledWavelengths
	{
	public:
		SampledWavelengths()
		{
			Lambda.resize(NSpectrumSamples);
			Pdf.resize(NSpectrumSamples);
		}
		bool operator==(const SampledWavelengths& S) const
		{
			for (int i = 0; i < NSpectrumSamples; i++)
			{
				if (Lambda[i] != S.Lambda[i]) return false;
				if (Pdf[i] != S.Pdf[i]) return false;
			}
			return true;
		}
		bool operator!=(const SampledWavelengths &S) const
		{
			return !(*this == S);
		}
		float operator[](int i) const
		{
			return Lambda[i];
		}
		float operator[](int i)
		{
			return Lambda[i];
		}
		SampledSpectrum PDF() const
		{
			return SampledSpectrum(Pdf);
		}
		static SampledWavelengths SampleUniform(float U, float Lambda_Min = Lambda_min, float Lambda_Max = Lambda_max)
		{
			SampledWavelengths WaveLength;
			WaveLength.Lambda[0] = Lerp(Lambda_Min, Lambda_Max, U);
			float Delta = (Lambda_Max - Lambda_Min) / NSpectrumSamples;
			for (int i = 1; i < NSpectrumSamples; i++)
			{
				WaveLength.Lambda[i] = WaveLength.Lambda[i] + Delta;
				if (WaveLength.Lambda[i] > Lambda_Max)
				{
					WaveLength.Lambda[i] = Lambda_Min + (WaveLength.Lambda[i] - Lambda_Max);
				}
			}
			for (int i = 0; i < NSpectrumSamples; i++)
			{
				WaveLength.Pdf[i] = 1.0f / NSpectrumSamples;
			}
			return WaveLength;
		}
	private:
		std::vector<float> Lambda;
		std::vector<float> Pdf;
	};
}