#include "Samplers.hpp"

namespace BlackWalnut
{

	HaltonSampler::HaltonSampler(int32_t samplesPerPixel, const Point2i &fullResolution, int32_t seed /*= 0*/)
		: samplesPerPixel(samplesPerPixel) 
	{
		digitPermutations = ComputeRadicalInversePermutations(seed);
		// Find radical inverse base scales and exponents that cover sampling area
		for (int i = 0; i < 2; ++i) {
			int base = (i == 0) ? 2 : 3;
			int scale = 1, exp = 0;
			while (scale < std::min(fullRes[i], MaxHaltonResolution)) {
				scale *= base;
				++exp;
			}
			baseScales[i] = scale;
			baseExponents[i] = exp;
		}

		// Compute multiplicative inverses for _baseScales_
		multInverse[0] = multiplicativeInverse(baseScales[1], baseScales[0]);
		multInverse[1] = multiplicativeInverse(baseScales[0], baseScales[1]);
	}

	std::vector<SamplerBase*> HaltonSampler::Clone(int n)
	{
		std::vector<SamplerBase*> samplers(n);
		HaltonSampler *samplerMem = new HaltonSampler[n];
		for (int i = 0; i < n; ++i) 
		{
			samplerMem[i] = *this;
			samplers[i] = &samplerMem[i];
		}
		return samplers;
	}

}