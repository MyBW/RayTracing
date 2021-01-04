#pragma once
#include <vector>
#include <algorithm>
#include "..\Math\Math.hpp"
#include "..\Util\Color.hpp"
namespace BlackWalnut
{
	const int NSpectrumSamples = 4;
	constexpr float Lambda_min = 360, Lambda_max = 830;
	static constexpr float CIE_Y_integral = 106.856895;
	inline float Blackbody(float Lambda, float T)
	{
		if (T <= 0)
			return 0;
		const float C = 299792458;
		const float H = 6.62606957e-34;
		const float KB = 1.3806488e-23;
		float L = Lambda * 1e-9f;
		float Le = (2 * H*C*C) / (Pow(5, 1) * (std::exp(H*C) / (1 * KB*T) - 1));
		std::isnan(Le);
		return Le;
	}

	class SampledSpectrum
	{
	public:
		SampledSpectrum() = default;
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
		float operator[](int i) const
		{
			return Values[i];
		}
		float& operator[](int i)
		{
			return Values[i];
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
		float& operator[](int i)
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


	class BaseSpectrum
	{
	public:
		virtual SampledSpectrum Sample(const SampledWavelengths &Lambda) const = 0;
		virtual float operator()(float) const = 0;
	};

	class DenselySampledSpectrum : public BaseSpectrum
	{
	public:
		DenselySampledSpectrum(int InLambda_Min = Lambda_min, int InLambda_Max = Lambda_max)
			:Lambda_Min(InLambda_Min),Lambda_Max(InLambda_Max)
		{
			Values.resize(Lambda_Max - Lambda_Min + 1);
		}
		DenselySampledSpectrum(BaseSpectrum& S, int InLambda_Min = Lambda_min, int InLambda_Max = Lambda_max)
			:Lambda_Min(InLambda_Min), Lambda_Max(InLambda_Max)
		{
			Values.resize(Lambda_Max - Lambda_Min + 1);
			for (int Lambda = Lambda_Min; Lambda < Lambda_Max; Lambda++)
			{
				Values[Lambda - Lambda_Min] = S(Lambda);
			}
		}
		SampledSpectrum Sample(const SampledWavelengths &Lambda) const override
		{
			SampledSpectrum S;
			for (int i = 0; i < NSpectrumSamples; i++)
			{
				int Offset = std::lround(Lambda[i]) - Lambda_Min;
				if (Offset < 0 || Offset >= Values.size())
					S[i] = 0;
				else
					S[i] = Values[Offset];
			}
			return S;
		}
		template<typename F>
		static DenselySampledSpectrum SampleFunction(F Func, int Lambda_Min = Lambda_min, int Lambda_Max = Lambda_max)
		{
			DenselySampledSpectrum S(Lambda_Min, Lambda_Max);
			for (int Lambda = Lambda_Min; Lambda <= Lambda_Max; Lambda++)
			{
				S.Values[Lambda - Lambda_Min] = Func(Lambda);
			}
			return S;
		}
		float operator()(float Lambda) const override
		{
			int Offset = std::lround(Lambda) - Lambda_Min;
			if (Offset < 0 || Offset >= Values.size())
				return 0;
			return Values[Offset];
		}
	private:
		float Lambda_Min, Lambda_Max;
		std::vector<float> Values;
	};

	class PiecewiseLinearSpectrum : public BaseSpectrum
	{
	public:
		PiecewiseLinearSpectrum() = default;
		PiecewiseLinearSpectrum(std::vector<float> &Lambda, std::vector<float> &Values);
		void Scale(float S)
		{
			for (float &V : Values)
			{
				V *= S;
			}
		}
		float MaxValue()
		{
			float Max = Values[0];
			for (float &f : Values)
			{
				Max = std::max(Max, f);
			}
			return Max;
		}
		float MinValue()
		{
			float Min = Values[0];
			for (float &f : Values)
			{
				Min = std::min(Min, f);
			}
			return Min;
		}
		SampledSpectrum Sample(const SampledWavelengths &Lambda) const
		{
			SampledSpectrum S;
			for (int i = 0;i < NSpectrumSamples; i++)
			{
				S[i] = (*this)(Lambda[i]);
			}
			return S;
		}
		float operator()(float Lambda) const override
		{
			if (Values.empty() || Lambda < Values.front() || Lambda > Values.back())
			{
				return 0;
			}
			int Offset = FindInterval(Values.size(), [&](int Index) {return Values[Index] < Lambda; });
			float T = (Lambda - Values[Offset]) / (Values[Offset + 1] - Values[Offset]);
			return Lerp(Values[Offset], Values[Offset + 1], T);
		}
	private:
		std::vector<float> Values;
		std::vector<float> Lambda;
	};

	class BlackbodySpectrum : BaseSpectrum
	{
	public:
		BlackbodySpectrum(float InT) :T(InT)
		{
			float LambdaMax = float(2.8977721e-3 / T * 1e9);
			NormalizationFactor = 1.0 / Blackbody(LambdaMax, T);
		}
		float operator()(float Lambda) const
		{
			return Blackbody(Lambda, T) * NormalizationFactor;
		}
		SampledSpectrum Sample(const SampledWavelengths& Lambda) const 
		{
			SampledSpectrum S;
			for (int i = 0; i < NSpectrumSamples; i++)
			{
				S[i] = Blackbody(Lambda[i], T) * NormalizationFactor;
			}
			return S;
		}
		float MaxValue() const { return 1.0f; }
	private:
		float T;
		float NormalizationFactor;
	};
	const DenselySampledSpectrum &X();
	const DenselySampledSpectrum &Y();
	const DenselySampledSpectrum &Z();
	XYZ SpectrumToXYZ(BaseSpectrum* S);
	DenselySampledSpectrum D(float Temperature);

	// Spectrum Inline Functions
	inline float InnerProduct(const BaseSpectrum &a, const BaseSpectrum &b) 
	{
		float result = 0;
		for (float lambda = Lambda_min; lambda <= Lambda_max; ++lambda)
			result += a(lambda) * b(lambda);
		return result / CIE_Y_integral;
	}
}