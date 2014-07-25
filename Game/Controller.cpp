#include <Windows.h>
#include "Controller.h"

#define STICK_PRESS_MIN 0.5f

CONTROLLER_STATE::CONTROLLER_STATE()
{
	for (int i=0;i<4;i++) stick_state[i] = 0;
	connected = false;
}

CONTROLLER_STATE::CONTROLLER_STATE(const CONTROLLER_STATE& other)
{
	this->state = other.state;
	this->keystroke = other.keystroke;
	for (int i=0; i<4; i++) this->stick_state[i] = other.stick_state[i];
	this->connected = other.connected;
}


void Controller::Update()
{
	for (int i=0; i<4; i++)
	{
		old_controller_state[i] = now_controller_state[i];

		DWORD result = XInputGetState(i, &now_controller_state[i].state);
		now_controller_state[i].connected = !(result == ERROR_DEVICE_NOT_CONNECTED);

		if (!now_controller_state[i].connected) continue;

		XInputGetKeystroke(i, XINPUT_FLAG_GAMEPAD, &now_controller_state[i].keystroke);

		GetLeftAnalogStick(&now_controller_state[i].stick_state[0]
		, &now_controller_state[i].stick_state[1], i);
		GetRightAnalogStick(&now_controller_state[i].stick_state[2]
		, &now_controller_state[i].stick_state[3], i);
	}
}


bool Controller::IsConnected(const unsigned int index)
{
	return (index<4 && now_controller_state[index].connected);
}


bool Controller::IsButtonPressed(const int key, const unsigned int index)
{
	if (index>=4 || !now_controller_state[index].connected) return false;

	if (key == ALL)
		return now_controller_state[index].state.Gamepad.wButtons > 0;
	else if (key < 0x10000)
		return (now_controller_state[index].state.Gamepad.wButtons & key) > 0;
	else
	{
		switch (key)
		{
		case LTRIGGER:
			return GetLeftTrigger(index)>0;
		case RTRIGGER:
			return GetRightTrigger(index)>0;
		}
	}
	return false;
}

bool Controller::IsButtonTriggered(const int key, const unsigned int index)
{
	if (index>=4 || !now_controller_state[index].connected) return false;

	if (key == ALL)
		return now_controller_state[index].state.Gamepad.wButtons > 0
			&& old_controller_state[index].state.Gamepad.wButtons == 0;
	else if (key < 0x10000)
		return (now_controller_state[index].state.Gamepad.wButtons & key) > 0
			&& (old_controller_state[index].state.Gamepad.wButtons & key) == 0;
	else
	{
		switch (key)
		{
		case LTRIGGER:
			return GetLeftTrigger(index)>0 && GetOldLeftTrigger(index) == 0;
		case RTRIGGER:
			return GetRightTrigger(index)>0 && GetOldRightTrigger(index) == 0;
		}
	}
	return false;
}

bool Controller::IsButtonReleased(const int key, const unsigned int index)
{
	if (index>=4 || !now_controller_state[index].connected) return false;

	if (key == ALL)
		return now_controller_state[index].state.Gamepad.wButtons == 0
			&& old_controller_state[index].state.Gamepad.wButtons > 0;
	else if (key < 0x10000)
		return (now_controller_state[index].state.Gamepad.wButtons & key) == 0
			&& (old_controller_state[index].state.Gamepad.wButtons & key) > 0;
	else
	{
		switch (key)
		{
		case LTRIGGER:
			return GetLeftTrigger(index) == 0 && GetOldLeftTrigger(index) > 0;
		case RTRIGGER:
			return GetRightTrigger(index) == 0 && GetOldRightTrigger(index) > 0;
		}
	}
	return false;
}


bool Controller::IsStickPressed(const int stick, const int dir, const unsigned int index)
{
	if (index>=4 || !now_controller_state[index].connected) return false;

	float x, y;
	switch (stick)
	{
	case LSTICK:
		x = now_controller_state[index].stick_state[0];
		y = now_controller_state[index].stick_state[1];
		break;
	case RSTICK:
		x = now_controller_state[index].stick_state[2];
		y = now_controller_state[index].stick_state[3];
		break;
	default:
		return false;
	}

	switch (dir)
	{
	case UP:
		return y>STICK_PRESS_MIN;
	case DOWN:
		return y<-STICK_PRESS_MIN;
	case RIGHT:
		return x>STICK_PRESS_MIN;
	case LEFT:
		return x<-STICK_PRESS_MIN;
	default:
		return false;
	}
}

