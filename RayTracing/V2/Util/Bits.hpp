#pragma once

namespace BlackWalnut
{
	// http://zimbry.blogspot.ch/2011/09/better-bit-mixing-improving-on.html
		inline uint64_t MixBits(uint64_t v) {
		v ^= (v >> 31);
		v *= 0x7fb5d329728ea185;
		v ^= (v >> 27);
		v *= 0x81dadef4bc2dd44d;
		v ^= (v >> 33);
		return v;
	}
}