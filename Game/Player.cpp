#include "Player.h"

#include "TextureManager.h"
#include "SEManager.h"
#include "BGMManager.h"

#include "JMain.h"
#include "Scene_Game.h"


#define MAX_SPEED 5.f

const int Player::BASIC_MAX_HEALTH = 8192;
const int Player::BASIC_MAX_AIR = 16384;
const int Player::BASIC_MAX_CARGO = 128;
const int Player::BASIC_MAX_JETPACK = 1024;
const int Player::BASIC_DRILL_POWER = 1;
const int Player::BASIC_HEALTH_RECOVERY = 4;
const int Player::BASIC_AIR_RECOVER = 2;

Player::Player(const float x, const float y)
	:GameObj("player", x, y, 0), fMoveSpeed(1.f), fJetPower(0.5f), fGravity(0.25f)
	, bOnGround(true), iHealth(BASIC_MAX_HEALTH), iHealthMax(iHealth), iJetFuel(BASIC_MAX_JETPACK), iJetFuelMax(iJetFuel)
	, iAir(BASIC_MAX_AIR), iAirMax(iAir), iAirRecover(BASIC_AIR_RECOVER)
	, iCargo(0), iCargoMax(BASIC_MAX_CARGO), iDrillPower(BASIC_DRILL_POWER), iDrillLevel(0), bDrilling(false)
	, iDir(4), iHitCooltime(0), iStandCounter(0)
	, iDrillDamage(0), bInFluid(false), bTotalyInFluid(false)
	, iAirpackBrokenTimer(0)
	, iHealthRecovery(BASIC_HEALTH_RECOVERY)
{
	fFrame_rate = 0.25f;
	sObject_tag = "player";
	mRect.width = 12;
	mRect.height = 30;
	ZeroMemory(iCargoDetail, sizeof(iCargoDetail));
}

Player::~Player()
{
	BGMManager::GetInstance().StopBGM("air_leak");
}


int Player::Update()
{
	static const char* const player_dir_texture[] = {"player_left", "player", "player_right"};

	if (iHealth <= 0) return STATE_OK;
	D3DXVECTOR3 v = GetVelocity();

	if (sTextureId != "ship")
	{
		sTextureId = player_dir_texture[iDir%3];
		if (iDir%3 != 1 && bOnGround && abs(v.x) > 0.03625f)
		{
			if ((int)fFrame%4 == 0)
				SEManager::GetInstance().PlaySE("walk", true);
			sTextureId += "_walk";
		}
	}

	iHitCooltime = max(0, iHitCooltime-1);
	fAlpha = (iHitCooltime>0)? 0.5f:1.f;
	v.x *= 0.75f;
	if (abs(v.x) <= 0.03625f) v.x = 0;

	v.y = min(v.y+fGravity, bInFluid? 20.f:15.f);
	if (bInFluid)
	{
		if (bTotalyInFluid)
			iAir = max(iAir-8, 0);
		v.x = v.x*0.75f;
		if (v.y > 0)
			v.y = v.y*0.75f;
	}
	if (iAirpackBrokenTimer>0)
	{
		iAirpackBrokenTimer--;
		iAir = max(0, iAir-128);
		if (iAirpackBrokenTimer > 0) BGMManager::GetInstance().PlayBGM("air_leak");
		else BGMManager::GetInstance().FadeOutBGMAndStop("air_leak", 500);
	}
	SetVelocity(v);

	if (iAir > 0)
		iHealth = min(iHealth+iHealthRecovery, iHealthMax);
	else 
		iHealth = max(iHealth-16+iHealthRecovery/4, 0);

	iStandCounter = min(iStandCounter+1, 60);
	iAir = min(iAir+iAirRecover, iAirMax);

	bOnGround = bInFluid = bTotalyInFluid = false;

	return GameObj::Update();
}

void Player::Draw()
{
	D3DXVECTOR3 v = vPos;
	v.x = floor(v.x);
	v.y = floor(v.y);

	if (iHealth <= 0) return;
	GameObj::Draw();
	if (bDrilling)
		TextureManager::GetInstance().GetTexture("drill")->Draw(
		v,0,0,iDir+iDrillLevel*9,fAlpha);
}


void Player::Move(int dir)
{
	D3DXVECTOR3 v = GetVelocity();
	switch (dir)
	{
	case 0:
		iDir = 3;
		v.x = max(v.x-fMoveSpeed, -MAX_SPEED);
		break;
	case 1:
		iDir = 5;
		v.x = min(v.x+fMoveSpeed, MAX_SPEED);
		break;
	}
	SetVelocity(v);
	iStandCounter = 0;
}

void Player::Jump()
{
	if (!bOnGround && !bInFluid) return;
	if (bOnGround) SEManager::GetInstance().PlaySE("walk");
	D3DXVECTOR3 v = GetVelocity();
	SetVelocity(v.x, -5.f);
	iStandCounter = 0;
}

void Player::Jet()
{
	if (iJetFuel <= 0 ) return;
	AddVelocity(0, -fJetPower);
	iJetFuel -= 1;
	iStandCounter = 0;
}

int Player::Drill()
{
	iDrillDamage += iDrillPower;
	iStandCounter = 0;
	return iDrillDamage;
}

bool Player::operator &(GameObj& other)
{
	D3DXVECTOR3 coli_v = mRect&other.mRect;
	if (coli_v.x != 0 || coli_v.y != 0)
	{
		D3DXVECTOR3 v = GetVelocity();
		if (coli_v.x != 0)
		{
			if (v.x*coli_v.x <= 0)
			{
				return false;
				//coli_v.x = mRect.width+other.mRect.width-abs(coli_v.x);
				//if (v.x < 0) coli_v.x = -coli_v.x;
			}
			if (iHitCooltime == 0 && abs(v.x) > 10.f)
			{
				iHealth = max(iHealth - (int)abs(v.x)*256, 0);
				iHitCooltime = 120;
				((Scene_Game*)JMain::GetInstance().GetScene())->fShake = abs(v.x)*0.5f;
			}
			SetVelocity(0, v.y);
		}
		else if (coli_v.y != 0)
		{
			if (v.y*coli_v.y <= 0)
			{
				return false;
				//coli_v.y = mRect.height+other.mRect.height-abs(coli_v.y);
				//if (v.y < 0) coli_v.y = -coli_v.y;
			}
			if (v.y > 1.f)
				SEManager::GetInstance().PlaySE("walk");
			else if (v.y < -1.f)
				SEManager::GetInstance().PlaySE("head", true);
			if (iHitCooltime == 0 && abs(v.y) > 10.f)
			{
				iHealth = max(0, iHealth - (int)abs(v.y)*256);
				iHitCooltime = 120;
				((Scene_Game*)JMain::GetInstance().GetScene())->fShake = abs(v.y)*0.5f;
				SEManager::GetInstance().PlaySE("hurt");
			}
			SetVelocity(v.x, 0);
		}
		vPos -= coli_v;
		mRect.center = vPos;
		if (coli_v.y > 0)
			bOnGround = true;
		return true;
	}
	return false;
}
