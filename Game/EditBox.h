#pragma once

#include <string>
#include "CRect.h"
#include "FTFont.h"

class Texture;

class EditBox
{
public:
	enum EditBoxStyle
	{
		EB_LEFT			= 0x00
		, EB_CENTER		= 0x01
		, EB_RIGHT		= 0x02
		
		, EB_ONLYNUM	= 0x04
		, EB_ONLYENG	= 0x08

		, EB_PASSWD		= 0x10
	};

private:
	TCHAR *szBuf;
	int iMaxLen;
	CRect mRect;
	EditBoxStyle mStyle;
	FTFont& mFont;

	int iFontHeight;
	Texture* txText;
	bool bFocus;

	unsigned iOffset;
	bool bComp;

	struct Caret
	{
		int x;
		int y;
		int mode;	// 0:| 1:_
		bool visible;
	} mCaret;

public:
	EditBox(const TCHAR* text, const int maxlen, const CRect rect
		, const EditBoxStyle style, const std::string font);
	~EditBox();

	void InputProc();
	void Draw();
	void Clear();

	bool IsClicked(const int x, const int y);

	void GetText(TCHAR* out, int maxlen) const;
	bool GetFocus() const;
	void SetText(const TCHAR* str);
	void SetFocus(const bool focus);

private:
	void UpdateTexture();
	void SetCaret();
	void HideCaret();
	void DeleteChar(const int off, const int len);
	void InsertChar(const int off, const TCHAR* str);
};