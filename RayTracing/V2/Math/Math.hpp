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
		return (T)(A +  (B - A) * Alpha);
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
	template <typename T>
	inline constexpr bool IsPowerOf2(T v) {
		return v && !(v & (v - 1));
	}

	// http://www.plunk.org/~hatch/rightway.php
	inline float SinXOverX(float x)
	{
		if (1 + x * x == 1)
			return 1;
		return std::sin(x) / x;
	}

	inline float Sinc(float x) {
		return SinXOverX(3.14159265358979323846f * x);
	}

	inline float WindowedSinc(float x, float radius, float tau) {
		if (std::abs(x) > radius)
			return 0;
		return Sinc(x) * Sinc(x / tau);
	}
	
	inline int32_t RoundUpPow2(int32_t v) 
	{
		v--;
		v |= v >> 1;
		v |= v >> 2;
		v |= v >> 4;
		v |= v >> 8;
		v |= v >> 16;
		return v + 1;
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

	inline float Bilerp(std::vector<float> p, std::vector<float> v) 
	{
		return ((1 - p[0]) * (1 - p[1]) * v[0] + p[0] * (1 - p[1]) * v[1] +
			(1 - p[0]) * p[1] * v[2] + p[0] * p[1] * v[3]);
	}

	template<typename T>
	inline T Log2Int(T a)
	{
		return std::log(a) / std::log(2);
	}
	inline float Log2(float Data)
	{
		return std::log(Data) / std::log(2);
	}
//	inline uint32_t FloatToBits(float f) 
//	{
//		return std::bit_cast<uint32_t>(f);
//	}
//	inline int Exponent(float v) 
//	{
//		return (FloatToBits(v) >> 23) - 127;
//	}
//	
//	inline int Log2Int(float v) {
//		CHECK_GT(v, 0);
//		if (v < 1)
//			return -Log2Int(1 / v);
//		// https://graphics.stanford.edu/~seander/bithacks.html#IntegerLog
//		// (With an additional check of the significant to get round-to-nearest
//		// rather than round down.)
//		// midsignif = Significand(std::pow(2., 1.5))
//		// i.e. grab the significand of a value halfway between two exponents,
//		// in log space.
//		const uint32_t midsignif = 0b00000000001101010000010011110011;
//		return Exponent(v) + ((Significand(v) >= midsignif) ? 1 : 0);
//	}
//
//	PBRT_CPU_GPU
//		inline int Log2Int(double v) {
//		DCHECK_GT(v, 0);
//		if (v < 1)
//			return -Log2Int(1 / v);
//		// https://graphics.stanford.edu/~seander/bithacks.html#IntegerLog
//		// (With an additional check of the significant to get round-to-nearest
//		// rather than round down.)
//		// midsignif = Significand(std::pow(2., 1.5))
//		// i.e. grab the significand of a value halfway between two exponents,
//		// in log space.
//		const uint64_t midsignif = 0b110101000001001111001100110011111110011101111001101;
//		return Exponent(v) + ((Significand(v) >= midsignif) ? 1 : 0);
//	}
//
//	PBRT_CPU_GPU
//		inline int Log2Int(uint32_t v) {
//#ifdef PBRT_IS_GPU_CODE
//		return 31 - __clz(v);
//#elif defined(PBRT_HAS_INTRIN_H)
//		unsigned long lz = 0;
//		if (_BitScanReverse(&lz, v))
//			return lz;
//		return 0;
//#else
//		return 31 - __builtin_clz(v);
//#endif
//	}
//
//	PBRT_CPU_GPU
//		inline int Log2Int(int32_t v) {
//		return Log2Int((uint32_t)v);
//	}
//
//	PBRT_CPU_GPU
//		inline int Log2Int(uint64_t v) {
//#ifdef PBRT_IS_GPU_CODE
//		return 64 - __clzll(v);
//#elif defined(PBRT_HAS_INTRIN_H)
//		unsigned long lz = 0;
//#if defined(_WIN64)
//		_BitScanReverse64(&lz, v);
//#else
//		if (_BitScanReverse(&lz, v >> 32))
//			lz += 32;
//		else
//			_BitScanReverse(&lz, v & 0xffffffff);
//#endif  // _WIN64
//		return lz;
//#else   // PBRT_HAS_INTRIN_H
//		return 63 - __builtin_clzll(v);
//#endif
//	}
//
//	PBRT_CPU_GPU
//		inline int Log2Int(int64_t v) {
//		return Log2Int((uint64_t)v);
//	}
}