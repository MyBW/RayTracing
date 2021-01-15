#pragma once 


namespace BlackWalnut
{
	class PiecewiseConstant2D
	{
	public:

	};
	inline float XYZMatchingPDF(float lambda) {
		if (lambda < 360 || lambda > 830)
			return 0;
		return 0.0039398042f / std::sqrt(std::cosh(0.0072f * (lambda - 538)));
	}
	inline float SampleXYZMatching(float u) {
		return 538 - 138.888889f * std::atanh(0.85691062f - 1.82750197f * u);
	}
}
