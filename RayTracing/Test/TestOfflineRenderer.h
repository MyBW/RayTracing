#pragma once
#include "../OfflineRenderer/Renderer.h"
#include "../OfflineRenderer/Parallel.h"
#include "../OfflineRenderer/Film.h"
template<typename SceneType, typename CameraType>
class TestOfflineRenderer : public Renderer<SceneType>
{
public:
	TestOfflineRenderer(CameraType* Camera = nullptr);
	void RenderScene(SceneType* Scene) override;
	void SetCamera(CameraType* Camera);
	CameraType* GetCamera() const { return Camera; }
	SceneType* GetScene() const { return Scene; }
	Film<CameraType>* GetFilm() { return &ScreenFilm; }
	Spectrum Li() override { return Spectrum(); }
private:
	Film<CameraType> ScreenFilm;
	SceneType *Scene;
    CameraType *Camera;
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