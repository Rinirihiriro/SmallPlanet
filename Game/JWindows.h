/*
	윈도우를 초기화하고 관리하는 클래스
*/
#pragma once

#include <Windows.h>

class JWindows
{
private:
	unsigned int iWindowWidth;
	unsigned int iWindowHeight;

	DWORD WindowStyle;

public:
	HWND hWindow;
	HINSTANCE hInstance;
	LPCTSTR lpszClassName;
	CRITICAL_SECTION _BGMcs;	// BGM Streaming Critical Section

public:
	JWindows( const HINSTANCE& hInstance, LPCTSTR lpszClassName
		, const unsigned int w, unsigned const int h
		, const bool fullscreen );
	~JWindows();

	void SetWindowSize(const int w, const int h);

	void GetWindowSize(unsigned int* w, unsigned int* h) const;
	HWND GetWindowHandle() const;
	HINSTANCE GetWindowInstance() const;

	void SetFullScreenAndReset(const bool fullscreen = true);
};