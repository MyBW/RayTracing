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
	int M = 0;
	Spectrum Ld;
	float Radius = 0;
	Spectrum Phi;
	float N = 0;
	Spectrum Tau;
};
struct SPPMPixelList
{
	SPPMPixel *VisiblePixle;
	SPPMPixelList *Next;
};
template<typename SceneType>
class TestOfflineRenderer : public Renderer<SceneType>
{
public:
	TestOfflineRenderer(CameraType* Camera = nullptr, Sampler *MainSampler = nullptr):Renderer<SceneType>(Camera, MainSampler){ }
	void RenderScene(SceneType* Scene) override;
	void InitSPPMPixel();
	Distribution1D* CreateLightPowerDistribute(SceneType *Scene);

	std::vector<SPPMPixel*>& GetSPPMPixel() { return SPPMPixels; }
	void SetLines(std::vector<BWVector3D> &DebugShowLine) { this->DebugShowLine = &DebugShowLine; }
	std::vector<SPPMPixel*> SPPMPixels;
	Distribution1D *LightDistr = nullptr;
	float InitialSearchRadius = 0;
	int IteratorNum = 0;
	int PhotonNumPreItor;
	int MaxTraceDepth;
	std::vector<BWVector3D> *DebugShowLine;
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

template<typename SceneType>
class AddVisiblePointToGridTask : public Task
{
public:
	AddVisiblePointToGridTask(int InSPPMPixelIndex, std::vector<SPPMPixel*>& InSPPMPixels, Bounds3f &InGridBound, std::vector<SPPMPixelList*> &InGrid, int GridResX, int GridResY, int GridResZ, Mutex *InGridMutex)
		: SPPMPixelIndex(InSPPMPixelIndex), SPPMPixels(InSPPMPixels), GridBound(InGridBound), Grid(InGrid), GridMutex(InGridMutex)
	{
		GridRes[0] = GridResX;
		GridRes[1] = GridResY;
		GridRes[2] = GridResZ;
	}
	void Run() override;
private:
	int SPPMPixelIndex;
	std::vector<SPPMPixel*> &SPPMPixels;
	std::vector<SPPMPixelList*> &Grid;
	Bounds3f GridBound;
    int GridRes[3];
	Mutex *GridMutex;
};

template<typename SceneType>
class TracePhotonsTask : public Task
{
public:
	TracePhotonsTask(TestOfflineRenderer<SceneType> *InRender, int InCurrentPhotonIndex, int InPhotonNumInTask, int InPhotonNumPreItor, int InCurItor,
					 Distribution1D *InLightDirstr, std::vector<RTLight*> &InLights, int InMaxTraceDepth,
					Bounds3f &InGridBound, int GridResX, int GridResY, int GridResZ, 
		            std::vector<SPPMPixelList*> &InGrid, std::vector<SPPMPixel*> &InSPPMPixels, Mutex *InGridMutex )
	:Render(InRender), CurrentPhotonIndex(InCurrentPhotonIndex), PhotonNumInTask(InPhotonNumInTask) ,PhotonNumPreItor(InPhotonNumPreItor), CurItor(InCurItor)
	,LightsDistr(InLightDirstr),Lights(InLights),MaxTraceDepth(InMaxTraceDepth),GridBound(InGridBound)
		, Grid(InGrid), SPPMPixels(InSPPMPixels), GridMutex(InGridMutex) 
	{
		GridRes[0] = GridResX;
		GridRes[1] = GridResY;
		GridRes[2] = GridResZ;
	}
	std::vector<BWVector3D> *DebugShowLine = nullptr;
	void Run() override;
private:
	int CurrentPhotonIndex;
	int PhotonNumInTask;
	int PhotonNumPreItor;
	int CurItor;
	Distribution1D *LightsDistr;
	std::vector<RTLight*> &Lights;
	int MaxTraceDepth;
	Bounds3f GridBound;
	int GridRes[3];
	std::vector<SPPMPixelList*> &Grid;
	std::vector<SPPMPixel*> &SPPMPixels;
	Mutex *GridMutex;
	TestOfflineRenderer<SceneType> *Render;
};

template<typename SceneType>
class UpdateVisiblePointValueTask : public Task
{
public:
	UpdateVisiblePointValueTask(int InIndex, std::vector<SPPMPixel*>& InSPPMPixels):Index(InIndex),SPPMPixels(InSPPMPixels){ }
	void Run() override;
private:
	int Index;
	std::vector<SPPMPixel*>& SPPMPixels;
};

#include "TestOfflineRenderer.inl"