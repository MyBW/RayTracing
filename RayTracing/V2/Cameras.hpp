#pragma once
#include "Film.hpp"
#include "Math\Matrix.hpp"

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
		CameraRayDifferential() = default;
		CameraRayDifferential(RayDifferential Ray, SampledSpectrum Weight):Ray(Ray),Weight(Weight) {}
		RayDifferential Ray;
		SampledSpectrum Weight = SampledSpectrum(1);
	};
	struct CameraRay 
	{
		Ray mRay;
		SampledSpectrum Weight = SampledSpectrum(1);
	};

	struct CameraWiSample 
	{
	public:
		CameraWiSample() = default;
		CameraWiSample(const SampledSpectrum &Wi, const Vector3f &wi, float pdf,
				Vector2f pRaster, const Interaction &pRef, const Interaction &pLens)
			: Wi(Wi), wi(wi), pdf(pdf), pRaster(pRaster), pRef(pRef), pLens(pLens) {}

		SampledSpectrum Wi;
		Vector3f wi;
		float pdf;
		Vector2f pRaster;
		Interaction pRef, pLens;
	};
	class CameraBase
	{
	public:
		CameraBase(const CameraBaseParameters& Param):Film(Param.Film),CameraTransform(Param.CameraTransform),ShutterClose(Param.ShutterClose),ShutterOpen(Param.ShutterOpen)
		{

		}
		FilmBase* GetFilm() { return Film; }
		const Matrix4X4f& GetCameraTransform() { return CameraTransform; }
		float SampleTime(float U) const { return Lerp(ShutterOpen, ShutterClose, U); }
		void ApproximatedPdxy(const SurfaceInteraction& Interaction) const
		{

		}
		void InitMetadata(ImageMetadata& MetaData)
		{

		}
		virtual CameraRayDifferential* GenerateRayDifferential(const CameraSample& InCameraSample, SampledWavelengths &Lambd) = 0;
	protected:
		FilmBase* Film;
		Matrix4X4f CameraTransform;
		float ShutterOpen, ShutterClose;
		Vector3f MinPosDifferentialX, MinPosDifferentialY;
		Vector3f MinDirDifferentialX, MinDirDifferentialY;
		static CameraRayDifferential* GenerateRayDifferential(CameraBase& Camera, const CameraSample& InCameraSample, SampledWavelengths &Lambd);
		
		virtual CameraRay*  GenerateRay(CameraSample Sample, SampledWavelengths &Lambd) const = 0;
		void FindMinimumDifferentials(CameraBase& Camera);
		Vector3f CameraFromRender(const Vector3f& V, float Time) const
		{
			return Vector3f();
		}
		Ray RenderFromCamera(const Ray& InRay) const
		{
			return Ray();
		}
	};
	template<typename CameraType>
	class ProjectiveCamera : public CameraBase
	{
	public:
		ProjectiveCamera() = default;
		ProjectiveCamera(const CameraBaseParameters& Param, const Matrix4X4f &ScreenFromCamera, const Bounds2f& ScreenWindows, float LensRadius, float FocalDistance)
			:CameraBase(Param), ScreenFromCamera(ScreenFromCamera), LensRadius(LensRadius), FocalDistance(FocalDistance)
		{

		}
		CameraRayDifferential* GenerateRayDifferential(const CameraSample& InCameraSample, SampledWavelengths &Lambd) override;
		Matrix4X4f ScreenFromCamera, CameraFromRaster;
		Matrix4X4f RasterFromScreen, ScreenFromRaster;
		float LensRadius;
		float FocalDistance;
		Vector3f DxCamera;
		Vector3f DyCamera;

		//Temp
		CameraType* OutCamera;
		void UpdateDxDy()
		{
			std::vector<float> Temp = OutCamera->GetViewportPositionInWorldSpace(0, 0, 0);
			std::vector<float> TempX = OutCamera->GetViewportPositionInWorldSpace(1.0f/ GetFilm()->GetFullResolution().X, 0, 0);
			std::vector<float> TempY = OutCamera->GetViewportPositionInWorldSpace(0, 1.0f/ GetFilm()->GetFullResolution().Y, 0);
			DxCamera = { TempX[0] - Temp[0], TempX[1] - Temp[1] , TempX[2] - Temp[2] };
			DyCamera = { TempY[0] - Temp[0], TempY[1] - Temp[1] , TempY[2] - Temp[2] };
		}
		//End Temp
	protected:
		virtual CameraRay*  GenerateRay(CameraSample Sample, SampledWavelengths &Lambd) const { return nullptr; }
	};

	

	//class PerspectiveCamera : public ProjectiveCamera
	//{
	//public:
	//	PerspectiveCamera(const CameraBaseParameters& Param, float FOV, const Bounds2f& ScreenWindow, float LensRadius, float FocalDistance) :
	//		ProjectiveCamera(Param,,ScreenWindow, LensRadius, FocalDistance)
	//	{
	//		DxCamera = Matrix4X4MultVector3(CameraFromRaster, Vector3f(1.0, 0.0, 0.0)) - Matrix4X4MultVector3(CameraFromRaster, Vector3f(0.0, 0.0, 0.0));
	//		DyCamera = Matrix4X4MultVector3(CameraFromRaster, Vector3f(0.0, 1.0, 0.0)) - Matrix4X4MultVector3(CameraFromRaster, Vector3f(0.0, 0.0, 0.0));
	//		Vector2f Radius = Film->GetFilterBase()->Radius();
	//		Vector3f Center(Radius.X, Radius.Y, 0.0f);
	//		Vector3f wCornerCamera = Normalize(Matrix4X4MultVector3(CameraFromRaster, Center));
	//		CosTotalWidth = wCornerCamera.Z;
	//		CHECK(.9999 * cosTotalWidth < std::cos(Radians(FOV / 2)));
	//		Vector2f Res = Film->GetFullResolution();
	//		Vector3f pMin = Matrix4X4MultVector3(CameraFromRaster, Vector3f(0.0, 0.0, 0.0));
	//		Vector3f pMax = Matrix4X4MultVector3(CameraFromRaster, Vector3f(Res.X, Res.Y, 0.0));
	//		pMin = pMin / pMin.Z;
	//		pMax = pMax / pMax.Z;
	//		A = std::abs((pMax.X - pMin.X) * (pMax.Y - pMin.Y));
	//		FindMinimumDifferentials(*this);
	//	}
	//	CameraRay* GenerateRay(CameraSample Sample, SampledWavelengths &Lambd) const;
	//	CameraRayDifferential* GenerateRayDifferential(const CameraSample& InCameraSample, SampledWavelengths &Lambd) const;
	//	SampledSpectrum We(const Ray &InRay, SampledWavelengths &Lambd, Vector2f *pRaster = nullptr) const;
	//	void PDF_We(const Ray& InRay, float *PdfPos, float *PdfDir = nullptr) const;
	//	CameraWiSample* SampleWi(const Interaction& Ref, const Vector2f &Sample, SampledWavelengths &Lambd) const;
	//private:
	//	Vector3f DxCamera, DyCamera;
	//	float CosTotalWidth;
	//	float A;
	//};

	template<typename CameraType>
	BlackWalnut::CameraRayDifferential* ProjectiveCamera<CameraType>::GenerateRayDifferential(const CameraSample& InCameraSample, SampledWavelengths &Lambd)
	{
		RayDifferential ray(Vector3f(0, 0, 0), Vector3f(0, 0, 0), SampleTime(InCameraSample.time));
		std::vector<float> Pos= OutCamera->GetPosition_ForV2();
		ray.O = { Pos[0],Pos[1], Pos[2] };
		std::vector<float> Temp = OutCamera->GetViewportPositionInWorldSpace(InCameraSample.pFilm.X / GetFilm()->GetFullResolution().X, InCameraSample.pFilm.Y/ GetFilm()->GetFullResolution().Y, 0);
		Vector3f VP = { Temp[0],Temp[1], Temp[2] };
		ray.D = VP - ray.O;
		Normalize(ray.D);


		
		ray.RxOrigin = ray.RyOrigin = ray.O;
		ray.RxDirection = VP + DxCamera;
		Normalize(ray.RxDirection);
		ray.RyDirection = VP + DyCamera;
		Normalize(ray.RyDirection);
		ray.HasDifferentials = true;
		return new CameraRayDifferential{ray,SampledSpectrum(1) };

		//// Compute raster and camera sample positions
		//Vector3f pFilm = Vector3f(InCameraSample.pFilm.X, InCameraSample.pFilm.Y, 0);
		//Vector3f pCamera = cameraFromRaster(pFilm);
		//Vector3f dir = pCamera;
		//Normalize(dir);
		//RayDifferential ray(Vector3f(0, 0, 0), dir, SampleTime(InCameraSample.time));
		//// Modify ray for depth of field
		//if (LensRadius > 0) {
		//	// Sample point on lens
		//	Vector2f pLens = SampleUniformDiskConcentric(InCameraSample.pLens) *  LensRadius;

		//	// Compute point on plane of focus
		//	float ft = FocalDistance / ray.D.Z;
		//	Vector3f pFocus = ray(ft);

		//	// Update ray for effect of lens
		//	ray.O = Vector3f(pLens.X, pLens.Y, 0);
		//	ray.D = pFocus - ray.O;
		//	Normalize(ray.D);
		//}

		//// Compute offset rays for \use{PerspectiveCamera} ray differentials
		//if (LensRadius > 0) {
		//	// Compute \use{PerspectiveCamera} ray differentials accounting for lens
		//	// Sample point on lens
		//	Vector2f pLens = SampleUniformDiskConcentric(InCameraSample.pLens) * LensRadius;

		//	// Compute $x$ ray differential for _PerspectiveCamera_ with lens

		//	Vector3f dx = Vector3f(pCamera + DxCamera);
		//	Normalize(dx);
		//	float ft = FocalDistance / dx.Z;
		//	Vector3f pFocus = Vector3f(0, 0, 0) + (ft * dx);
		//	ray.RxOrigin = Vector3f(pLens.X, pLens.Y, 0);
		//	ray.RxDirection = pFocus - ray.RxOrigin;
		//	Normalize(ray.RxDirection);

		//	// Compute $y$ ray differential for _PerspectiveCamera_ with lens
		//	Vector3f dy = pCamera + DyCamera;
		//	Normalize(dy);
		//	ft = FocalDistance / dy.Z;
		//	pFocus = Vector3f(0, 0, 0) + (ft * dy);
		//	ray.RyOrigin = Vector3f(pLens.X, pLens.Y, 0);
		//	ray.RyDirection = pFocus - ray.RyOrigin;
		//	Normalize(ray.RyDirection);

		//}
		//else {
		//	ray.RxOrigin = ray.RyOrigin = ray.O;
		//	ray.RxDirection = Vector3f(pCamera) + DxCamera;
		//	Normalize(ray.RxDirection);
		//	ray.RyDirection = Vector3f(pCamera) + DyCamera;
		//	Normalize(ray.RyDirection);
		//}

		//ray.HasDifferentials = true;
		//return new CameraRayDifferential{ RenderFromCamera(ray),SampledSpectrum(1) };
	}

}