#pragma once

#include "GameObj.h"


class Smoke :public GameObj
{
public:

private:
	int iLifeTimer;

public:
	Smoke(const int type, const float x, const float y);
	~Smoke();

	virtual int Update() override;
	virtual void Draw() override;
};