#include"gtest/gtest.h"
#include "..\V2\Samplers.hpp"
using namespace BlackWalnut;
TEST(Sampler, ConsistentValues) {
	constexpr int rootSpp = 4;
	constexpr int spp = rootSpp * rootSpp;
	Vector2i resolution(100, 101);

	std::vector<SamplerBase*> samplers;
	samplers.push_back(new HaltonSampler(spp, resolution));
	/*samplers.push_back(new RandomSampler(spp));
	samplers.push_back(new PaddedSobolSampler(spp, RandomizeStrategy::None));
	samplers.push_back(new PaddedSobolSampler(spp, RandomizeStrategy::CranleyPatterson));
	samplers.push_back(new PaddedSobolSampler(spp, RandomizeStrategy::XOR));
	samplers.push_back(new PaddedSobolSampler(spp, RandomizeStrategy::Owen));
	samplers.push_back(new PMJ02BNSampler(spp));
	samplers.push_back(new StratifiedSampler(rootSpp, rootSpp, true));
	samplers.push_back(new SobolSampler(spp, resolution, RandomizeStrategy::None));
	samplers.push_back(
		new SobolSampler(spp, resolution, RandomizeStrategy::CranleyPatterson));
	samplers.push_back(new SobolSampler(spp, resolution, RandomizeStrategy::XOR));
	samplers.push_back(new SobolSampler(spp, resolution, RandomizeStrategy::Owen));*/

	for (auto &sampler : samplers) {
		std::vector<float> s1d[spp];
		std::vector<Vector2f> s2d[spp];

		for (int s = 0; s < spp; ++s) {
			sampler->StartPixelSample(Vector2i( 1, 5 ), s);
			for (int i = 0; i < 10; ++i) {
				s2d[s].push_back(sampler->Get2D());
				s1d[s].push_back(sampler->Get1D());
			}
		}

		// Go somewhere else and generate some samples, just to make sure
		// things are shaken up.
		sampler->StartPixelSample({ 0, 6 }, 10);
		sampler->Get2D();
		sampler->Get2D();
		sampler->Get1D();

		// Now go back and generate samples again, but enumerate them in a
		// different order to make sure the sampler is doing the right
		// thing.
		for (int s = spp - 1; s >= 0; --s) {
			sampler->StartPixelSample({ 1, 5 }, s);
			for (int i = 0; i < s2d[s].size(); ++i) {
				EXPECT_EQ(s2d[s][i], sampler->Get2D());
				EXPECT_EQ(s1d[s][i], sampler->Get1D());
			}
		}
	}
}