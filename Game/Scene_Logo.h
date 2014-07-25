#pragma once

#include "Scene.h"

class Scene_Logo :public Scene
{
private:
	int iTimer;
	float fAlpha;

public:
	Scene_Logo();
	virtual void InputProc();
	virtual void Update();
	virtual void Render();
};