#pragma once
#include "..\V2\Math\Geom.hpp"
#include <vector>

namespace BlackWalnut
{
	struct CameraSample {
		Vector2f pFilm;
		Vector2f pLens;
		float time = 0;
		float weight = 1;
	};
	class SamplerBase
	{
	public:

		virtual void StartPixelSample(const Vector2i &p, int32_t sampleIndex, int32_t dim = 0) = 0;
		virtual float Get1D() = 0;
		virtual Vector2f Get2D() = 0;
		virtual std::vector<SamplerBase*> Clone(int32_t N) = 0;
		virtual uint32_t SamplesPerPixel() const = 0;
	};
}