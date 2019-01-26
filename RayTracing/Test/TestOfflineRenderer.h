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
		const BSDF *Bsdf = nullptr;
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
	TestOfflineRenderer(CameraType* Camera = nullptr, Sampler *MainSampler = nullptr):Render<SceneType>(Camera, MainSampler){ }
	void RenderScene(SceneType* Scene) override;
	void InitSPPMPixel();
	Distribution1D* CreateLightPowerDistribute(SceneType *Scene);

	std::vector<SPPMPixel*>& GetSPPMPixel() { return SPPMPixels; }

	std::vector<SPPMPixel*> SPPMPixels;
	float InitialSearchRadius = 0;
	int IteratorNum = 0;
};



template<typename SceneType>
class TestOfflineRendererTask :public Task
{
public:
	TestOfflineRendererTask(TestOfflineRenderer<SceneType> *Render , int StarPixelIndex, int EndPiexlIndex);
	~TestOfflineRendererTask();
	void Run() override;
private:
	TestOfflineRenderer<SceneType> *Render;
	int StarPixelIndex;
	int EndPiexlIndex;
};

template<typename SceneType>
class GenerateSPPMVisiblePointTask : public Task
{
public:
	GenerateSPPMVisiblePointTask(int x, int y, int InTileSize , std::vector<SPPMPixel*>& InSPPMPixels):TileX(x), TileY(y),TileSize(InTileSize),SPPMPixels(InSPPMPixels) { }
	void Run() override;
private:
	int TileX;
	int TileY;
	int TileSize;
	std::vector<SPPMPixel*> *SPPMPixels;
};

#include "TestOfflineRenderer.inl"