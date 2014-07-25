#include "Effect.h"
#include "TextureManager.h"

void Effect::init()
{
	sObject_tag = "Effect";
}

Effect::Effect( const std::string texture, const D3DXVECTOR3 pos, const float frame_rate, const int start_frame, const int end_frame )
	:GameObj(texture, pos)
{
	this->fFrame_rate = frame_rate;
	fFrame = (float)start_frame;
	endframe = end_frame;
	if (endframe < 0)
		endframe = TextureManager::GetInstance().GetTexture(sTextureId)->GetFrame()-1;
}

int Effect::Update()
{
	fFrame += fFrame_rate;
	if (fFrame >= TextureManager::GetInstance().GetTexture(sTextureId)->GetFrame()
		|| fFrame>endframe) return STATE_DELETED;
	return STATE_OK;
}