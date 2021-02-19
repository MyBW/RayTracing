#include "Integrators.hpp"
#include "Math\Math.hpp"
#include "Base\Sampler.hpp"
#include <algorithm>
#include "Util\BlueNoise.hpp"

namespace BlackWalnut
{
	BlackWalnut::ShapeIntersection Integrator::Intersect(const Ray &InRay, float TMax /*= Infinity*/) const
	{
		return IntersectFun(InRay, TMax);
	}

	bool Integrator::IntersectP(const Ray &InRay, float TMax /*= Infinity*/) const
	{
		return IntersectPFun(InRay, TMax);
		/*ShapeIntersection si = Intersect(InRay);
		if (si.THit == -1.0f || si.THit) 
		{
			return false;
		}
		return true;*/
	}



	void ImageTileIntegrator::Render()
	{
		
		Bounds2i PixelBounds = Camera->GetFilm()->GetPixelBounds();
		uint32_t SPP = SamplerProtoType->SamplesPerPixel();
		uint32_t WaveStart = 0, WaveEnd = 1, NextWaveSize = 1;

		const int SplitScreenWidth = 64;
		std::vector<Bounds2i> SplitScreenBounds;
		PixelBounds.SplitBounds(SplitScreenWidth, SplitScreenBounds);
		std::vector<SamplerBase*> Samplers = SamplerProtoType->Clone(SplitScreenBounds.size());

		while (WaveStart < SPP)
		{
			ParallelProcess([&](std::vector<Task*> &Tasks)
			{
				for (size_t i = 0; i < SplitScreenBounds.size(); i++)
				{
					Tasks.push_back(new ImageTileIntegratorTask(this, SplitScreenBounds[i], Samplers[i], WaveStart, WaveEnd));
				}
			}, false);
			WaveStart = WaveEnd;
			WaveEnd = (std::min)(SPP, WaveEnd + NextWaveSize);
			NextWaveSize = (std::min<int>)(2 * NextWaveSize, 64);
			ImageMetadata MetaData;
			MetaData.renderTimeSeconds = 10;
			MetaData.samplesPerPixel = WaveStart;
			Camera->InitMetadata(MetaData);
			Camera->GetFilm()->WriteImage(MetaData, 1.0f / WaveStart);
		}
	}

	void ImageTileIntegratorTask::Run()
	{
		Bound.Itorator([&](int X, int Y)
		{
			for (int SampleIndex = WaveStart; SampleIndex < WaveEnd; SampleIndex++)
			{
				Vector2i Pixel(X, Y);
				Sampler->StartPixelSample(Pixel, SampleIndex);
				TileIntegrator->EvaluatePixelSample(Pixel, SampleIndex, Sampler);
			}
		});
	}

	SimplePathIntegrator::SimplePathIntegrator(int32_t MaxDepth, bool SampleLights, bool SampleBSDF, CameraBase *Camer, SamplerBase *Sampler, std::vector<LightBase*> AllLights)
		:RayIntegrator(Camer, Sampler, AllLights), MaxDepth(MaxDepth), SampleLights(SampleLights), SampleBSDF(SampleBSDF), LightSampler(AllLights)
	{

	}

