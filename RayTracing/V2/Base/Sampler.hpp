#pragma once

namespace BlackWalnut
{
	struct CameraSample {
		Vector2f pFilm;
		Vector2f pLens;
		float time = 0;
		float weight = 1;
	};
}