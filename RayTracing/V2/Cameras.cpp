#include "Cameras.hpp"
namespace BlackWalnut
{

	CameraRayDifferential* CameraBase::GenerateRayDifferential(CameraBase& Camera, const CameraSample& InCameraSample, SampledWavelengths &Lambd)
	{
		return nullptr;
	}

	void CameraBase::FindMinimumDifferentials(CameraBase& Camera)
	{
		MinPosDifferentialX = MinPosDifferentialY = MinDirDifferentialX =
		MinDirDifferentialY = Vector3f(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());

		CameraSample Sample;
		Sample.pLens = Vector2f(0.5, 0.5);
		Sample.time = 0.5f;
		SampledWavelengths Lambd = SampledWavelengths::SampleXYZ(0.5f);

		int32_t n = 512;
		for (int i = 0; i < n; i++)
		{
			Sample.pFilm.X = float(i) / (n - 1) * Film->GetFullResolution().X;
			Sample.pFilm.Y = float(i) / (n - 1) * Film->GetFullResolution().Y;

			CameraRayDifferential* CRD = Camera.GenerateRayDifferential(Camera, Sample, Lambd);
			
			if(!CRD) continue;

			RayDifferential &Ray = CRD->Ray;
			Vector3f DOX = CameraFromRender(Ray.RxOrigin - Ray.O, Ray.Time);
			if (Length(DOX) < Length(MinPosDifferentialX))
			{
				MinPosDifferentialX = DOX;
			}
			Vector3f DOY = CameraFromRender(Ray.RyOrigin - Ray.O, Ray.Time);
			if (Length(DOY) < Length(MinPosDifferentialY))
			{
				MinPosDifferentialY = DOX;
			}

			Normalize(Ray.D);
			Normalize(Ray.RxDirection);
			Normalize(Ray.RyDirection);

			Frame F = Frame::FromZ(Ray.D);
			Vector3f DF = F.ToLocal(Ray.D);
			Vector3f DXF = F.ToLocal(Ray.RxDirection); 
			Normalize(DXF);
			Vector3f DYF = F.ToLocal(Ray.RyDirection);
			Normalize(DYF);

			if (Length(DXF - DF) < Length(MinDirDifferentialX))
			{
				MinDirDifferentialX = DXF - DF;
			}
			if (Length(DYF - DF) < Length(MinDirDifferentialY))
			{
				MinDirDifferentialY = DYF - DF;
			}
		}
	}

	//CameraRay* PerspectiveCamera::GenerateRay(CameraSample Sample, SampledWavelengths &Lambd) const
	//{
	//	Vector3f pFilm = Vector3f(Sample.pFilm.X, Sample.pFilm.Y, 0);
	//	Vector3f pCamera = Matrix4X4MultVector3(CameraFromRaster, pFilm);
	//	Ray mRay = {Vector3f(0.0, 0.0, 0.0), Normalize(Vector3f(pCamera), SampleTime(Sample.time)};
	//	if (LensRadius > 0)
	//	{
	//		Vector2f pLens = LensRadius * SampleUniformDiskConcentric(Sample.pLens);
	//		float ft = FocalDistance / Ray.D.Z;
	//		Vector3f pFocus = mRay(ft);

	//		mRay.O = Vector3f(pLens.X, pLens.Y, 0);
	//		mRay.D = Normalize(pFocus - Ray.O);
	//	}
	//	return CameraRay{RenderFromCamera(mRay)};
	//}

	//CameraRayDifferential* PerspectiveCamera::GenerateRayDifferential(const CameraSample& sample, SampledWavelengths &Lambd) const
	//{
	//	// Compute raster and camera sample positions
	//	Vector3f pFilm = Vector3f(sample.pFilm.X, sample.pFilm.Y, 0);
	//	Vector3f pCamera = Matrix4X4MultVector3(CameraFromRaster, pFilm);
	//	Vector3f dir = Normalize(Vector3f(pCamera.x, pCamera.y, pCamera.z));
	//	RayDifferential ray(Vector3f(0, 0, 0), dir, SampleTime(sample.time));
	//	// Modify ray for depth of field
	//	if (LensRadius > 0) {
	//		// Sample point on lens
	//		Vector2f pLens = LensRadius * SampleUniformDiskConcentric(sample.pLens);

	//		// Compute point on plane of focus
	//		float ft = FocalDistance / ray.D.Z;
	//		Vector3f pFocus = ray(ft);

	//		// Update ray for effect of lens
	//		ray.O = Vector3f(pLens.X, pLens.Y, 0);
	//		ray.D = Normalize(pFocus - ray.O);
	//	}

	//	// Compute offset rays for \use{PerspectiveCamera} ray differentials
	//	if (LensRadius > 0) {
	//		// Compute \use{PerspectiveCamera} ray differentials accounting for lens
	//		// Sample point on lens
	//		Vector2f pLens = LensRadius * SampleUniformDiskConcentric(sample.pLens);

	//		// Compute $x$ ray differential for _PerspectiveCamera_ with lens
	//		Vector3f dx = Normalize(Vector3f(pCamera + DxCamera));
	//		float ft = FocalDistance / dx.z;
	//		Vector3f pFocus = Vector3f(0, 0, 0) + (ft * dx);
	//		ray.RxOrigin = Vector3f(pLens.x, pLens.y, 0);
	//		ray.RxDirection = Normalize(pFocus - ray.rxOrigin);

	//		// Compute $y$ ray differential for _PerspectiveCamera_ with lens
	//		Vector3f dy = Normalize(Vector3f(pCamera + DyCamera));
	//		ft = FocalDistance / dy.z;
	//		pFocus = Vector3f(0, 0, 0) + (ft * dy);
	//		ray.RyOrigin = Vector3f(pLens.x, pLens.y, 0);
	//		ray.RyDirection = Normalize(pFocus - ray.RyOrigin);

	//	}
	//	else {
	//		ray.RxOrigin = ray.RyOrigin = ray.O;
	//		ray.RxDirection = Normalize(Vector3f(pCamera) + DxCamera);
	//		ray.RyDirection = Normalize(Vector3f(pCamera) + DyCamera);
	//	}

	//	ray.HasDifferentials = true;
	//	return CameraRayDifferential{ RenderFromCamera(ray) };
	//}

	//SampledSpectrum PerspectiveCamera::We(const Ray &InRay, SampledWavelengths &Lambd, Vector2f *pRaster /*= nullptr*/) const
	//{

	//}

	//void PerspectiveCamera::PDF_We(const Ray& InRay, float *PdfPos, float *PdfDir /*= nullptr*/) const
	//{

	//}

	//CameraWiSample* PerspectiveCamera::SampleWi(const Interaction& Ref, const Vector2f &Sample, SampledWavelengths &Lambd) const
	//{

	//}

}


