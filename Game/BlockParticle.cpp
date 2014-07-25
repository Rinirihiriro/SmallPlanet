#include "BlockParticle.h"
#include "GameData.h"
#include "SEManager.h"

BlockParticle::BlockParticle(const int type, const float x, const float y)
	:GameObj("particle", x, y, -1), iType(type), vFollow(0), fGravity(0.25f)
{
	sObject_tag = "bparticle";
	fFrame = (float)iType;
	iLifeTimer = gBlockData[iType].collectable? 300:120;
	mRect.width = mRect.height = 4;
}

BlockParticle::~BlockParticle()
{

}

int BlockParticle::Update()
{
	D3DXVECTOR3 v = GetVelocity();
	v.y += fGravity;
	v.x = v.x*0.75f;
	if (vFollow)
		v += ((*vFollow)-vPos)*0.0625f;
	SetVelocity(v);

	iLifeTimer -= 1;
	if (iLifeTimer <= 0)
		erase();

	return GameObj::Update();
}

void BlockParticle::Draw()
{
	GameObj::Draw();
}

bool BlockParticle::operator &(GameObj& other)
{
	D3DXVECTOR3 coli_v = mRect&other.mRect;
	if (coli_v.x != 0 || coli_v.y != 0)
	{
		D3DXVECTOR3 v = GetVelocity();
		if (coli_v.x != 0)
		{
			SetVelocity(-v.x*0.75f, v.y);
		}
		else if (coli_v.y != 0)
		{
			SetVelocity(v.x, -v.y*0.75f);
		}
		vPos -= coli_v;
		mRect.center = vPos;
		return true;
	}
	return false;
}
