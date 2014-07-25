#include "GameObj.h"
#include "Essential.h"

#include "TextureManager.h"

bool GameObj::CompairZ::operator ()(const GameObj* a, const GameObj* b)
{
	return a->z > b->z;
}

void GameObj::init()
{
// 	fFriction = 0;
	bErase = false;

	fAlpha = 1;
	vScale = D3DXVECTOR3(1, 1, 0);
	fRotation = 0;

	fFrame = 0;
	fFrame_rate = 0;

	bTransform = false;

	sObject_tag = "Object";

	vVelocity = D3DXVECTOR3(0, 0, 0);
	fSpeed = 0;
	fDirection = 0;
}

GameObj::GameObj( const std::string texture, const D3DXVECTOR3 pos, const int z )
	:sTextureId(texture), vPos(pos), z(z)
{
	init();
}

GameObj::GameObj( const std::string texture, const float x, const float y, const int z )
	:sTextureId(texture), vPos(x, y, 0), z(z)
{
	init();
}

GameObj::~GameObj()
{
}


void GameObj::Draw()
{
	if (sTextureId=="null") return;
	D3DXVECTOR3 v = vPos;
	v.x = floor(v.x);
	v.y = float(v.y);

	if (!bTransform)
		TextureManager::GetInstance().GetTexture(sTextureId)
		->Draw(v, NULL, NULL, (unsigned int)fFrame, fAlpha);
	else
		TextureManager::GetInstance().GetTexture(sTextureId)
		->Draw(v, &vScale, &fRotation, (unsigned int)fFrame, fAlpha);
}

int GameObj::Update()
{
	vPos+=vVelocity;
	if (mRect) mRect.center = vPos;
	if (fFrame_rate!=0) UpdateFrame();
	UpdateVelocity();
	return bErase? STATE_DELETED:STATE_OK;
}

void GameObj::UpdateFrame()
{
	if (sTextureId=="null") return;
	unsigned texture_frame = TextureManager::GetInstance().GetTexture(sTextureId)->GetFrame();
	fFrame += fFrame_rate;
	while (fFrame >= (float)texture_frame) fFrame -= (float)texture_frame;
	while (fFrame < 0) fFrame += (float)texture_frame;
}

void GameObj::UpdateVelocity()
{
	if (fSpeed<0)
	{
		fSpeed*=-1;
		fDirection+=D3DXToRadian(180);
	}
// 	fSpeed = max(fSpeed-fFriction, 0);
	speed2velocity();
}

void GameObj::speed2velocity()
{
	vVelocity.x = fSpeed * std::cos(fDirection);
	vVelocity.y = fSpeed * std::sin(fDirection);
}

void GameObj::velocity2speed()
{
	fSpeed = D3DXVec3Length(&vVelocity);
	fDirection = std::atan2(vVelocity.y, vVelocity.x);
}


std::string GameObj::GetObjectTag() const { return sObject_tag; }

void GameObj::SetVelocity( const D3DXVECTOR3 velocity ){ this->vVelocity = velocity; velocity2speed(); }
void GameObj::SetVelocity( const float x, const float y){ SetVelocity(D3DXVECTOR3(x, y, 0)); }
void GameObj::SetSpeed( const float speed ){ this->fSpeed = speed; speed2velocity(); }
void GameObj::SetDirection( const float direction ){ this->fDirection = direction; speed2velocity(); }

void GameObj::AddVelocity( const D3DXVECTOR3 velocity ){ SetVelocity(this->vVelocity+velocity); }
void GameObj::AddVelocity( const float x, const float y ){ AddVelocity(D3DXVECTOR3(x, y, 0)); }
void GameObj::AddSpeed( const float speed ){ SetSpeed(this->fSpeed+speed); }
void GameObj::AddDirection( const float direction ){ SetDirection(this->fDirection+direction); }

D3DXVECTOR3 GameObj::GetVelocity() const { return vVelocity; }
float GameObj::GetSpeed() const { return fSpeed; }
float GameObj::GetDirection() const { return fDirection; }


void GameObj::erase()
{
	bErase = true;
}

bool GameObj::operator &(GameObj& other)
{
	return ((this->mRect)&(other.mRect)) != 0;
}