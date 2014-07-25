#include "Boss.h"

Boss::	Boss(const std::string texture_id, const D3DXVECTOR3 pos, const int x, const int y, const int size, const int health)
	:GameObj(texture_id, pos), iX(x), iY(y), iSize(size), iHealth(health), iMaxHealth(health), iHitCoolTime(0)
	, iTimer(0), fFlash(0), iPhase(0)
{
	sObject_tag = "boss";
}

int Boss::Update()
{
	iHitCoolTime = max(0, iHitCoolTime-1);
	if (fFlash>0.125f) fFlash *= 0.75f;
	return GameObj::Update();
}

void Boss::Draw()
{
	GameObj::Draw();
}

void Boss::Damage(const int dmg)
{
	if (iHitCoolTime > 0) return;
	iHealth = max(0, iHealth-dmg);
	fFlash = 1;
}

bool Boss::IsDead()
{
	return iHealth <= 0;
}