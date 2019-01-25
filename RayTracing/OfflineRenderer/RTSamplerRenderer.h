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
	~RTSamplerRendererTask();
	void Run() override;
private:
	RTSamplerRenderer<SceneType> *Render;
	int StarPixelIndex;
	int EndPiexlIndex;
};
#include "RTSamplerRenderer.inl"