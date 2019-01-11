#pragma once
#include "../OfflineRenderer/Renderer.h"
#include "../OfflineRenderer/Parallel.h"
#include "../OfflineRenderer/Film.h"
#include "../OfflineRenderer/Integrator.h"
class Sampler;
template<typename SceneType, typename CameraType>
class TestOfflineRenderer : public Renderer<SceneType>
{
public:
	TestOfflineRenderer(CameraType* Camera = nullptr, Sampler *MainSampler = nullptr);
	void RenderScene(SceneType* Scene) override;
	void SetCamera(CameraType* Camera);
	void SetIntegrator(Integrator<typename SceneType, IntersectionInfo> *InIntergrator);
	void SetSampler(Sampler* InSampler) { MainSampler = InSampler; }
	CameraType* GetCamera() const { return Camera; }
	SceneType* GetScene() const { return Scene; }
	Film<CameraType>* GetFilm() { return &ScreenFilm; }
	Sampler* GetMainSampler() { return MainSampler; }
	Spectrum Transmittance() override { return Spectrum(); }
	Spectrum Li() override { return Spectrum(); }

	Film<CameraType> ScreenFilm;
	SceneType *Scene;
    CameraType *Camera;
	Sampler *MainSampler;
	Sample *OrigSample;
	Integrator<typename SceneType , IntersectionInfo> *RendererIntegrator;
};



template<typename SceneType, typename CameraType>
class TestOfflineRendererTask :public Task
{
public:
	TestOfflineRendererTask(TestOfflineRenderer<SceneType, CameraType> *Render , int StarPixelIndex, int EndPiexlIndex);
	~TestOfflineRendererTask();
	void Run() override;
private:
	TestOfflineRenderer<SceneType, CameraType> *Render;
	int StarPixelIndex;
	int EndPiexlIndex;
};

#include "TestOfflineRenderer.inl"