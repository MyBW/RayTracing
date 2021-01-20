#pragma once

namespace BlackWalnut
{
	struct CameraBaseParameters
	{
		Matrix4X4f CameraTransform;
		float ShutterOpen = 0;
		float ShutterClose = 1;
		FilmBase* Film;
	};
	struct CameraRayDifferential 
	{
		RayDifferential Ray;
		SampledSpectrum Weight = SampledSpectrum(1);
	};
	class CameraBase
	{
	public:
		CameraBase(const CameraBaseParameters& Param):Film(Param.Film),CameraTransform(Param.CameraTransform),ShutterClose(Param.ShutterClose),ShutterOpen(Param.ShutterOpen)
		{

		}
		FilmBase* GetFilm() { return Film; }
		const Matrix4X4f& GetCameraTransform() { return CameraTransform; }
		float SampleTime(float U) { return Lerp(ShutterOpen, ShutterClose, U); }
		void ApproximatedPdxy(const SurfaceInteraction& Interaction) const;
	protected:
		FilmBase* Film;
		Matrix4X4f CameraTransform;
		float ShutterOpen, ShutterClose;
		Vector3f MinPosDifferentialX, MinPosDifferentialY;
		Vector3f MinDirDifferentialX, MinDirDifferentialY;
		static CameraRayDifferential GenerateRayDifferential(CameraBase& Camera, const CameraSample& InCameraSample, SampledWavelengths &Lambd);
		void FindMinimumDifferentials(CameraBase& Camera);
	};

	class ProjectiveCamera : public CameraBase
	{
	public:
		ProjectiveCamera() = default;
		ProjectiveCamera(const CameraBaseParameters& Param, const Matrix4X4f &ScreenFromCamera, const Bounds2f& ScreenWindows, float LensRadius, float FocalDistance)
			:CameraBase(Param), ScreenFromCamera(ScreenFromCamera), LensRadius(LensRadius), FocalDistance(FocalDistance)
		{
			
		}
		Matrix4X4f ScreenFromCamera, CameraFromRaster;
		Matrix4X4f RasterFromScreen, ScreenFromRaster;
		float LensRadius;
		float FocalDistance;
	};
	class PerspectiveCamera : public ProjectiveCamera
	{
	public:
		PerspectiveCamera(const CameraBaseParameters& Param, float FOV, const Bounds2f& ScreenWindow, float LensRadius, float FocalDistance) :
			ProjectiveCamera(Param, , ScreenWindow, LensRadius, FocalDistance)
		{
			DxCamera = Matrix4X4MultVector3(CameraFromRaster, Vector3f(1.0, 0.0, 0.0)) - Matrix4X4MultVector3(CameraFromRaster, Vector3f(0.0, 0.0, 0.0));
			DyCamera = Matrix4X4MultVector3(CameraFromRaster, Vector3f(0.0, 1.0, 0.0)) - Matrix4X4MultVector3(CameraFromRaster, Vector3f(0.0, 0.0, 0.0));
			Vector2f Radius = Film->GetFilterBase()->Radius();
			Vector3f Center(Radius.X, Radius.Y, 0.0f);
			Vector3f wCornerCamera = Normalize(Matrix4X4MultVector3(CameraFromRaster, Center));
			CosTotalWidth = wCornerCamera.Z;
			CHECK(.9999 * cosTotalWidth < std::cos(Radians(FOV / 2)));
			Vector2f Res = Film->GetFullResolution();
			Vector3f pMin = Matrix4X4MultVector3(CameraFromRaster, Vector3f(0.0, 0.0, 0.0));
			Vector3f pMax = Matrix4X4MultVector3(CameraFromRaster, Vector3f(Res.X, Res.Y, 0.0));
			pMin /= pMin.Z;
			pMax /= pMax.Z;
			A = std::abs((pMax.X - pMin.X) * (pMax.Y - pMin.Y));
			FindMinimumDifferentials(*this);
		}
	private:
		Vector3f DxCamera, DyCamera;
		float CosTotalWidth;
		float A;
	};
}