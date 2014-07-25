// 메모리 누수 탐지
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <Windows.h>
#include <tchar.h>

#include "JMain.h"
#include "MyException.h"
#include "GlobalDataManager.h"

/*
	** 4:3
	 - 640 x 480
	 - 800 x 600
	 - 1200 x 900

	** 16:9
	 - 960 x 540
	 - 1280 x 720
	 - 1600 x 900
*/

int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	// n번째 메모리 할당 시 브레이크
	// 출력창에서 {}안에 들어있는 숫자
	// _CrtSetBreakAlloc(804);

// 	wchar_t path[128] = {0,};
// 	GetCurrentDirectory(128, path);
// 	MessageBox(NULL, path, L"", MB_OK);

	int returnvalue;
	try
	{
		unsigned char tmp;
		bool fullscreen;
		GlobalDataManager::GetInstance().GetData("fullscreen", (void*)&tmp);
		fullscreen = tmp>0;
		JMain::CreateCMain( hInstance, L"Small Planet", 400, 300, fullscreen );
		JMain::GetInstance().PrepareToStart();
		returnvalue = JMain::GetInstance().Run();
	}
	catch( MyException e )
	{
		// 예외 출력
		size_t len;
		TCHAR* str;
		if (e.comment_t!=NULL)
		{
			len = _tcslen(e.message)+_tcslen(e.comment_t)+64;
			str = new TCHAR[len];
			_stprintf_s(str, len, L"Error : %s\nComment : %s\nErrorCode : %X", e.message, e.comment_t, e.hr);
		}
		else
		{
			len = _tcslen(e.message)+strlen(e.comment)+64;
			str = new TCHAR[len];
			_stprintf_s(str, len, L"Error : %s\nComment : %hs\nErrorCode : %X", e.message, e.comment, e.hr);
		}
		MessageBox( NULL, str, e.name, MB_OK | MB_ICONERROR );
		return e.hr;
	}
	JMain::Release();

#ifdef _DEBUG
	if (_CrtDumpMemoryLeaks())
	{
		MessageBox(NULL, L"메모리 누수 발생.\n출력창을 확인해주세요", L"DEBUG", MB_OK);
	}
#endif

	return returnvalue;
}