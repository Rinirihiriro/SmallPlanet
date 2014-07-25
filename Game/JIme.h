#pragma once

#include <Windows.h>

class JIme
{
private:
	TCHAR ime_comp_char[3];		// 0:������(-1�� ���� �������) 1~2:�����Ϸ�

public:
	LRESULT ImeMsgProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

	void ResetComposition();
	void GetCompChar(TCHAR* out);
	void SetImeMode(const DWORD mode = IME_CMODE_ALPHANUMERIC);
};