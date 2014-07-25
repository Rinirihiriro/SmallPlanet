#include "FTFont.h"

#include "Essential.h"

#include "Texture.h"

#include <freetype/ftoutln.h>
#include <freetype/fttypes.h>

#include <vector>
#include <string>
#include <sstream>

JFreeType* FTFont::pFreeType = NULL;

FTFont::FTFont(LPCSTR fontPath, const long faceIndex, const unsigned int height, const bool outlined)
	:txTexture(NULL), bOutlined(outlined)
{
	pFreeType->CreateFont(fontPath, faceIndex, &ftFace);
	FT_Set_Pixel_Sizes(ftFace, 0, height);

	bool apply_kerning = FT_HAS_KERNING(ftFace)>0;
}

FTFont::~FTFont()
{
	SAFE_DELETE(txTexture);
	FT_Done_Face(ftFace);
	ClearCache();
}

/*
	Text Format :
	\b ~ \b/	: Bold
	\i ~ \i/	: Italic
	\#aarrggbb	: Change Color
	\\			: \
*/
void FTFont::DrawText(const int x, const int y, LPCTSTR text
	, const D3DXCOLOR color, const int align, const bool formated, Texture* texture)
{
	if (!text || !text[0]) return;

	static D3DXCOLOR textcolor=0xff000000;
	bool flag[2]={0,};			// b, i

	if (color>0) textcolor=color;

	float pen_x, pen_y;
	int width, height;
	int tx_width, tx_hight;
	DWORD* textureBuffer;
	D3DXCOLOR c;
	D3DXVECTOR3 center(0,0,0);
	GlyphCash glyph_cache;
	std::map<long, GlyphCash>::iterator cache;
	TCHAR buffer[256];

	// Kerning
	short left_char=0, right_char=0;
	FT_Vector kerning;

	pen_x = (float)x;
	pen_y = (float)y+(ftFace->size->metrics.ascender>>6);


	// Set(Align) Pen's position
	if (align != FONT_LEFT)
	{
		int i, w, h;
		for (i=0; text[i] != 0 && text[i] != '\n'; i++)
			buffer[i] = text[i];
		buffer[i] = 0;
		CalcSize(buffer, &w, &h, formated);
		
		if (align&FONT_CENTER)
		{
			pen_x-=w/2;
		}
		else if (align&FONT_RIGHT)
		{
			pen_x-=w;
		}

		if (align&FONT_VCENTER)
		{
			pen_y-=h/2;
		}
		else if (align&FONT_TOP)
		{
			pen_y-=h;
		}
	}
	

	// Render Text
	for (int i=0; text[i]!=0; i++)
	{
		if (text[i] == '\n')
		{
			// Reset Pen's position if new line
			pen_x=(float)x;
			pen_y += ftFace->size->metrics.height/64.f;
			left_char = 0;

			if (align != FONT_LEFT)
			{
				int j, w, h;
				for (j=0; text[i+j+1] != 0 && text[i+j+1] != '\n'; j++)
					buffer[j] = text[i+j+1];
				buffer[j] = 0;
				CalcSize(buffer, &w, &h, formated);

				if (align&FONT_CENTER)
				{
					pen_x-=w/2;
				}
				else if (align&FONT_RIGHT)
				{
					pen_x-=w;
				}
			}

			continue;
		}
		if (formated && text[i]=='\\')
		{
			// special commands
			switch (text[i+1])
			{
			case 'b':
			case 'B':
				{
					if (text[i+2] == '/')
					{
						flag[0] = false;
						i+=2;
					}
					else
					{
						flag[0] = true;
						i+=1;
					}
					continue;
				}
			case 'i':
			case 'I':
				{
					if (text[i+2] == '/')
					{
						flag[1] = false;
						i+=2;
					}
					else
					{
						flag[1] = true;
						i+=1;
					}
					continue;
				}
			case '#':
				{
					i+=2;
					std::wstring hexstr = L"0x";
					unsigned int hex;
					for (int j=0; j<8; j++) hexstr += text[i++];
					std::wstringstream tohex(hexstr);
					tohex>>std::hex>>hex;
					textcolor = hex;
					i--;
					continue;
				}
			case '//':
				i++;
				break;
			}
		}

		long char_id = text[i];
		if (flag[0]) char_id += 0x10000;
		if (flag[1]) char_id += 0x20000;

		cache = mCache.find(char_id);
		if (cache == mCache.end())
		{
			// 캐싱되어있지 않으면 새로 불러온다.
			txTexture = new Texture(
				ftFace->size->metrics.x_ppem
				, (ftFace->size->metrics.ascender-ftFace->size->metrics.descender)>>6
				, D3DFMT_A8R8G8B8
				, NULL
				, NULL
				, &center
				);

			txTexture->ClearTexture(D3DXCOLOR(0,0,0,0));
			textureBuffer = (DWORD*)txTexture->LockRect();
			tx_width = txTexture->GetSurfaceWidth();
			tx_hight = txTexture->GetSurfaceHeight();

			long index = FT_Get_Char_Index(ftFace, text[i]);

			// 외각선
			if (bOutlined)
			{
				FT_Load_Glyph(ftFace, index, FT_LOAD_DEFAULT);
				FT_Outline_Embolden(&ftFace->glyph->outline, ((int)(ftFace->size->metrics.x_ppem*0.2)<<4));
				if (flag[0])	// bold
				{
					FT_Outline_Embolden(&ftFace->glyph->outline, ((int)(ftFace->size->metrics.x_ppem*0.1)<<4));
				}
				if (flag[1])	// italic
				{
					FT_Matrix mat={0x10000, 0x06000, 0x00000, 0x10000};
					FT_Outline_Transform(&ftFace->glyph->outline, &mat);
				}
				FT_Render_Glyph(ftFace->glyph, FT_RENDER_MODE_NORMAL);
				// FT_Load_Char(ftFace, text[i], FT_LOAD_RENDER/* | FT_LOAD_NO_BITMAP*/);
				width = ftFace->glyph->bitmap.width;
				height = ftFace->glyph->bitmap.rows;

				for (int y=0; y<height; y++)
				{
					for (int x=0; x<width; x++)
					{
						if (ftFace->glyph->bitmap.buffer[y*width+x]>0)
						{
							c = 0xff000000;
							c.a*=ftFace->glyph->bitmap.buffer[y*width+x]/255.f;
							textureBuffer[y*tx_width+x] = c;
						}
					}
				}
			}

			FT_Load_Glyph(ftFace, index, FT_LOAD_DEFAULT);
			if (flag[0])	// bold
			{
				FT_Outline_Embolden(&ftFace->glyph->outline, ((int)(ftFace->size->metrics.x_ppem*0.1)<<4));
			}
			if (flag[1])	// italic
			{
				FT_Matrix mat={0x10000, 0x06000, 0x00000, 0x10000};
				FT_Outline_Transform(&ftFace->glyph->outline, &mat);
			}
			FT_Render_Glyph(ftFace->glyph, FT_RENDER_MODE_NORMAL);
			// FT_Load_Char(ftFace, text[i], FT_LOAD_RENDER/* | FT_LOAD_NO_BITMAP*/);
			width = ftFace->glyph->bitmap.width;
			height = ftFace->glyph->bitmap.rows;

			for (int y=0; y<height; y++)
			{
				for (int x=0; x<width; x++)
				{
					if (ftFace->glyph->bitmap.buffer[y*width+x]>0)
					{
						c = 0xffffffff;
						c.a*=ftFace->glyph->bitmap.buffer[y*width+x]/255.f;
						if (bOutlined)
						{
							D3DXCOLOR o(textureBuffer[y*tx_width+x]);
							textureBuffer[y*tx_width+x] = o*(1-c.a)+c*c.a;
						}
						else
							textureBuffer[y*tx_width+x] = c;
					}
				}
			}

			txTexture->UnlockRect();

			glyph_cache.txGlyph			= txTexture;
			glyph_cache.bitmap_left		= ftFace->glyph->bitmap_left;
			glyph_cache.bitmap_top		= ftFace->glyph->bitmap_top;
			glyph_cache.advance_x		= ftFace->glyph->advance.x/64.f;
			glyph_cache.advance_y		= ftFace->glyph->advance.y/64.f;

			cache = mCache.insert(std::pair<long, GlyphCash>(char_id, glyph_cache)).first;
		}
		else
		{
			txTexture = cache->second.txGlyph;
		}

		if (apply_kerning)
		{
			right_char = FT_Get_Char_Index(ftFace, text[i]);
			if (left_char != 0)
			{
				FT_Get_Kerning(ftFace, left_char, right_char, FT_KERNING_DEFAULT, &kerning);
				pen_x += kerning.x/64.f;
			}
			left_char = right_char;
		}

		txTexture->color = textcolor;
		if (texture)
			txTexture->DrawOnTexture(
				texture
				, D3DXVECTOR3(pen_x+cache->second.bitmap_left,pen_y-cache->second.bitmap_top,0)
				);
		else
			txTexture->Draw(
				D3DXVECTOR3(pen_x+cache->second.bitmap_left,pen_y-cache->second.bitmap_top,0)
				);

		pen_x += cache->second.advance_x;
		pen_y += cache->second.advance_y;

		txTexture = NULL;
		cache->second.iUsingCounter = 10;
	}
	UpdateCache();
}

