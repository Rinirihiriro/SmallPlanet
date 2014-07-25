#pragma once

#include "GameObj.h"

class Boss :public GameObj
{
public:
	int iX, iY, iSize;
	int iHealth, iMaxHealth;
	int iHitCoolTime;

protected:
	int iTimer;
	float fFlash;
	int iPhase;

public:
	Boss(const std::string texture_id, const D3DXVECTOR3 pos, const int x, const int y, const int size, const int health);
	virtual int Update() override;
	virtual void Draw() override;

	virtual void Damage(const int dmg);

	virtual bool IsDead();

};