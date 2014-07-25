#include "Smoke.h"
#include "Random.h"
#include "TextureManager.h"

Smoke::Smoke(const int type, const float x, const float y)
	:GameObj("smoke", x, y, -1)
{
	fFrame = (float)type;
	iLifeTimer = 60;
	SetVelocity((float)Random::rand.NextDouble()*4-2, -0.125f);
	bTransform = true;
	vScale.x = vScale.y = 0;
	sObject_tag = "smoke";
}

Smoke::~Smoke()
{

}


int Smoke::Update()
{
	D3DXVECTOR3 v = GetVelocity();
	v.x *= 0.75f;
	if (v.y>0) v.y -= 0.125f;
	SetVelocity(v);

	if (iLifeTimer >= 50)
		vScale.x = vScale.y = (60-iLifeTimer)/10.f;
	else
		fAlpha = min(iLifeTimer/40.f, 1);
	if (iLifeTimer == 0)
		erase();
	iLifeTimer -= 1;
	return GameObj::Update();
}

void Smoke::Draw()
{
	Texture* tx = TextureManager::GetInstance().GetTexture("smoke");
	tx->color = 0xffdddddd;
	tx->Draw(vPos, &vScale, 0, 0, fAlpha);
}
