#pragma once
#include "../App/App.h"
#include "../Scene/Object.h"
#include "../RealTimeRenderer/RealTimeRenderer.h"
#include "../Scene/Scene.h"
#include "../Scene/Camera.h"
#include "TestOfflineRenderer.h"
#include "../OfflineRenderer/DirectLightingIntegrator.h"

class AppTest : public App
{
public:
	virtual void Init(int Width, int Height) override;
	virtual void Update() override;
	virtual void ProcessKeyboard(unsigned char key, int x, int y)override;
	virtual void MouseMove(int x, int y) override;
	virtual void MouseStateChange(int button, int state, int x, int y) override;
	void UpdataSceneWithRealTimeRenderer();
	void UpdateSceneWithOfflineRenderer();
	void ShowAllNormal();
	void ShowAllAABB();
private:
	RealTimeRenderer<Object> RTRenderer;
	TestOfflineRenderer<Scene, Camera , DirectLightingIntegrator<Scene, Scene::IntersectionType>> OfflineRenderer;

	Scene Sceen;
	Camera CameraForRender;
	bool IsContorllCamera = false;
	int OldMouseX = 0;
	int OldMouseY = 0;
	bool ShowOfflineRender = false;
};