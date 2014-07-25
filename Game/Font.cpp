#include <tchar.h>
#include "MyException.h"

#include "Font.h"

Font::Font( LPD3DXFONT font )
	:font(font)
{
}

Font::~Font()
{
	font->Release();
}

void Font::DrawText( const long x, const long y, LPCTSTR text, const int len
	, const D3DCOLOR color, const int/*FONT_ALIGN*/ align ) const
{
	RECT rt = {x, y, x, y};
	font->DrawText(
		NULL
		, text
		, len
		, &rt
		, align | DT_NOCLIP
		, color
		);
}

void Font::DrawTextInBox( const long x, const long y, const long w
	, const long h, LPCTSTR text, const int len, const D3DCOLOR color
	, const int/*FONT_ALIGN*/ align) const
{
	RECT rt = {x, y, x+w, y+h};
	font->DrawText(
		NULL
		, text
		, len
		, &rt
		, align/* | DT_WORDBREAK*/
		, color
		);

}

long Font::CalcWidth( LPCTSTR text, const int len )
{
	RECT rect = {0};
	font->DrawText(
		NULL
		, text
		, len
		, &rect
		, DT_CALCRECT
		, 0
		);
	return rect.right - rect.left;
}

void Font::OnLostDevice()
{
	font->OnLostDevice();
}

void Font::OnResetDevice()
{
	font->OnResetDevice();
}
