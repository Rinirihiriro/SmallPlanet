#pragma once

#include "Boss.h"

class Bullet;

class Boss_Cookie :public Boss
{
private:
	float fShootDir;
	float fTriConst;
	Bullet* mBulletArr[32];
	int iArrInd;

public:
	Boss_Cookie(const D3DXVECTOR3 pos, const int x, const int y);

	virtual int Update() override;
	virtual void Draw() override;

	virtual void Damage(const int dmg) override;

	virtual bool IsDead() override;

private:
	void Phase1Pattern();
	void Phase2Pattern();
	void Phase3Pattern();
	void Phase4Pattern();
	void Phase5Pattern();
	void Phase6Pattern();
	void Phase7Pattern();
	void Phase8Pattern();
	void Phase9Pattern();

	void CircularShot(const float x, const float y, const float start_angle, const int num, const float spd, const int power = 0);

	Bullet* MakeBullet(const float x, const float y, const float dir, const float spd, const int power = 0);
};