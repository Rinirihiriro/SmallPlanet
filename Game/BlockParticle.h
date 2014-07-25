#pragma once

#include "GameObj.h"

class BlockParticle :public GameObj
{
public:
	D3DXVECTOR3* vFollow;
	int iType;
	int iLifeTimer;
	float fGravity;
	bool bOnGround;

public:
	BlockParticle(const int type, const float x, const float y);
	~BlockParticle();

	virtual int Update() override;
	virtual void Draw() override;
	virtual bool operator &(GameObj& other);
};