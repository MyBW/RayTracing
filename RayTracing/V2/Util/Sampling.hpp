#pragma once 

#include "..\Math\Math.hpp"
#include <cmath>
#include <algorithm>
#include "Rng.hpp"
namespace BlackWalnut
{
	template<typename Iterator>
	class IndexingIterator
	{
	public:
		template<typename Generator>
		IndexingIterator(int Ini, int Inn, const Generator* G):i(Ini),n(Inn) { }
		bool operator==(const Iterator& It) const
		{
			return It.i == i;
		}
		bool operator!=(const Iterator& It) const
		{
			return It.i != i;
		}
		Iterator& operator++()
		{
			i++;
			return (Iterator &)*this;
		}
		Iterator operator++(int) const
		{
			Iterator It = *this;
			++It;
			return It;
		}

	protected:
		int32_t i, n;
	};
	template<typename Generator, typename Iterator>
	class IndexingGenerator
	{
	public:
		IndexingGenerator(int32_t n):n(n) { }
		Iterator begin() const
		{
			return Iterator(0, n, (const Generator*) this);
		}
		Iterator end() const
		{
			return Iterator(n, n, (const Generator*) this);
		}
	protected:
		int32_t n;
	};

	template <typename Iterator>
	class RNGIterator;

	template<typename Generator, typename Iterator>
	class RNGGenerator : public IndexingGenerator<Generator, Iterator>
	{
	public:
		RNGGenerator(int32_t n, uint64_t SequenceIndx = 0, uint64_t Seed = PCG32_DEFAULT_STATE)
			:IndexingGenerator<Generator, Iterator>(n),SequenceIndx(SequenceIndx),Seed(Seed) { }
	protected:
		friend RNGIterator<Iterator>;
		uint64_t Seed;
		uint64_t SequenceIndx;
	};
	template<typename Iterator>
	class RNGIterator : public IndexingIterator<Iterator>
	{
	public:
		template<typename Generator>
		RNGIterator(int32_t i, int32_t n, const RNGGenerator<Generator,Iterator> *G) 
			:IndexingIterator<Iterator>(i, n, G),Rng(G->SequenceIndx)
		{

		}
	protected:
		RNG Rng;
	};

	class Stratified1DItor : public RNGIterator<Stratified1DItor>
	{
	public:
		using RNGIterator<Stratified1DItor>::RNGIterator;
		float operator*() { return (i + Rng.Uniform<float>()) / n; }
	};
	class Stratified1D : public RNGGenerator<Stratified1D, Stratified1DItor>
	{
	public:
		using RNGGenerator<Stratified1D, Stratified1DItor>::RNGGenerator;
	};
	class PiecewiseConstant2D
	{
	public:

	};
	inline float XYZMatchingPDF(float lambda) 
	{
		if (lambda < 360 || lambda > 830)
			return 0;
		return 0.0039398042f / Square(std::cosh(0.0072f * (lambda - 538)));
	}
	inline float SampleXYZMatching(float u) 
	{
		return 538 - 138.888889f * std::atanh(0.85691062f - 1.82750197f * u);
	}
}
