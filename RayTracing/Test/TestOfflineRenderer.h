#pragma once
#include "../OfflineRenderer/Renderer.h"
#include "../OfflineRenderer/Parallel.h"
#include "../OfflineRenderer/Film.h"
#include "../OfflineRenderer/Integrator.h"
class Sampler;
struct SPPMPixel 
{
	struct VisibalePoint
	{
		BWVector3D P;
		BWVector3D Wo;
		BSDF *Bsdf = nullptr;
		Spectrum Beta;
	} VP;
	int M;
	Spectrum Ld;
	float Radius = 0;
	float Phi[Spectrum::SampleNum];
	float N;
	Spectrum Tau;
};
template<typename SceneType>
class TestOfflineRenderer : public Renderer<SceneType>
{
public:
	TestOfflineRenderer(CameraType* Camera = nullptr, Sampler *MainSampler = nullptr):Renderer<SceneType>(Camera, MainSampler){ }
	void RenderScene(SceneType* Scene) override;
	void InitSPPMPixel();
	Distribution1D* CreateLightPowerDistribute(SceneType *Scene) { return nullptr; }

	std::vector<SPPMPixel*>& GetSPPMPixel() { return SPPMPixels; }

	std::vector<SPPMPixel*> SPPMPixels;
	float InitialSearchRadius = 0;
	int IteratorNum = 0;
};

template<typename SceneType>
class GenerateSPPMVisiblePointTask : public Task
{
public:
	GenerateSPPMVisiblePointTask( TestOfflineRenderer<SceneType> *InRender, const Bounds2i& InPixelBounds, std::vector<SPPMPixel*>& InSPPMPixels):Render(InRender), PixelBounds(InPixelBounds),SPPMPixels(&InSPPMPixels) { }
	void Run() override;
private:
	Bounds2i PixelBounds;
	std::vector<SPPMPixel*> *SPPMPixels;
	TestOfflineRenderer<SceneType> *Render;
};

#include "TestOfflineRenderer.inl"