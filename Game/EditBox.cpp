#include "EditBox.h"

#include <tchar.h>
#include "TextureManager.h"
#include "JMain.h"
#include "JInput.h"
#include "MyException.h"

#include "Essential.h"

#include "FTFontManager.h"

EditBox::EditBox(const TCHAR* text, const int maxlen, const CRect rect
	, const EditBox::EditBoxStyle style, const std::string font)
	:iMaxLen(maxlen), mRect(rect), mStyle(style), bFocus(false), txText(NULL), bComp(false), iOffset(0)
	, mFont(FTFontManager::GetInstance().GetFont(font)), mCaret()
{
	iFontHeight = mFont.GetFontHeight();
	szBuf = new TCHAR[maxlen+1];

	D3DXVECTOR3 center(0,0,0);
	txText = new Texture(maxlen*iFontHeight, iFontHeight, D3DFMT_A8R8G8B8, 0, 0, &center);
	txText->ClearTexture((DWORD)0);

	Clear();
	mCaret.visible = false;
}

EditBox::~EditBox()
{
	SAFE_DELETE(txText);
	SAFE_DELETE_ARR(szBuf);
}

void EditBox::InputProc()
{
	if (!bFocus) return;
	bool changed = false;
	TCHAR imechar[3], tmpChar[3]={0,};
	JInput::ime.GetCompChar(imechar);

	// 조합 완료 문자가 있을 경우
	if (imechar[1])
	{
		int len = (imechar[2])? 2:1;
		tmpChar[0] = imechar[1];
		tmpChar[1] = imechar[2];
		tmpChar[2] = 0;
		if (bComp)
		{
			iOffset -= 1;
			DeleteChar(iOffset, 1);
		}
		InsertChar(iOffset, tmpChar);
		iOffset += len;
		bComp = false;
		changed = true;
	}

	// 조합중 문자가 있을 경우
	if (imechar[0])
	{
		tmpChar[0] = imechar[0];
		tmpChar[1] = 0;
		if (bComp)
		{
			iOffset -= 1;
			DeleteChar(iOffset, 1);
		}
		if (imechar[0]!=0xffff)
		{
			InsertChar(iOffset, tmpChar);
			bComp = true;
			iOffset += 1;
		}
		else	// 조합이 취소된 경우
		{
			bComp = false;
		}
		changed = true;
	}

	if (JInput::IsPressedWithDelay(VK_LEFT) && iOffset>0)
	{
		iOffset -= 1;
		SetCaret();
	}
	else if (JInput::IsPressedWithDelay(VK_RIGHT)  && iOffset<_tcslen(szBuf))
	{
		iOffset += 1;
		SetCaret();
	}
	else if (JInput::IsTriggered(VK_HOME))
	{
		iOffset = 0;
		SetCaret();
	}
	else if (JInput::IsTriggered(VK_END))
	{
		iOffset = _tcslen(szBuf);
		SetCaret();
	}
	else if (JInput::IsPressedWithDelay(VK_DELETE) && iOffset<_tcslen(szBuf))
	{
		DeleteChar(iOffset, 1);
		changed = true;
	}
	else if (JInput::IsPressedWithDelay(VK_BACK) && !bComp && !changed && iOffset>0)
	{
		iOffset -= 1;
		DeleteChar(iOffset, 1);
		changed = true;
	}

	// 텍스처 갱신
	if (changed)
	{
		UpdateTexture();
		SetCaret();
	}
}

void EditBox::Draw()
{
	RECT rect = {0, 0, (long)mRect.width, (long)mRect.height};
	txText->DrawRect(mRect.center-D3DXVECTOR3(mRect.width/2, mRect.height/2, 0), rect);

	if (mCaret.visible)
	{
		D3DXVECTOR3 scale(iFontHeight/16.f, iFontHeight/16.f, 1);
		TextureManager::GetInstance().GetTexture("caret")->Draw(
			D3DXVECTOR3((float)mCaret.x, (float)mCaret.y, 0), &scale, 0, mCaret.mode);
	}
}

void EditBox::Clear()
{
	ZeroMemory(szBuf, (iMaxLen+1)*sizeof(TCHAR));
	txText->ClearTexture((DWORD)0);
	iOffset = 0;
	bComp = false;
	SetCaret();
}


bool EditBox::IsClicked(const int x, const int y)
{
	return mRect&D3DXVECTOR3((float)x, (float)y, 0);
}


void EditBox::GetText(TCHAR* out, int maxlen) const
{
	if (maxlen-1<iMaxLen) return;
	_tcscpy_s(out, maxlen, szBuf);
}

bool EditBox::GetFocus() const
{
	return bFocus;
}

void EditBox::SetText(const TCHAR* str)
{
	if ((int)_tcslen(str)>iMaxLen) return;
	_tcscpy_s(szBuf, iMaxLen+1, str);
	iOffset = (int)_tcslen(str);
	UpdateTexture();
}

void EditBox::SetFocus(const bool focus)
{
	bFocus = focus;
	JInput::ime.ResetComposition();
	bComp = false;

	if (focus)
		SetCaret();
	else
		HideCaret();
}

void EditBox::UpdateTexture()
{
	txText->ClearTexture((DWORD)0);
	mFont.DrawText(0, 0, szBuf, 0xff000000, FTFont::FONT_LEFT, false, txText);
}

void EditBox::SetCaret()
{
	int x, y, mode;
	TCHAR *tmpbuf = new TCHAR[iOffset+1];

	if (bComp)
	{
		mode = 1;
		_tcsncpy_s(tmpbuf, iOffset, szBuf, iOffset-1);
		mFont.CalcSize(tmpbuf, &x, &y, false);
	}
	else
	{
		mode = 0;
		_tcsncpy_s(tmpbuf, iOffset+1, szBuf, iOffset);
		mFont.CalcSize(tmpbuf, &x, &y, false);
	}
	delete[] tmpbuf;

	x += (int)(mRect.center.x-mRect.width/2);
	y = (int)(mRect.center.y-mRect.height/2);

	mCaret.x = x;
	mCaret.y = y;
	mCaret.mode = mode;
	mCaret.visible = true;
}

void EditBox::HideCaret()
{
	mCaret.visible = false;
}


void EditBox::DeleteChar(const int off, const int len)
{
	int movelen = (int)_tcslen(szBuf)-off-len+1;
	if (movelen<=0) return;
	memmove_s(szBuf+off, (iMaxLen+1)*sizeof(TCHAR), szBuf+off+len, movelen*sizeof(TCHAR));
}

void EditBox::InsertChar(const int off, const TCHAR* str)
{
	int str_len = (int)_tcslen(str);
	int movelen = (int)_tcslen(szBuf)-off+1;
	if ((int)_tcslen(szBuf)+str_len+1>=iMaxLen) return;
	memmove_s(szBuf+off+str_len, (iMaxLen+1)*sizeof(TCHAR), szBuf+off, movelen*sizeof(TCHAR));
	memcpy_s(szBuf+off, (iMaxLen+1)*sizeof(TCHAR), str, str_len*sizeof(TCHAR));
}
