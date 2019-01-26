#pragma once
class Sampler;
template<typename SceneType>
class RTSamplerRenderer : public Renderer<SceneType>
{
public:
	typedef typename SceneType::CameraType CameraType;
	RTSamplerRenderer(CameraType* Camera = nullptr, Sampler *MainSampler = nullptr) : Renderer<SceneType>(Camera , MainSampler){ }
	void RenderScene(SceneType* Scene) override;
};



template<typename SceneType>
class RTSamplerRendererTask :public Task
{
public:
	RTSamplerRendererTask(RTSamplerRenderer<SceneType> *Render, int StarPixelIndex, int EndPiexlIndex);
	RTSamplerRendererTask(RTSamplerRenderer<SceneType> *Render, const Bounds2i &InScreenBounds);
	~RTSamplerRendererTask();
	void Run() override;
private:
	RTSamplerRenderer<SceneType> *Render;
	Bounds2i ScreenBounds;
	int StarPixelIndex;
	int EndPiexlIndex;
};



#include "RTSamplerRenderer.inl"