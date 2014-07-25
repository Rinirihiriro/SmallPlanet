#pragma once

#include "Boss.h"

class Boss_Unobtainium :public Boss
{
private:
	float fShootDir;

public:
	Boss_Unobtainium(const D3DXVECTOR3 pos, const int x, const int y);

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

	void CircularShot(const float start_angle, const int num, const float spd);

	void MakeBullet(const float x, const float y, const float dir, const float spd);
	void MakeDropBullet(const float x);
	void MakeBottomBullet(const float x, const bool big = false);
};