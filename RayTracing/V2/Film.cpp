#include "Film.hpp"
#include "Math\MatrixMath.hpp"
BlackWalnut::Matrix3X3f BlackWalnut::PixelSensor::SolveXYZFromSensorRGB(BaseSpectrum &SensorIlluminant, BaseSpectrum &OutputIlluminant)
{
	float RGBCamera[24][3], XYZOutput[24][3];
	for (int i = 0; i < SwatchReflectances.size(); i++)
	{
		Vector3f rgb = ProjectReflectance<Vector3f>(SwatchReflectances[i], SensorIlluminant, R_Bar, G_Bar, B_Bar);
		for (int c = 0; c < 3; c++)
		{
			RGBCamera[i][c] = rgb[i];
		}
	}
	float SensorWhiteG = InnerProduct(SensorIlluminant, G_Bar);
	float SensorWhiteY = InnerProduct(SensorIlluminant, Y());
	for (int i = 0; i < SwatchReflectances.size(); i++)
	{
		BaseSpectrum &S = SwatchReflectances[i];
		XYZ xyz = ProjectReflectance<XYZ>(S, OutputIlluminant, X(), Y(), Z()) * (SensorWhiteY / SensorWhiteG);
		for (int c = 0; c < 3; c++)
		{
			XYZOutput[i][c] = xyz[c];
		}
	}
	return LinearLeastSquares(RGBCamera, XYZOutput, SwatchReflectances.size());
}

