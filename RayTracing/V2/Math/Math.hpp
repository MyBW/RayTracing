#pragma once
#include <type_traits>
#include <xtr1common>
#include <stdint.h>
#include <vector>
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
	inline float Radians(float deg) 
	{
		return (3.14159265358979323846f / 180) * deg;
	}
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

	inline int PermutationElement(uint32_t i, uint32_t l, uint32_t p) 
	{
		uint32_t w = l - 1;
		w |= w >> 1;
		w |= w >> 2;
		w |= w >> 4;
		w |= w >> 8;
		w |= w >> 16;
		do {
			i ^= p;
			i *= 0xe170893d;
			i ^= p >> 16;
			i ^= (i & w) >> 4;
			i ^= p >> 8;
			i *= 0x0929eb3f;
			i ^= p >> 23;
			i ^= (i & w) >> 1;
			i *= 1 | p >> 27;
			i *= 0x6935fa69;
			i ^= (i & w) >> 11;
			i *= 0x74dcb303;
			i ^= (i & w) >> 2;
			i *= 0x9e501cc3;
			i ^= (i & w) >> 2;
			i *= 0xc860a3df;
			i &= w;
			i ^= i >> 5;
		} while (i >= l);
		return (i + p) % l;
	}

	template <typename T>
	inline T Mod(T a, T b) {
		T result = a - (a / b) * b;
		return (T)((result < 0) ? result + b : result);
	}

	inline float Bilerp(std::vector<float> p, std::vector<float> v) {
		return ((1 - p[0]) * (1 - p[1]) * v[0] + p[0] * (1 - p[1]) * v[1] +
			(1 - p[0]) * p[1] * v[2] + p[0] * p[1] * v[3]);
	}

}