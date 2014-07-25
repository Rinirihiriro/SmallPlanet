// �޸� ���� Ž��
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
	// n��° �޸� �Ҵ� �� �극��ũ
	// ���â���� {}�ȿ� ����ִ� ����
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
		// ���� ���
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
		MessageBox(NULL, L"�޸� ���� �߻�.\n���â�� Ȯ�����ּ���", L"DEBUG", MB_OK);
	}
#endif

	return returnvalue;
}