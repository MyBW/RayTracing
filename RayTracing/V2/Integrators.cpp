#include "Integrators.hpp"
#include "Math\Math.hpp"
#include "Base\Sampler.hpp"
#include <algorithm>
#include "Util\BlueNoise.hpp"

namespace BlackWalnut
{
	BlackWalnut::ShapeIntersection Integrator::Intersect(const Ray &InRay, float TMax /*= Infinity*/) const
	{
		return{};
	}

	bool Integrator::IntersectP(const Ray &InRay, float TMax /*= Infinity*/) const
	{
		return false;
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
			});
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
		SampledSpectrum L(0.f), beta(1.f);
		bool specularBounce = true;
		int depth = 0;

		while (beta) {
			// Find next _SimplePathIntegrator_ path vertex and accumulate contribution
			// Intersect _ray_ with scene
			ShapeIntersection si = Intersect(ray);

			// Account for infinite lights if ray has no intersection
			if (si.THit = -1.0f) {
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
						Vector3f wi = ls.Wi;
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
		Camera->GetFilm()->AddSample(Pixel, L, lambda, &visibleSurface, cameraSample.weight);
	}

}