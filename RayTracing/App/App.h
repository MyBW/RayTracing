#pragma once
class App
{
public:
	virtual void Init(int Width, int Height) = 0;
	virtual void Update() = 0;
	virtual void ProcessKeyboard(unsigned char key, int x, int y) = 0;
	virtual void MouseMove(int x, int y) = 0;
	virtual void MouseStateChange(int button, int state, int x, int y) = 0;
};