#pragma once
#include <stdint.h>
#include "..\V2\Math\Geom.hpp"
#include "Check.h"
namespace BlackWalnut
{
	static constexpr int BlueNoiseResolution = 128;
	static constexpr int NumBlueNoiseTextures = 48;

	extern const uint16_t
		BlueNoiseTextures[NumBlueNoiseTextures][BlueNoiseResolution][BlueNoiseResolution];

	// Returns a sample in [0,1].
	inline float BlueNoise(int textureIndex, Vector2i p) 
	{
		CHECK(textureIndex >= 0 && p.X >= 0 && p.Y >= 0);
		textureIndex %= NumBlueNoiseTextures;
		int x = p.X % BlueNoiseResolution, y = p.Y % BlueNoiseResolution;
		return BlueNoiseTextures[textureIndex][x][y] / 65535.f;
	}
}