	SampledSpectrum SimplePathIntegrator::Li(RayDifferential ray, SampledWavelengths &Lambda, SamplerBase *Sampler, VisibleSurface *InVisibleSurface)
	{
		//Test 
		//{
		//	SampledSpectrum L(0.f);
		//	ShapeIntersection si = Intersect(ray);
		//	if (si.THit == -1.0f) 
		//	{
		//		return SampledSpectrum(0);
		//	}
		//	/*L = SampledSpectrum(si.THit / 20.f);
		//	return L;*/

		//	SampledLight SampleLight =
		//		LightSampler.Sample(Sampler->Get1D());
		//	if (SampleLight.Light) {
		//		// Sample point on _sampledLight_ to estimate direct illumination
		//		Vector2f uLight = Sampler->Get2D();
		//		LightLiSample ls = SampleLight.Light->SampleLi(si.Intr, uLight, Lambda);
		//		if (ls && ls.L) 
		//		{
		//			//坐标系是右手坐标系y轴向上 pbrt是左手坐标系y轴向上 但是貌似没有影响  BlackWalnut::frame 这个类可以使用 但是不方便 也就是你必须要知道两个相互垂直的轴才能计算一个坐标系 而且坐标系的原点在000点
		//			// MakeViewMatrix更方便一些并且可以移动坐标系
		//			//目前存在的问题：1.IdealDiffuseBxDF::f 函数中调用了SameHemisphere  这个时候 wo和wi都已经转化到了shading的坐标系中 该坐标系以法线为y轴。但是SameHemisphere函数里
		//			//使用 Z分量来计算 也就是wo和wi在YX平面的同一侧时则返回true 否则返回false  但是放在IdealDiffuseBxDF::f 逻辑上不正确 渲染结果也不正确 
		//			//目前看来 shading坐标系可以使用
		//			// Evaluate BSDF for light and possibly add scattered radiance
		//			Vector3f wi = -ls.Wi; //世界坐标系 光源ls.Wi 指向物体
		//			Vector3f wo = ray.D * -1.0f; // 世界坐标系 ray.D 指向物体
		//			BSDF bsdf = si.Intr.GetBSDF(ray, Lambda, Camera, Sampler);

		//			SampledSpectrum f = bsdf.f(wo, wi) * AbsDot(wi, si.Intr.shading.n);
		//			if (f && Unoccluded(si.Intr, ls.InteractionPoint)) //Unoccluded 该函数有问题 计算遮挡的时候距离太短 这个可能是为什么在DistanceLight::SampleLi的时候需要用到 SceneRadius
		//				L += SafeDiv(f * ls.L, Lambda.PDF() * SampleLight.Pdf * ls.PDF);

		//			//L += SafeDiv(f * ls.L, Lambda.PDF() * SampleLight.Pdf * ls.PDF);
		//			/*	if (!Unoccluded(si.Intr, ls.InteractionPoint))
		//				{
		//					CHECK(false);
		//				}*/
		//			//float dot = Dot(wi, si.Intr.shading.n);
		//			//SampledSpectrum f = bsdf.f(wo, wi) * dot;
		//			////SampledSpectrum f = bsdf.f(wo, wi);
		//			//if(dot > 0)
		//			//	L = SafeDiv(ls.L * f , Lambda.PDF() * SampleLight.Pdf * ls.PDF) ;
		//		}
		//	}
		//	return L;
		//}

		SampledSpectrum L(0.f), beta(1.f);
		bool specularBounce = true;
		int depth = 0;

		while (beta) {
			// Find next _SimplePathIntegrator_ path vertex and accumulate contribution
			// Intersect _ray_ with scene
			ShapeIntersection si = Intersect(ray);

			// Account for infinite lights if ray has no intersection
			if (si.THit == -1.0f) {
				if (!SampleLights || specularBounce)
					for (const auto &light : InfiniteLights)
						L += SafeDiv(beta * light->Le(ray, Lambda), Lambda.PDF());
				break;
			}

			// Account for emsisive surface if light wasn't sampled
			SurfaceInteraction &isect = si.Intr;
			if (!SampleLights || specularBounce)
				L += SafeDiv(beta * isect.Le(ray.D * -1.0f, Lambda), Lambda.PDF());

			// End path if maximum depth reached
			if (depth++ == MaxDepth)
				break;

			// Compute scattering functions and skip over medium boundaries
			BSDF bsdf = isect.GetBSDF(ray, Lambda, Camera, Sampler);
			if (!bsdf) {
				isect.SkipIntersection(&ray, si.THit);
				continue;
			}

			// Sample direct illumination if _sampleLights_ is true
			Vector3f wo = ray.D * -1.0f;
			if (SampleLights) {
				SampledLight SampleLight =
					LightSampler.Sample(Sampler->Get1D());
				if (SampleLight.Light) {
					// Sample point on _sampledLight_ to estimate direct illumination
					Vector2f uLight = Sampler->Get2D();
					LightLiSample ls = SampleLight.Light->SampleLi(isect, uLight, Lambda);
					if (ls && ls.L) {
						// Evaluate BSDF for light and possibly add scattered radiance
						//Vector3f wi = ls.Wi;
						Vector3f wi = -ls.Wi;// my test
						SampledSpectrum f = bsdf.f(wo, wi) * AbsDot(wi, isect.shading.n);
						if (f && Unoccluded(isect, ls.InteractionPoint))
							L += SafeDiv(beta * f * ls.L,
								Lambda.PDF() * SampleLight.Pdf * ls.PDF );
					}
				}
			}

			// Sample outoing direction at intersection to continue path
			if (SampleBSDF) {
				// Sample BSDF for new path direction
				float u = Sampler->Get1D();
				BSDFSample bs = bsdf.Sample_f(wo, u, Sampler->Get2D());
				if (bs.Pdf == 0)
					break;
				beta *= bs.f * AbsDot(bs.Wi, isect.shading.n) / bs.Pdf;
				specularBounce = bs.IsSpecular();
				ray = isect.SpawnRay(bs.Wi);

			}
			else {
				// Uniformly sample sphere or hemisphere to get new path direction
				float pdf;
				Vector3f wi;
				if (bsdf.HasReflection() && bsdf.HasTransmission()) {
					wi = SampleUniformSphere(Sampler->Get2D());
					pdf = UniformSpherePDF();
				}
				else {
					wi = SampleUniformHemisphere(Sampler->Get2D());
					pdf = UniformHemispherePDF();
					if (bsdf.HasReflection() && Dot(wo, isect.n) * Dot(wi, isect.n) < 0)
						wi = wi * -1.0f;
					else if (bsdf.HasTransmission() &&
						Dot(wo, isect.n) * Dot(wi, isect.n) > 0)
						wi = wi * -1.0f;
				}
				beta *= bsdf.f(wo, wi) * AbsDot(wi, isect.shading.n) / pdf;
				specularBounce = false;
				ray = isect.SpawnRay(wi);
			}
			delete bsdf.GetBxDF();

			CHECK_GE(beta.y(Lambda), 0.f);
			CHECK(!IsInf(beta.y(Lambda)));
		}
		return L;
	}

