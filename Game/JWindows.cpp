#include "MyException.h"
#include "JWindows.h"
#include "JInput.h"

LRESULT CALLBACK WndProc( HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam )
{
	LRESULT lr;
	HWND focusedWnd = GetFocus();
	switch (iMessage)
	{
	case WM_SETFOCUS:
		{
			JInput::InputEnable(true);
			return 0;
		}
	case WM_KILLFOCUS:
		{
			JInput::InputEnable(false);
			return 0;
		}
	case WM_DESTROY:
		{
			PostQuitMessage( 0 );
			return 0;
		}
	}
	if (focusedWnd == hWnd)
	{
		lr = JInput::CInputMsgProc(hWnd, iMessage, wParam, lParam);
		if (lr>=0) return lr;
	}
	return DefWindowProc(hWnd, iMessage, wParam, lParam);
}

JWindows::JWindows( const HINSTANCE& hInstance, LPCTSTR lpszClassName
	, const unsigned int w, const unsigned int h
	, const bool fullscreen)
	:iWindowWidth(w), iWindowHeight(h), hInstance(hInstance), lpszClassName(lpszClassName)
{
	// HINSTANCE hDll = LoadLibrary(L"Resource_dll.dll");

	WNDCLASS wc = {
		CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS
		, WndProc
		, 0
		, 0
		, hInstance
		, LoadIcon(NULL, IDI_APPLICATION)
//		, LoadIcon(hInstance, MAKEINTRESOURCE(ICON))
		, LoadCursor(NULL, IDC_ARROW)
		, (HBRUSH)GetStockObject(BLACK_BRUSH)
		, NULL
		, lpszClassName
	};

	ShowCursor(!fullscreen);

	RegisterClass(&wc);

	WindowStyle = (fullscreen)? (WS_POPUP):(WS_CAPTION | WS_SYSMENU);

	hWindow = CreateWindow(
		lpszClassName
		, lpszClassName
		, WindowStyle
		, CW_USEDEFAULT
		, CW_USEDEFAULT
		, iWindowWidth
		, iWindowHeight
		, NULL
		, NULL
		, hInstance
		, NULL
	);
	if (hWindow == NULL) throw ObjectWasNotCreated("Window Handle");
	ShowWindow(hWindow, SW_SHOW);

	RECT screen = {0, 0, iWindowWidth, iWindowHeight};
	AdjustWindowRect( &screen, WindowStyle, NULL );
	SetWindowPos( hWindow, HWND_NOTOPMOST, 0, 0
		, screen.right-screen.left, screen.bottom-screen.top, SWP_NOMOVE);

	InitializeCriticalSection( &_BGMcs );
}

JWindows::~JWindows()
{
	DeleteCriticalSection( &_BGMcs );
	UnregisterClass(lpszClassName, hInstance);
}


void JWindows::SetWindowSize(const int w, const int h)
{
	iWindowWidth = w;
	iWindowHeight = h;
	RECT screen = {0, 0, w, h};
	AdjustWindowRect( &screen, WindowStyle, NULL );
	SetWindowPos( hWindow, HWND_NOTOPMOST, 0, 0
		, screen.right-screen.left, screen.bottom-screen.top, SWP_NOMOVE);
}


void JWindows::GetWindowSize(unsigned int* w, unsigned int* h) const
{
	(*w) = (iWindowWidth);
	(*h) = (iWindowHeight);
}

HWND JWindows::GetWindowHandle() const
{
	return hWindow;
}

HINSTANCE JWindows::GetWindowInstance() const
{
	return hInstance;
}


void JWindows::SetFullScreenAndReset(const bool fullscreen)
{
	WindowStyle = (fullscreen)? (WS_POPUP):(WS_CAPTION | WS_SYSMENU);
	SetWindowLong(hWindow, GWL_STYLE, WindowStyle);
}