/*
	�Է� ó���� �˻��ϰ� ������Ʈ�ϴ� Ŭ����
*/
#pragma once
#include <Windows.h>
// #include <XInput.h>
#include "Controller.h"
#include "JIme.h"

#define VK_ANYKEY -1
#define VK_LBUTTONDBCLICK -2

// struct CONTROLLER_STATE
// {
// 	XINPUT_STATE state;
// 	bool connected;
// };

class JInput
{
private:
	static HWND* hwnd;

	static BYTE old_key[256];
	static BYTE now_key[256];

	static bool old_double_click[2];
	static bool now_double_click[2];

	static int last_key;

	static POINT old_mouse;
	static POINT now_mouse;

	static SHORT old_mouse_wheel;
	static SHORT now_mouse_wheel;	// ��� : ��, ���� : �Ʒ�, |�ִ�| : 120

	static BYTE key_delay[256];
	static bool key_delay_flag[2];

	static const BYTE KEY_DELAY_FRAME[2];

	static double screen_scale[2];

	static bool enabled;

public:
	static Controller controller;
	static JIme ime;

public:
	static LRESULT CInputMsgProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

	static void Update();
	
	static bool IsPressed( const int key);				// Ű�� �����ִ� ���
	static bool IsPressedWithDelay( const int key);	// Ű�� �����ִ� ��� (������ ����)
	static bool IsTriggered( const int key);			// Ű�� ���� ���� ���
	static bool IsReleased( const int key);			// Ű�� ���� ���� ���

	static int GetLastKeyInput();
	static void SetLastKeyInput( const int key );

	static POINT GetMousePos();
	static POINT GetMousePosAtScreen();
	static POINT GetMouseMovement();
	static short GetMouseWheel();

	static void RememberHWND(HWND* hwnd);
	static void UpdateScreenScale(const double ws, const double hs);

	static void SetMouseWheel(const short mw);
	static void SetMouseDoubleClicked(const bool left = true);

	static void InputEnable(const bool enable);
};