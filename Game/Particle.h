#pragma once

#include "GameObj.h"

class Particle :public GameObj
{
public:
	float fGravity;
	float fFlash;
	int iLife;

public:
	Particle(const std::string texture_id, float x, float y);
	virtual int Update() override;
	virtual void Draw() override;
};