void FTFont::DrawTextInBox(const int x, const int y, const int w, const int h
	, LPCTSTR text, const D3DXCOLOR color, const int align, const bool formated)
{

}


void FTFont::CalcSize(LPCTSTR text, int* width, int* height, const bool formated)
{
	int w=0, h=(ftFace->size->metrics.height>>6);
	int max_w=0;

	// Kerning
	short left_char=0, right_char=0;
	FT_Vector kerning;

	for (int i=0; text[i]!=0; i++)
	{
		if (text[i] == '\n')
		{
			h += (ftFace->size->metrics.height>>6);
			w = 0;
			left_char = 0;
			continue;
		}
		if (formated && text[i] == '\\')
		{
			switch (text[i+1])
			{
			case 'b':
			case 'B':
			case 'i':
			case 'I':
				if (text[i+2] == '/')	i+=2;
				else					i+=1;
				continue;
			case '#':
				i+=9;
				continue;
			case '\\':
				continue;
			}
		}

		if (apply_kerning)
		{
			right_char = FT_Get_Char_Index(ftFace, text[i]);
			if (left_char != 0)
			{
				FT_Get_Kerning(ftFace, left_char, right_char, FT_KERNING_DEFAULT, &kerning);
				w += (kerning.x>>6);
			}
			left_char = right_char;
		}

		FT_Load_Char(ftFace, text[i], FT_LOAD_DEFAULT);
		w += (ftFace->glyph->advance.x>>6);

		if (w>max_w) max_w = w;
	}
	if (width) *width = max_w;
	if (height) *height = h;
}


void FTFont::ClearCache()
{
	for each (std::pair<long, GlyphCash> p in mCache)
		SAFE_DELETE(p.second.txGlyph);
	mCache.clear();
}


void FTFont::SetJFreeType(JFreeType* freeType)
{
	SAFE_DELETE(pFreeType);
	pFreeType = freeType;
}

int FTFont::GetFontHeight()
{
	return ftFace->size->metrics.height>>6;
}


void FTFont::UpdateCache()
{
	std::vector<std::map<long, GlyphCash>::iterator> erase_arr;
	std::map<long, GlyphCash>::iterator it;
	for (it = mCache.begin(); it!= mCache.end(); it++)
	{
		if (--it->second.iUsingCounter <= 0)
		{
			erase_arr.push_back(it);
		}
	}

	while (!erase_arr.empty())
	{
		it = erase_arr.back();
		SAFE_DELETE(it->second.txGlyph);
		mCache.erase(it);
		erase_arr.pop_back();
	}
}
