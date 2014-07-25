/*
	D3DFONT를 관리하고 사용하기 위한 클래스
	폰트 객체는 JDirectX에서 생성해 가져온다.
*/
#pragma once

#include <d3dx9.h>

class Font
{
private:
	LPD3DXFONT font;

public:

	enum FONT_ALIGN
	{
		FONT_LEFT = DT_LEFT
		,FONT_RIGHT = DT_RIGHT
		,FONT_CENTER = DT_CENTER

		,FONT_VHCENTER = DT_CENTER | DT_VCENTER

		,FONT_VCENTER = DT_VCENTER
		,FONT_WORDBREAK = DT_WORDBREAK
	};

	Font( LPD3DXFONT font );
	~Font();

	void DrawText( const long x, const long y, LPCTSTR text, const int len
		, const D3DCOLOR color, const int/*FONT_ALIGN*/ align = FONT_LEFT ) const;
	void DrawTextInBox( const long x, const long y, const long w
		, const long h, LPCTSTR text, const int len, const D3DCOLOR color
		, const int/*FONT_ALIGN*/ align = FONT_LEFT) const;
	long CalcWidth( LPCTSTR text, const int len );

	void OnLostDevice();
	void OnResetDevice();

};