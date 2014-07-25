#pragma once

#include "GameObj.h"
#include "GameData.h"

class Player :public GameObj
{
public:
	static const int BASIC_MAX_HEALTH, BASIC_MAX_AIR, BASIC_MAX_CARGO, BASIC_MAX_JETPACK
		, BASIC_DRILL_POWER, BASIC_HEALTH_RECOVERY, BASIC_AIR_RECOVER;

	int iHealth, iHealthMax, iHealthRecovery;
	int iJetFuel, iJetFuelMax;
	int iAir, iAirMax, iAirRecover;

	int iCargo, iCargoMax;
	int iCargoDetail[BLOCK_TYPE_NUM];

	int iStandCounter;
	bool bOnGround;
	bool bInFluid, bTotalyInFluid;
	bool bDrilling;

	int iDrillPower, iDrillLevel;
	int iDrillDamage;

	float fGravity;

	int iDir;
	int iHitCooltime;

	int iAirpackBrokenTimer;

private:
	float fMoveSpeed;
	float fJetPower;


public:
	Player(const float x, const float y);
	~Player();

	virtual int Update() override;
	virtual void Draw() override;

	void Move(int dir);	// 0:left 1:right
	void Jump();
	void Jet();
	int Drill();

	virtual bool operator &(GameObj& other);
};