	void RayIntegrator::EvaluatePixelSample(const Vector2i &Pixel, int SampleIndex, SamplerBase* InSampler)
	{
		FilterBase* filter = Camera->GetFilm()->GetFilterBase();
		CameraSample cameraSample = GetCameraSample(*InSampler, Pixel, filter);

		// Sample wavelengths for the ray
		float lu = RadicalInverse(1, SampleIndex) + BlueNoise(47, Pixel);
		if (lu >= 1)
			lu -= 1;
		/*if (Options->disableWavelengthJitter)
			lu = 0.5;*/
		SampledWavelengths lambda = Camera->GetFilm()->SampleWavelengths(lu);

		// Generate camera ray for current sample
		CameraRayDifferential *cameraRay =
			Camera->GenerateRayDifferential(cameraSample, lambda);

		SampledSpectrum L(0.);
		VisibleSurface visibleSurface;
		bool initializeVisibleSurface = Camera->GetFilm()->UsesVisibleSurface();
		// Trace _cameraRay_ if valid
		if (cameraRay) {
			// Double check that the ray's direction is normalized.
			CHECK_GT(Length(cameraRay->Ray.D), .999f);
			CHECK_LT(Length(cameraRay->Ray.D), 1.001f);
			// Scale camera ray differentials based on sampling rate
			float rayDiffScale =
				std::max<float>({.125, 1 / std::sqrt((float)InSampler->SamplesPerPixel())});
			/*if (!Options->disablePixelJitter)
				cameraRay->Ray.ScaleDifferentials(rayDiffScale);*/

			//++nCameraRays;
			// Evaluate radiance along camera ray
			L = Li(cameraRay->Ray, lambda, InSampler, initializeVisibleSurface ? &visibleSurface : nullptr) * cameraRay->Weight;

			// Issue warning if unexpected radiance value is returned
			if (L.HasNaNs()) {
				/*LOG_ERROR("Not-a-number radiance value returned for pixel (%d, "
					"%d), sample %d. Setting to black.",
					pPixel.x, pPixel.y, sampleIndex);*/
				L = SampledSpectrum(0.f);
			}
			else if (IsInf(L.y(lambda))) {
				/*LOG_ERROR("Infinite radiance value returned for pixel (%d, %d), "
					"sample %d. Setting to black.",
					pPixel.x, pPixel.y, sampleIndex);*/
				L = SampledSpectrum(0.f);
			}

			/*if (cameraRay)
				PBRT_DBG(
					"%s\n",
					StringPrintf("Camera sample: %s -> ray %s -> L = %s, visibleSurface %s",
						cameraSample, cameraRay->Ray, L,
						(visibleSurface ? visibleSurface.ToString() : "(none)"))
					.c_str());
			else
				PBRT_DBG("%s\n",
					StringPrintf("Camera sample: %s -> no ray generated", cameraSample)
					.c_str());*/
		}

		// Add camera ray's contribution to image
		


		//lambda = lambda.SampleUniform(InSampler->Get1D());
		/*lambda[0] = Y().MaxValueWavelengths();
		lambda[1] = Y().MaxValueWavelengths();
		lambda[2] = Y().MaxValueWavelengths();
		lambda[3] = Y().MaxValueWavelengths();
		L = L * SampledSpectrum(1.0/683.0f);//683是ImageRatio */
		Camera->GetFilm()->AddSample(Pixel, L, lambda, &visibleSurface, cameraSample.weight);
	}

}