#pragma once

#include <Windows.h>

class JIme
{
private:
	TCHAR ime_comp_char[3];		// 0:조립중(-1이 들어가면 조립취소) 1~2:조립완료

public:
	LRESULT ImeMsgProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

	void ResetComposition();
	void GetCompChar(TCHAR* out);
	void SetImeMode(const DWORD mode = IME_CMODE_ALPHANUMERIC);
};