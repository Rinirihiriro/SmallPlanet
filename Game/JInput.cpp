#include "JInput.h"

HWND* JInput::hwnd = NULL;

BYTE JInput::old_key[256];
BYTE JInput::now_key[256];

bool JInput::old_double_click[2];
bool JInput::now_double_click[2];

int JInput::last_key;

POINT JInput::old_mouse;
POINT JInput::now_mouse;

SHORT JInput::old_mouse_wheel;
SHORT JInput::now_mouse_wheel;

// CONTROLLER_STATE JInput::old_controller_state[4]={0};
// CONTROLLER_STATE JInput::now_controller_state[4]={0};
Controller JInput::controller;
JIme JInput::ime;

BYTE JInput::key_delay[256] = {0,};
bool JInput::key_delay_flag[2] = {false, false};

const BYTE JInput::KEY_DELAY_FRAME[2] = {10, 5};

double JInput::screen_scale[2] = {1, 1};

bool JInput::enabled = false;

LRESULT JInput::CInputMsgProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage)
	{
	case WM_KEYDOWN:
		{
			JInput::SetLastKeyInput((int)wParam);
			return 0;
		}
	case WM_LBUTTONDBLCLK:
		{
			JInput::SetMouseDoubleClicked();
			return 0;
		}
	case WM_MOUSEWHEEL:
		{
			JInput::SetMouseWheel((short)(wParam >> 16 & 0xffff));
			return 0;
		}
	}
	return ime.ImeMsgProc(hWnd, iMessage, wParam, lParam);
}

void JInput::Update()
{
	if (!enabled) return;

	memcpy_s(old_key, sizeof(old_key), now_key, sizeof(now_key));
	for (int i=0; i<256; i++)
	{
// 		old_key[i] = now_key[i];
		if (key_delay[i])
			key_delay[i] = max(key_delay[i]-1, 0);
	}

//	last_key = 0;
	
	old_mouse = now_mouse;
	
	old_double_click[0] = now_double_click[0];
	old_double_click[1] = now_double_click[1];
	now_double_click[0] = now_double_click[1] = false;
	
	old_mouse_wheel = now_mouse_wheel;
	now_mouse_wheel = 0;

	GetKeyboardState(now_key);
	GetCursorPos(&now_mouse);
	controller.Update();
// 	for (int i=0; i<4; i++)
// 	{
// 		old_controller_state[i] = now_controller_state[i];
// 		DWORD result = XInputGetState(i, &now_controller_state[i].state);
// 		now_controller_state[i].connected = !(result == ERROR_DEVICE_NOT_CONNECTED);
// 	}
	
	key_delay_flag[0] = key_delay_flag[1];
	key_delay_flag[1] = false;
}

bool JInput::IsPressed( const int key )
{
	if ( key == VK_ANYKEY )
	{
		for ( int i=0; i<256; i++ )
		{
			if ( ( now_key[i] & 0x80 ) > 0 )
			{
				return true;
			}
		}
		return false;
	}
	else if ( key == VK_LBUTTONDBCLICK )
	{
		return false;
	}

	return ( now_key[key] & 0x80 ) > 0;
}

bool JInput::IsPressedWithDelay( const int key )
{
	if ( key == VK_ANYKEY )
	{
/*
		for ( int i=0; i<256; i++ )
		{
			if ( ( now_key[i] & 0x80 ) > 0 )
			{
				key_delay_flag[1] = true;
				if (key_delay[0]==0)
				{
					key_delay[0] = KEY_DELAY_FRAME[((key_delay_flag[0])? 1:0)];
					return true;
				}
				else
					return false;
			}
		}
*/
		return false;
	}
	else if ( key == VK_LBUTTONDBCLICK )
	{
		return false;
	}
	else if ( ( now_key[key] & 0x80 ) > 0 )
	{
		// 또 다른 키를 새로 누른 경우도 플래그 리셋하기 위해서
		if (( old_key[key] & 0x80 ) == 0) key_delay_flag[0] = false;

		key_delay_flag[1] = true;
		if (key_delay[key]==0)
		{
			key_delay[key] = KEY_DELAY_FRAME[((key_delay_flag[0])? 1:0)];
			return true;
		}
		else
			return false;
	}

	key_delay[key] = 0;
	return false;
}

bool JInput::IsTriggered( const int key )
{
	if ( key == VK_ANYKEY )
	{
		for ( int i=0; i<256; i++ )
		{
			if ( ( old_key[i] & 0x80 ) == 0 && ( now_key[i] & 0x80 ) > 0 )
			{
				return true;
			}
		}
		return false;
	}
	else if ( key == VK_LBUTTONDBCLICK )
	{
		return old_double_click[0];
	}

	return ( old_key[key] & 0x80 ) == 0 && ( now_key[key] & 0x80 ) > 0;
}

bool JInput::IsReleased( const int key )
{
	if ( key == VK_ANYKEY )
	{
		for ( int i=0; i<256; i++ )
		{
			if ( ( old_key[i] & 0x80 ) > 0 && ( now_key[i] & 0x80 ) == 0 )
			{
				return true;
			}
		}
		return false;
	}
	else if ( key == VK_LBUTTONDBCLICK )
	{
		return false;
	}

	return ( old_key[key] & 0x80 ) > 0 && ( now_key[key] & 0x80 ) == 0;
}

int JInput::GetLastKeyInput() {return last_key;}
void JInput::SetLastKeyInput( const int key ) {last_key = key;}

POINT JInput::GetMousePos()
{
	POINT p = now_mouse;
	ScreenToClient(*hwnd, &p);
	p.x = (long)(p.x/screen_scale[0]);
	p.y = (long)(p.y/screen_scale[1]);
	return p;
}

POINT JInput::GetMousePosAtScreen()
{
	return now_mouse;
}

POINT JInput::GetMouseMovement()
{
	POINT p = {now_mouse.x - old_mouse.x, now_mouse.y - old_mouse.y};
	p.x = (long)(p.x/screen_scale[0]);
	p.y = (long)(p.y/screen_scale[1]);
	return p;
}

short JInput::GetMouseWheel()
{
	return old_mouse_wheel;		// 휠값을 받아온 뒤 업데이트가 되서 old로 해줘야함
}

void JInput::RememberHWND(HWND* hwnd)
{
	JInput::hwnd = hwnd;
}

void JInput::UpdateScreenScale(const double ws, const double hs)
{
	screen_scale[0] = ws;
	screen_scale[1] = hs;
}

void JInput::SetMouseWheel(const short mw)
{
	old_mouse_wheel = now_mouse_wheel;
	now_mouse_wheel = mw;
}

void JInput::SetMouseDoubleClicked(const bool left)
{
	now_double_click[(left)? 0:1] = true;
}

void JInput::InputEnable(const bool enable)
{
	JInput::enabled = enable;
	XInputEnable(enable);
}