#include "Bullet.h"
#include "JMain.h"
#include "Particle.h"
#include "Random.h"
#include "TextureManager.h"
#include "Scene_Game.h"
#include "SEManager.h"

Bullet::Bullet(const std::string texture_id, const float x, const float y, const float size, const int power, const int life)
	:GameObj(texture_id, x, y), sParticle_tx_id(NULL), iLife(life), iPower(power), fAirLeakRate(0.1f), fFlash(1)
{
	SEManager::GetInstance().PlaySE("shoot", true);
	mRect.width = mRect.height = size;
	sObject_tag = "bullet";
}

int Bullet::Update()
{
	int result = GameObj::Update();
	if (fFlash > 0)
		fFlash -= 0.125f;
	iLife = max(-1, iLife-1);
	if (iLife == 0)
	{
		Scene_Game* scene = (Scene_Game*)JMain::GetInstance().GetScene();
		int loop = (int)sqrtf(mRect.width*mRect.height)/2;
		for (int i=0; sParticle_tx_id && i<loop; i++)
		{
			Particle* particle = new Particle(sParticle_tx_id
				, vPos.x+mRect.width*2*(float)(Random::rand.NextDouble()-0.5)
				, vPos.y+mRect.height*2*(float)(Random::rand.NextDouble()-0.5));
			particle->SetVelocity((float)(Random::rand.NextDouble()-0.5)*3.5f, -(float)Random::rand.NextDouble()*5);
			if (scene->mSpecial == Scene_Game::MODE_LESS_GRAVITY)
				particle->fGravity *= 0.5f;
			else if (scene->mSpecial == Scene_Game::MODE_HARDCORE)
				particle->fGravity *= 1.5f;
			particle->fFrame = (float)(Random::rand.Next()%8)*2;
			scene->AddObject(particle);
		}
		scene->fShake = max(loop/10.f, scene->fShake);
		return STATE_DELETED;
	}
	if (vPos.y <= 0) return STATE_DELETED;

	else return result;
}

void Bullet::Draw()
{
	TextureManager::GetInstance().GetTexture(sTextureId)->Draw(vPos,0,0,(int)fFrame);
	if (fFlash > 0)
		TextureManager::GetInstance().GetTexture(sTextureId)->Draw(vPos,0,0,(int)fFrame+1, fFlash);
}
