#pragma once
class Sampler;
template<typename SceneType>
class PathTracingRender : public Renderer<SceneType>
{
public:
	typedef typename SceneType::CameraType CameraType;
	PathTracingRender(CameraType* Camera = nullptr, Sampler *MainSampler = nullptr) : Renderer<SceneType>(Camera, MainSampler) { }
	void RenderScene(SceneType* Scene) override;
};



template<typename SceneType>
class PathTracingRenderTask :public Task
{
public:
	PathTracingRenderTask(PathTracingRender<SceneType> *Render, int StarPixelIndex, int EndPiexlIndex);
	PathTracingRenderTask(PathTracingRender<SceneType> *Render, const Bounds2i &InScreenBounds);
	~PathTracingRenderTask();
	void Run() override;
private:
	PathTracingRender<SceneType> *Render;
	Bounds2i ScreenBounds;
	int StarPixelIndex;
	int EndPiexlIndex;
};



#include "PathTracingRender.inl"
