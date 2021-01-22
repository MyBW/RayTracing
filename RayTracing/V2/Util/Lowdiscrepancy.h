#pragma once
#include <stdint.h>
#include "Check.h"
#include "Bits.hpp"
#include "..\V2\Math\Math.hpp"
#include "Primes.hpp"
#include "float.hpp"
#include <algorithm>
namespace BlackWalnut
{
	class DigitPermutation
	{
	public:
		DigitPermutation() = default;
		DigitPermutation(int base, uint32_t seed) : base(base) 
		{
			CHECK(base < 65536);  // uint16_t
									// Compute number of digits needed for _base_
			nDigits = 0;
			float invBase = (float)1 / (float)base, invBaseN = 1;
			while (1 - invBaseN < 1) {
				++nDigits;
				invBaseN *= invBase;
			}

			permutations = new uint16_t[nDigits * base];
			for (int digitIndex = 0; digitIndex < nDigits; ++digitIndex) {
				// Compute random permutation for _digitIndex_
				uint32_t digitSeed = MixBits(((base << 8) + digitIndex) ^ seed);
				for (int digitValue = 0; digitValue < base; ++digitValue) {
					int index = digitIndex * base + digitValue;
					permutations[index] = PermutationElement(digitValue, base, digitSeed);
				}
			}
		}
		int Permute(int digitIndex, int digitValue) const 
		{
			CHECK(digitIndex < nDigits);
			CHECK(digitValue < base);
			return permutations[digitIndex * base + digitValue];
		}
	private:
		// DigitPermutation Private Members
		int base, nDigits;
		uint16_t *permutations;
	};

	inline uint64_t InverseRadicalInverse(uint64_t inverse, int base, int nDigits) {
		uint64_t index = 0;
		for (int i = 0; i < nDigits; ++i) {
			uint64_t digit = inverse % base;
			inverse /= base;
			index = index * base + digit;
		}
		return index;
	}

	inline float ScrambledRadicalInverse(int baseIndex, uint64_t a, const DigitPermutation &perm) 
	{
		int base = Primes[baseIndex];
		float invBase = (float)1 / (float)base, invBaseN = 1;
		uint64_t reversedDigits = 0;
		int digitIndex = 0;
		while (1 - invBaseN < 1) {
			// Permute least significant digit from _a_ and update _reversedDigits_
			uint64_t next = a / base;
			int digitValue = a - next * base;
			reversedDigits = reversedDigits * base + perm.Permute(digitIndex, digitValue);
			invBaseN *= invBase;
			++digitIndex;
			a = next;
		}
		return (std::min)(invBaseN * reversedDigits, OneMinusEpsilon);
	}


	inline float RadicalInverse(int baseIndex, uint64_t a) 
	{
		int base = Primes[baseIndex];
		float invBase = (float)1 / (float)base, invBaseN = 1;
		uint64_t reversedDigits = 0;
		while (a) {
			// Extract least significant digit from _a_ and update _reversedDigits_
			uint64_t next = a / base;
			uint64_t digit = a - next * base;
			reversedDigits = reversedDigits * base + digit;
			invBaseN *= invBase;
			a = next;
		}
		return (std::min)(reversedDigits * invBaseN, OneMinusEpsilon);
	}

	// Low Discrepancy Function Definitions
	inline std::vector<DigitPermutation> *ComputeRadicalInversePermutations(uint32_t seed) 
	{
		std::vector<DigitPermutation> *perms = new std::vector<DigitPermutation>();
		perms->resize(PrimeTableSize);
		for (int i = 0; i < PrimeTableSize; i++)
		{
			(*perms)[i] = DigitPermutation(Primes[i], seed);
		}
		return perms;
	}
}