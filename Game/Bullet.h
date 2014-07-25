#pragma once

#include "GameObj.h"

class Bullet :public GameObj
{
public:
	char* sParticle_tx_id;
	int iPower;
	int iLife;
	float fAirLeakRate;
	float fFlash;

public:
	Bullet(const std::string texture_id, const float x, const float y, const float size, const int power, const int life=-1);
	virtual int Update();
	virtual void Draw();
};