#include "JIme.h"

#include <imm.h>
#include "JMain.h"

LRESULT JIme::ImeMsgProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage)
	{
	case WM_IME_STARTCOMPOSITION:
		return 0;

	case WM_IME_COMPOSITION:
		if (lParam & GCS_COMPSTR)
		{
			TCHAR ch[2] = {0,};
			HIMC hImc = ImmGetContext(hWnd);
			int len = ImmGetCompositionString(hImc, GCS_COMPSTR, NULL, 0);
			if (len==2)
			{
				ImmGetCompositionString(hImc, GCS_COMPSTR, ch, len);
			}
			ime_comp_char[0] = (len==0)?-1:ch[0];
			ImmReleaseContext(hWnd, hImc);
		}
		return DefWindowProc(hWnd, iMessage, wParam, lParam);

	case WM_IME_CHAR:
		{
			if (ime_comp_char[1] == 0)
				ime_comp_char[1] = (TCHAR)wParam;
			else
				ime_comp_char[2] = (TCHAR)wParam;
			return 0;
		}

	case WM_CHAR:
		{
			if (wParam>=32 && wParam != 127)
				ime_comp_char[1] = (TCHAR)wParam;
			return 0;
		}
	}
	return -1;
}

void JIme::ResetComposition()
{
	HWND hWnd = JMain::GetInstance().GetJWindow().GetWindowHandle();
	HIMC hImc = ImmGetContext(hWnd);
	ImmSetCompositionString(hImc, SCS_SETSTR, NULL, 0, NULL, 0);
	ImmReleaseContext(hWnd, hImc);
	ZeroMemory(&ime_comp_char, 3*sizeof(TCHAR));
}

void JIme::GetCompChar(TCHAR* out)
{
	out[0] = ime_comp_char[0];
	out[1] = ime_comp_char[1];
	out[2] = ime_comp_char[2];

	ZeroMemory(&ime_comp_char, 3*sizeof(TCHAR));
}

void JIme::SetImeMode(const DWORD mode)
{
	HWND hWnd = JMain::GetInstance().GetJWindow().GetWindowHandle();
	HIMC hImc = ImmGetContext(hWnd);
	DWORD dwConv, dwSent;
	ImmGetConversionStatus(hImc, &dwConv, &dwSent);
	ImmSetConversionStatus(hImc, mode, dwSent);
	ImmReleaseContext(hWnd, hImc);
}
