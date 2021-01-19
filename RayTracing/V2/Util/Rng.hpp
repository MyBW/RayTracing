#pragma once
#include "float.hpp"
#include "Check.h"
#include "Bits.hpp"
#include <algorithm>
namespace BlackWalnut
{
// Random Number Declarations
#define PCG32_DEFAULT_STATE 0x853c49e6748fea9bULL
#define PCG32_DEFAULT_STREAM 0xda3e39cb94b95bdbULL
#define PCG32_MULT 0x5851f42d4c957f2dULL
	class RNG
	{
	public:
		RNG():State(PCG32_DEFAULT_STATE), Inc(PCG32_DEFAULT_STREAM) { }
		RNG(uint64_t SequenceIndex, uint64_t Seed)
		{
			SetSequence(SequenceIndex, Seed);
		}
		RNG(uint64_t SequenceIndex)
		{
			SetSequence(SequenceIndex);
		}
		inline void SetSequence(uint64_t SequenceIndex, uint64_t Seed);
		inline void SetSequence(uint64_t SequenceIndex)
		{
			SetSequence(SequenceIndex, MixBits(SequenceIndex));
		}
		template<typename T>
		T Uniform();
		template<typename T>
		T Uniform(T b, typename std::enable_if_t<std::is_integral<T>::value> * = nullptr)
		{
			T threshold = (~b + 1u) % b;
			while (true)
			{
				T r = Uniform<T>();
				if (r >= threshold)
					return r % b;
			}

		}
		void Advance(int64_t Idelta)
		{
			uint64_t CurMult = PCG32_MULT, CurPlus = Inc, AccMult = 1u;
			uint64_t AccPlus = 0u, Delta = (uint64_t)Idelta;
			while (Delta > 0) {
				if (Delta & 1) {
					AccMult *= CurMult;
					AccPlus = AccPlus * CurMult + CurPlus;
				}
				CurPlus = (CurMult + 1) * CurPlus;
				CurMult *= CurMult;
				Delta /= 2;
			}
			State = AccMult * State + AccPlus;
		}
		int64_t operator-(const RNG &Other) const
		{
			CHECK(Inc == Other.Inc);
			uint64_t CurMult = PCG32_MULT, CurPlus = Inc, CurState = Other.State;
			uint64_t TheBit = 1u, Distance = 0u;
			while (State != CurState) {
				if ((State & TheBit) != (CurState & TheBit)) {
					CurState = CurState * CurMult + CurPlus;
					Distance |= TheBit;
				}
				CHECK( (State & TheBit) == (CurState & TheBit));
				TheBit <<= 1;
				CurPlus = (CurMult + 1ULL) * CurPlus;
				CurMult *= CurMult;
			}
			return (int64_t)Distance;
		}

	private:
		uint64_t State, Inc;
	};
	template<>
	inline uint32_t RNG::Uniform<uint32_t>()
	{
		uint64_t OldState = State;
		State = OldState * PCG32_MULT + Inc;
		uint32_t XOrShifted = (uint32_t)(((OldState >> 18u) ^ OldState) >> 27u);
		uint32_t ROT = (uint32_t)(OldState >> 59u);
		return (XOrShifted >> ROT) | (XOrShifted << ((~ROT + 1u) & 31));
	}
	template<>
	inline uint64_t RNG::Uniform<uint64_t>()
	{
		uint64_t V0 = Uniform<uint32_t>(), V1 = Uniform<uint32_t>();
		return (V0 << 32) | V1;
	}
	template <>
	inline int32_t RNG::Uniform<int32_t>() {
		// https://stackoverflow.com/a/13208789
		uint32_t v = Uniform<uint32_t>();
		if (v <= (uint32_t)(std::numeric_limits<int32_t>::max)())
			// Safe to type convert directly.
			return int32_t(v);

		CHECK(v >= (uint32_t)(std::numeric_limits<int32_t>::min)());
		return int32_t(v - (std::numeric_limits<int32_t>::min)()) +
			(std::numeric_limits<int32_t>::min)();
	}
	template <>
	inline int64_t RNG::Uniform<int64_t>() {
		// https://stackoverflow.com/a/16408789
		uint64_t v = Uniform<uint64_t>();
		if (v <= (uint64_t)(std::numeric_limits<int64_t>::max)())
			// Safe to type convert directly.
			return int64_t(v);

		CHECK(v >= (uint64_t)(std::numeric_limits<int64_t>::min)());
		return int64_t(v - (std::numeric_limits<int64_t>::min)()) +
			(std::numeric_limits<int64_t>::min)();
	}

	template <>
	inline float RNG::Uniform<float>() {
		//return std::min<float>(OneMinusEpsilon, Uniform<uint32_t>() * 0x1p-32f);
		return std::min<float>(OneMinusEpsilon, Uniform<uint32_t>() * 2.328606437e-10f);
	}

	template <>
	inline double RNG::Uniform<double>() {
		//return std::min<double>(OneMinusEpsilon, Uniform<uint64_t>() * 0x1p-64);
		return std::min<double>(OneMinusEpsilon, Uniform<uint64_t>() * 5.42101086242752217e-20);
	}

	template <typename T>
	inline T RNG::Uniform() {
		return T::unimplemented;
	}
	inline void RNG::SetSequence(uint64_t SequenceIndex, uint64_t Seed)
	{
		State = 0u;
		Inc = (SequenceIndex << 1u) | 1u;
		Uniform<uint32_t>();
		State += Seed;
		Uniform<uint32_t>();
	}
}

