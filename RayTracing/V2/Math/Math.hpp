#pragma once

namespace BlackWalnut
{
	 #define DELTA			(0.00001f)
	int DivideAndRoundUp(int Dividend, int Divisor);
	float Exp(float Data);
	float Sqrt(float Data);
	float Loge(float Data);
	float Sin(float Data);
	float Cos(float Data);
	float Pow(float A, float B);
	int CeilToInt(float F);
	template< class T, class U >
	 T Lerp(const T& A, const T& B, const U& Alpha)
	{
		return (T)(A + Alpha * (B - A));
	}
	template<typename  T>
	T Square(T Data)
	{
		return Data * Data;
	}
	template<typename  T>
	T Clamp(T Data, T MinL, T MaxL)
	{
		return Data < MaxL ? (Data < MinL ?  MinL: Data)  : MaxL;
	}
	template<typename  T>
	T Abs(T Data)
	{
		return Data > 0 ? Data : -Data;
	}
	template<typename  T>
	T Min(T Data1, T Data2)
	{
		return Data1 < Data2 ? Data1 : Data2;
	}
	template<typename  T>
	T Max(T Data1, T Data2)
	{
		return Data1 > Data2 ? Data1 : Data2;
	}

	float Gaussian(float x, float mu = 0, float sigma = 1) 
	{
		return 1 / std::sqrt(2 * PI * sigma * sigma) *
			Exp(-std::sqrt(x - mu) / (2 * sigma * sigma));
	}
}