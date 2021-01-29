//
//#include"gtest/gtest.h"
//#include "../V2/Base/Textures.hpp"
//#include "RNG.h"
//#include "..\V2/LightSamplers.hpp"
//
//TEST(UniformLightSampling, PdfMethod) {
//	RNG rng(5251);
//	auto r = [&rng]() { return rng.Uniform<float>(); };
//
//	std::vector<LightBase*> lights;
//	std::vector<ShapeHandle> tris;
//	std::tie(lights, tris) = randomLights(20, Allocator());
//
//	UniformLightSampler distrib(lights, Allocator());
//	for (int i = 0; i < 100; ++i) {
//		Vector3f p(-1 + 3 * r(), -1 + 3 * r(), -1 + 3 * r());
//		Interaction intr(Vector3i(p), Vector3f(0, 0, 0), Vector2f(0, 0));
//		pstd::optional<SampledLight> sampledLight =
//			distrib.Sample(intr, rng.Uniform<Float>());
//		ASSERT_TRUE((bool)sampledLight) << i << " - " << p;
//		EXPECT_FLOAT_EQ(sampledLight->pdf, distrib.PDF(intr, sampledLight->light));
//	}
//}