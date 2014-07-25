#include "JSprite.h"

#include "Essential.h"

JSprite::JSprite(LPD3DXSPRITE sprite)
	:sprite(NULL)
{
	this->sprite = sprite;
	D3DXMatrixScaling(&finalScale, 1, 1, 1);
}

JSprite::JSprite()
	:sprite(NULL)
{
	D3DXMatrixScaling(&finalScale, 1, 1, 1);
}

JSprite::~JSprite()
{
	SAFE_RELEASE(sprite);
}


void JSprite::ResetSprite(LPD3DXSPRITE sprite)
{
	SAFE_RELEASE(this->sprite);
	this->sprite = sprite;
}


void JSprite::DrawTexture(	LPDIRECT3DTEXTURE9 texture
							, const RECT *rect
							, const D3DXVECTOR3 *center
							, const D3DXVECTOR3 *pos
							, const D3DXVECTOR3 *scale
							, const float *rotation
							, const D3DXCOLOR color)
{
	// 여기부터 변환행렬 만듬
	D3DXMATRIX originalTransformMat, transformMat, centerTransMatI, transMat, scaleMat, rotMat;
	bool transform = center || scale || rotation || pos;

	if (transform)
	{
		sprite->GetTransform(&originalTransformMat);
		D3DXMatrixIdentity(&transformMat);

		if (center)
		{
			D3DXMatrixTranslation(&centerTransMatI, -center->x, -center->y, 0);
			transformMat *= centerTransMatI;
		}
		if (scale)
		{
			D3DXMatrixScaling(&scaleMat, scale->x, scale->y, 0);
			transformMat *= scaleMat;
		}
		if (rotation)
		{
			D3DXMatrixRotationZ(&rotMat, *rotation);
			transformMat *= rotMat;
		}
		if (pos)
		{
			D3DXMatrixTranslation(&transMat, pos->x, pos->y, 0);
			transformMat *= transMat;
		}

		transformMat *= originalTransformMat /*finalScale*/;

		// 행렬 적용
		sprite->SetTransform(&transformMat);
	}

	// 그린다.
//	sprite->Begin(D3DXSPRITE_ALPHABLEND);
	sprite->Draw(texture, rect, NULL, NULL, color);
//	sprite->End();

	// 원상복귀
	if (transform)
		sprite->SetTransform(&originalTransformMat);
}

void JSprite::GetDevice(LPDIRECT3DDEVICE9 *out)
{
	sprite->GetDevice(out);
}

void JSprite::GetTransform(D3DXMATRIX* mat) const
{
	sprite->GetTransform(mat);
}

void JSprite::SetTransform(const D3DXMATRIX mat)
{
	sprite->SetTransform(&mat);
}

void JSprite::SetFinalScale(const D3DXMATRIX mat)
{
	finalScale = mat;
}


void JSprite::Begin()
{
	sprite->Begin(D3DXSPRITE_ALPHABLEND);
}

void JSprite::End()
{
	sprite->End();
};
