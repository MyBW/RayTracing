#pragma once
#include <stdint.h>
#include "Math\Geom.hpp"
#include <vector>
#include "Util\Lowdiscrepancy.h"
#include "Base\Sampler.hpp"

namespace BlackWalnut
{
	class FilterBase;
	class HaltonSampler  : public SamplerBase
	{
	public:
		HaltonSampler() = default;
		HaltonSampler(int32_t samplesPerPixel, const Vector2i &fullResolution,  int32_t seed = 0);

		
		static constexpr const char *Name() { return "HaltonSampler"; }
		/*static HaltonSampler *Create(const ParameterDictionary &parameters,
			const Point2i &fullResolution, const FileLoc *loc,
			Allocator alloc);*/

		uint32_t SamplesPerPixel() const override { return samplesPerPixel; }

		
		void StartPixelSample(const Vector2i &p, int32_t sampleIndex, int32_t dim) override
		{
			haltonIndex = 0;
			int sampleStride = baseScales[0] * baseScales[1];
			// Compute Halton sample offset for first sample in pixel _p_
			if (sampleStride > 1) 
			{
				Vector2i pm(Mod(p[0], MaxHaltonResolution), Mod(p[1], MaxHaltonResolution));
				for (int32_t i = 0; i < 2; ++i) {
					uint64_t dimOffset =
						(i == 0) ? InverseRadicalInverse(pm[i], 2, baseExponents[i])
						: InverseRadicalInverse(pm[i], 3, baseExponents[i]);
					haltonIndex +=
						dimOffset * (sampleStride / baseScales[i]) * multInverse[i];
				}
				haltonIndex %= sampleStride;
			}

			haltonIndex += sampleIndex * sampleStride;
			dimension = dim;
		}

		
		float Get1D() 
		{
			if (dimension >= PrimeTableSize)
				dimension = 2;
			int32_t dim = dimension++;
			return ScrambledRadicalInverse(dim, haltonIndex, (*digitPermutations)[dim]);
		}

		Vector2f Get2D() 
		{
			if (dimension == 0) {
				// Return Halton pixel sample
				dimension += 2;
				return{ RadicalInverse(0, haltonIndex >> baseExponents[0]),
					RadicalInverse(1, haltonIndex / baseScales[1]) };

			}
			else {
				// Return non-pixel 2D Halton sample
				if (dimension + 1 >= PrimeTableSize)
					dimension = 2;
				int dim = dimension;
				dimension += 2;
				return{ ScrambledRadicalInverse(dim, haltonIndex, (*digitPermutations)[dim]),
					ScrambledRadicalInverse(dim + 1, haltonIndex,
					(*digitPermutations)[dim + 1]) };
			}
		}

		std::vector<SamplerBase*> Clone(int32_t n) override;

	private:
		// HaltonSampler Private Methods
		static uint64_t multiplicativeInverse(int64_t a, int64_t n) {
			int64_t x, y;
			extendedGCD(a, n, &x, &y);
			return Mod(x, n);
		}

		static void extendedGCD(uint64_t a, uint64_t b, int64_t *x, int64_t *y) {
			if (b == 0) {
				*x = 1;
				*y = 0;
				return;
			}
			int64_t d = a / b, xp, yp;
			extendedGCD(b, a % b, &xp, &yp);
			*x = yp;
			*y = xp - (d * yp);
		}

		// HaltonSampler Private Members
		int32_t samplesPerPixel;
		std::vector<DigitPermutation> *digitPermutations;
		static constexpr int MaxHaltonResolution = 128;
		Vector2i baseScales, baseExponents;
		int32_t multInverse[2];
		int64_t haltonIndex = 0;
		int32_t dimension = 0;
	};


	template <typename Sampler>
	inline CameraSample GetCameraSample(Sampler& sampler, const Vector2i &pPixel,
		FilterBase* filter) {
		FilterSample fs = filter->Sample(sampler.Get2D());
		/*if (GetOptions().disablePixelJitter) {
			fs.p = Point2f(0, 0);
			fs.weight = 1;
		}*/

		CameraSample cs;
		cs.pFilm = Vector2f(pPixel.X, pPixel.Y)  + fs.P + Vector2f(0.5, 0.5);
		cs.time = sampler.Get1D();
		cs.pLens = sampler.Get2D();
		cs.weight = fs.Weight;
		return cs;
	}
}