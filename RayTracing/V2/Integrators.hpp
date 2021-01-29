#pragma once
#include "Interaction.hpp"
#include "Util/float.hpp"
#include "Math/Math.hpp"
#include "Util/Spectrum.hpp"
#include "Base/Light.hpp"
#include "Samplers.hpp"
#include "Cameras.hpp"
#include <functional>
#include "../OfflineRenderer/Parallel.h"
#include "Film.hpp"
#include "LightSamplers.hpp"

namespace BlackWalnut
{
	class Integrator
	{
	public:
		virtual ~Integrator()
		{

		}
		virtual void Render() = 0;
		ShapeIntersection Intersect(const Ray &InRay, float TMax = Infinity) const;
		bool IntersectP(const Ray &InRay, float TMax = Infinity) const;
		bool Unoccluded(const Interaction &P0, const Interaction &P1) const
		{
			return  !IntersectP(P0.SpawnRayTo(P1), 1.0f - ShadowEpsilon);
		}
		SampledSpectrum Tr(const Interaction &P0, const Interaction &P1, const SampledWavelengths &Lambd, RNG &Rng) const;
		std::vector<LightBase*> Lights;
		std::vector<LightBase*> InfiniteLights;
	protected:
		Integrator(std::vector<LightBase*> AllLights):Lights(AllLights)
		{
			for (auto Light : AllLights)
			{
				Light->PreProcess(SceneBounds);
				if (Light->Type() == LightType::Infinite)
				{
					InfiniteLights.push_back(Light);
				}
			}
		}
		Bounds3f SceneBounds;
	};

	

	class ImageTileIntegrator : public Integrator
	{
	public:
		ImageTileIntegrator(CameraBase* InCamera, SamplerBase* InSampler, std::vector<LightBase*> AllLights):Integrator(AllLights),Camera(InCamera), SamplerProtoType(InSampler)
		{

		}
		void Render() override;
		virtual void EvaluatePixelSample(const Vector2i &Pixel, int SampleIndex, SamplerBase* InSampler) = 0;
		void ParallelProcess(std::function<void(std::vector<Task*>&)> CreateTask, bool IsUseSingleThread = false)
		{
			std::vector<Task*> Tasks;
			CreateTask(Tasks);
			if (IsUseSingleThread == true)
			{
				for (size_t i = 0; i < Tasks.size(); i++)
				{
					Tasks[i]->Run();
				}
				for (size_t i = 0; i < Tasks.size(); i++)
				{
					delete Tasks[i];
					Tasks[i] = nullptr;
				}
			}
			else
			{
				EnqueueTasks(Tasks);
				WaitTaskListFinish();
				CleanupTaskList();
			}
		}
	protected:
		CameraBase* Camera;
		SamplerBase* SamplerProtoType;
	};
	class ImageTileIntegratorTask : public Task
	{
	public:
		ImageTileIntegratorTask(ImageTileIntegrator* InTileIntegrator, Bounds2i InBound, SamplerBase* Sample, uint32_t WaveStart, uint32_t WaveEnd) 
			:TileIntegrator(InTileIntegrator), Bound(InBound),Sampler(Sampler),WaveStart(WaveStart),WaveEnd(WaveEnd)
		{

		}
		void Run() override;
	private:
		ImageTileIntegrator* TileIntegrator;
		Bounds2i Bound;
		SamplerBase* Sampler;
		uint32_t WaveStart, WaveEnd;
	};

	class RayIntegrator : public ImageTileIntegrator
	{
	public:
		RayIntegrator(CameraBase* InCamera, SamplerBase* InSampler, std::vector<LightBase*> AllLights) :ImageTileIntegrator(InCamera, InSampler, AllLights)
		{

		}
		void EvaluatePixelSample(const Vector2i &Pixel, int SampleIndex, SamplerBase* InSampler) override final;
		virtual SampledSpectrum Li(RayDifferential InRay, SampledWavelengths &Lambd, SamplerBase *Sampler, VisibleSurface *InVisibleSurface) = 0;
	private:
		

	};

	class SimplePathIntegrator : public RayIntegrator
	{
	public:
		SimplePathIntegrator(int32_t MaxDepth, bool SampleLights, bool SampleBSDF, CameraBase *Camer, SamplerBase *Sampler, std::vector<LightBase*> AllLights);
		SampledSpectrum Li(RayDifferential InRay, SampledWavelengths &Lambd, SamplerBase *Sampler, VisibleSurface *InVisibleSurface) override;
	private:
		int MaxDepth;
		bool SampleBSDF;
		bool SampleLights;
		UniformLightSampler LightSampler;
	};
}