bool Controller::IsStickTriggered(const int stick, const int dir, const unsigned int index)
{
	if (index>=4 || !now_controller_state[index].connected) return false;

	float x, y, ox, oy;
	switch (stick)
	{
	case LSTICK:
		x = now_controller_state[index].stick_state[0];
		ox = old_controller_state[index].stick_state[0];
		y = now_controller_state[index].stick_state[1];
		oy = old_controller_state[index].stick_state[1];
		break;
	case RSTICK:
		x = now_controller_state[index].stick_state[2];
		ox = old_controller_state[index].stick_state[2];
		y = now_controller_state[index].stick_state[3];
		oy = old_controller_state[index].stick_state[3];
		break;
	default:
		return false;
	}

	switch (dir)
	{
	case UP:
		return y>STICK_PRESS_MIN && oy<STICK_PRESS_MIN && oy>-STICK_PRESS_MIN;
	case DOWN:
		return y<-STICK_PRESS_MIN && oy<STICK_PRESS_MIN && oy>-STICK_PRESS_MIN;
	case RIGHT:
		return x>STICK_PRESS_MIN && ox<STICK_PRESS_MIN && ox>-STICK_PRESS_MIN;
	case LEFT:
		return x<-STICK_PRESS_MIN && ox<STICK_PRESS_MIN && ox>-STICK_PRESS_MIN;
	default:
		return false;
	}
}

bool Controller::IsStickReleased(const int stick, const int dir, const unsigned int index)
{
	if (index>=4 || !now_controller_state[index].connected) return false;

	float x, y, ox, oy;
	switch (stick)
	{
	case LSTICK:
		x = now_controller_state[index].stick_state[0];
		ox = old_controller_state[index].stick_state[0];
		y = now_controller_state[index].stick_state[1];
		oy = old_controller_state[index].stick_state[1];
		break;
	case RSTICK:
		x = now_controller_state[index].stick_state[2];
		ox = old_controller_state[index].stick_state[2];
		y = now_controller_state[index].stick_state[3];
		oy = old_controller_state[index].stick_state[3];
		break;
	default:
		return false;
	}

	switch (dir)
	{
	case UP:
		return y>-STICK_PRESS_MIN && y<STICK_PRESS_MIN && oy>STICK_PRESS_MIN;
	case DOWN:
		return y>-STICK_PRESS_MIN && y<STICK_PRESS_MIN && oy<-STICK_PRESS_MIN;
	case RIGHT:
		return x>-STICK_PRESS_MIN && x<STICK_PRESS_MIN && ox>STICK_PRESS_MIN;
	case LEFT:
		return x>-STICK_PRESS_MIN && x<STICK_PRESS_MIN && ox<-STICK_PRESS_MIN;
	default:
		return false;
	}
}


void Controller::GetLeftAnalogStick(float* x, float* y, const unsigned int index)
{
	if (index>=4 || !now_controller_state[index].connected)
	{
		if (x) *x = 0;
		if (y) *y = 0;
		return;
	}

	int xx, yy;
	xx = now_controller_state[index].state.Gamepad.sThumbLX;
	yy = now_controller_state[index].state.Gamepad.sThumbLY;

	if ((xx<XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && xx>-XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		&& (yy<XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && yy>-XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
	{
		xx = 0;
		yy = 0;
	}

	if (x) *x = (float)xx/32767;
	if (y) *y = (float)yy/32767;
}

void Controller::GetRightAnalogStick(float* x, float* y, const unsigned int index)
{
	if (index>=4 || !now_controller_state[index].connected) return;

	int xx, yy;
	xx = now_controller_state[index].state.Gamepad.sThumbRX;
	yy = now_controller_state[index].state.Gamepad.sThumbRY;

	if ((xx<XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && xx>-XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
		&& (yy<XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE&& yy>-XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
	{
		xx = 0;
		yy = 0;
	}

	*x = (float)xx/32767;
	*y = (float)yy/32767;
}


float Controller::GetLeftTrigger(const unsigned int index)
{
	if (index>=4 || !now_controller_state[index].connected) return -1;

	int amount = now_controller_state[index].state.Gamepad.bLeftTrigger;
	if (amount < XINPUT_GAMEPAD_TRIGGER_THRESHOLD) amount = 0;

	return (float)amount/255;
}

float Controller::GetRightTrigger(const unsigned int index)
{
	if (index>=4 || !now_controller_state[index].connected) return -1;

	int amount = now_controller_state[index].state.Gamepad.bRightTrigger;
	if (amount < XINPUT_GAMEPAD_TRIGGER_THRESHOLD) amount = 0;

	return (float)amount/255;
}


void Controller::SetVibration(const WORD left, const WORD right, const unsigned int index)
{
	if (index>=4 || !now_controller_state[index].connected) return;

	XINPUT_VIBRATION vibration;
	vibration.wLeftMotorSpeed = left;
	vibration.wRightMotorSpeed = right;
	XInputSetState(index, &vibration);
}



float Controller::GetOldLeftTrigger(const unsigned int index)
{
	if (index>=4 || !old_controller_state[index].connected) return -1;

	int amount = old_controller_state[index].state.Gamepad.bLeftTrigger;
	if (amount < XINPUT_GAMEPAD_TRIGGER_THRESHOLD) amount = 0;

	return (float)amount/255;
}

float Controller::GetOldRightTrigger(const unsigned int index)
{
	if (index>=4 || !old_controller_state[index].connected) return -1;

	int amount = old_controller_state[index].state.Gamepad.bRightTrigger;
	if (amount < XINPUT_GAMEPAD_TRIGGER_THRESHOLD) amount = 0;

	return (float)amount/255;
}
