#include "Film.hpp"
#include "Math\MatrixMath.hpp"
namespace BlackWalnut
{
	Matrix3X3f PixelSensor::SolveXYZFromSensorRGB(BaseSpectrum &SensorIlluminant, BaseSpectrum &OutputIlluminant)
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
	void BlackWalnut::RGBFilm::WriteImage(ImageMetadata &Metadata, float SplatScale)
	{
		Image OutImage = GetImage(Metadata, SplatScale);
		OutImage.Write(FileName,Metadata);
	}

	Image RGBFilm::GetImage(ImageMetadata &MetaData, float SplatScale)
	{
		PixelFormat format = WriteFP16 ? PixelFormat::Half : PixelFormat::Float;
		Image OutImage(format, Vector2i(PixelBounds.Diagonal()[0], PixelBounds.Diagonal()[1]), std::vector<std::string>{ "R", "G", "B" });

		PixelBounds.Itorator([&](int X, int Y) 
		{
			Vector2i P(X, Y);
			RGB rgb = GetPixelRGB(P, SplatScale);

			Vector2i pOffset(P.X - PixelBounds.Min[0], P.Y - PixelBounds.Min[1]);
			OutImage.SetChannels(pOffset, std::vector<float>{ rgb[0], rgb[1], rgb[2] });
		});
		MetaData.pixelBounds = PixelBounds;
		MetaData.fullResolution = FullResolution;
		MetaData.colorSpace = ColorSpace;
		return OutImage;
	}

	VisibleSurface::VisibleSurface(const SurfaceInteraction& SI, const Matrix4X4f &InCameraTransform, const SampledSpectrum &InAlbedo, const SampledWavelengths &Lambda)
		:Albedo(InAlbedo)
	{
		Set = true;
		Matrix4X4MultVector3(P, InCameraTransform, SI.P());
		Vector3f Wo;
		Matrix4X4MultVector3(Wo, InCameraTransform, SI.wo);
		Matrix4X4MultVector3(Normal, InCameraTransform, SI.n);
		Normal = FaceForward(Normal, Wo);
		Matrix4X4MultVector3(ShadingNoraml, InCameraTransform, SI.shading.n);
		ShadingNoraml = FaceForward(ShadingNoraml, Wo);
		Time = SI.time;
		Vector3f DpDz;
		Matrix4X4MultVector3(DpDz, InCameraTransform, SI.dpdx);
		DzDx = DpDz.Z;
		Matrix4X4MultVector3(DpDz, InCameraTransform, SI.dpdy);
		DzDy = DpDz.Z;
	}

}
