#pragma once

#include <d3d9.h>
#include <d3dx9.h>

class JSprite
{
private:
	LPD3DXSPRITE sprite;
	D3DXMATRIX finalScale;

public:
	JSprite(LPD3DXSPRITE sprite);
	JSprite();
	~JSprite();

	void ResetSprite(LPD3DXSPRITE sprite);

	// center~rotation은 NULL을 넣을 시 작동하지 않음
	void DrawTexture(LPDIRECT3DTEXTURE9 texture
			, const RECT *rect = NULL
			, const D3DXVECTOR3 *center = NULL
			, const D3DXVECTOR3 *pos = NULL
			, const D3DXVECTOR3 *scale = NULL
			, const float *rotation = NULL					// Radian
			, const D3DXCOLOR color = 0xffffffff);

	void GetDevice(LPDIRECT3DDEVICE9 *out);
	void GetTransform(D3DXMATRIX* mat) const;

	void SetTransform(const D3DXMATRIX mat);
	void SetFinalScale(const D3DXMATRIX mat);


	void Begin();
	void End();
};