#pragma once
#include "RTRenderer.h"
template<typename ScenenType>
class Renderer
{
public:
	virtual void RenderScene(ScenenType* Scene) = 0;
	virtual Spectrum Li() = 0;
};