#pragma once

#include <map>

#include <d3dx9math.h>
#include "JFreeType.h"

class Texture;

class FTFont
{
public:
	enum AlignOption
	{
		FONT_LEFT		= 0x00
		, FONT_CENTER	= 0x01
		, FONT_RIGHT	= 0x02
		, FONT_VCENTER	= 0x04
		, FONT_TOP	= 0x08
	};
private:
	static JFreeType* pFreeType;

	Texture* txTexture;
	FT_Face ftFace;

	bool apply_kerning;
	bool bOutlined;

	struct GlyphCash
	{
		Texture* txGlyph;
		
		int bitmap_left;
		int bitmap_top;
		
		float advance_x;
		float advance_y;

		int iUsingCounter;
	};
	/*
		A를 저장할 때 :
		A			: 일반
		A + 0x10000	: 굵게
		A + 0x20000 : 기울임
	*/
	std::map<long, GlyphCash> mCache;

public:
	FTFont(LPCSTR fontPath, const long faceIndex, const unsigned int height
		, const bool outlined=false);
	~FTFont();

	void DrawText(const int x, const int y, LPCTSTR text
		, const D3DXCOLOR color, const int align, const bool formated=false
		, Texture* texture=NULL);
	void DrawTextInBox(const int x, const int y, const int w, const int h
		, LPCTSTR text, const D3DXCOLOR color, const int align, const bool formated=false);
	void CalcSize(LPCTSTR text, int* width, int* height, const bool formated=false);

	void ClearCache();

	static void SetJFreeType(JFreeType* freeType);
	int GetFontHeight();

private:
	void UpdateCache();
};