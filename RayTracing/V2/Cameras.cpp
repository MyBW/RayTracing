#include "Cameras.hpp"
namespace BlackWalnut
{
	void CameraBase::FindMinimumDifferentials(CameraBase& Camera)
	{
		MinPosDifferentialX = MinPosDifferentialY = MinDirDifferentialX =
		MinDirDifferentialY = Vector3f(std::numeric_limits<Float>::infinity(), std::numeric_limits<Float>::infinity(), std::numeric_limits<Float>::infinity());

		CameraSample Sample;
		Sample.pLens = Vector2f(0.5, 0.5);
		Sample.time = 0.5f;
		SampledWavelengths Lambd = SampledWavelengths::SampleXYZ(0.5f);


	}
}


