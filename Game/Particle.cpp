#include "Particle.h"
#include "TextureManager.h"

Particle::Particle(const std::string texture_id, float x, float y)
	:GameObj(texture_id, x, y), fGravity(0.25f), fFlash(1), iLife(80)
{
	sObject_tag = "particle";
}

int Particle::Update()
{
	iLife--;
	fAlpha = min(1, iLife/60.f);
	fFlash = max(0, fFlash-0.125f);
	AddVelocity(0, fGravity);
	if (iLife <= 0) return STATE_DELETED;
	else return GameObj::Update();
}

void Particle::Draw()
{
	TextureManager::GetInstance().GetTexture(sTextureId)->Draw(vPos,0,0,(int)fFrame, fAlpha);
	TextureManager::GetInstance().GetTexture(sTextureId)->Draw(vPos,0,0,(int)fFrame+1, fFlash*fAlpha);
}
