#pragma once
#include <type_traits>
#include <xtr1common>
namespace BlackWalnut
{
	//const float PI = 3.1415926;
	
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

	float Gaussian(float x, float mu = 0, float sigma = 1);

	template<typename Predicate>
	inline int FindInterval(int i, const Predicate &Pred)
	{
		int Size = i - 2, First = 1;
		while (Size > 0)
		{
			int Half = Size >> 1, Middle = First + Half;
			bool PredResult = Pred(Middle);
			First = PredResult ? Middle + 1 : First;
			Size = PredResult ? Size - (Half + 1) : Half;
		}
		return Clamp(First - 1, 0, i - 2);
	}

	template<typename T>
	bool IsNaN(T V)
	{
		return std::isnan(V);
	}

	template <typename T>
	inline typename std::enable_if_t<std::is_floating_point<T>::value, bool> IsInf(T v)
	{
		return std::isinf(v);
	}
}