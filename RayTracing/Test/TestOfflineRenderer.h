#pragma once
#include "../OfflineRenderer/Renderer.h"
#include "../OfflineRenderer/Parallel.h"
class Film;
template<typename SceneType, typename CameraType>
class TestOfflineRenderer : public Renderer<SceneType>
{
public:
	TestOfflineRenderer(const CameraType* Camera);
	void RenderScene(SceneType* Scene) override;
	CameraType* GetCamera() const { return Camera; }
	SceneType* GetScene() const { return Scene; }
	Film* GetFilm() const { return &ScreenFilm; }
private:
	Film ScreenFilm;
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