#pragma once
#include "../OfflineRenderer/Renderer.h"

template<typename SceneType>
class TestOfflineRenderer : public Renderer<SceneType>
{
public:
	void RenderScene(SceneType* Scene) override;
};
