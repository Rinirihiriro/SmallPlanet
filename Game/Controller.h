#pragma once

#include <XInput.h>

struct CONTROLLER_STATE
{
	XINPUT_STATE state;
	XINPUT_KEYSTROKE keystroke;
	float stick_state[4];
	bool connected;

	CONTROLLER_STATE();
	CONTROLLER_STATE(const CONTROLLER_STATE& other); 
};

class Controller
{
public:
	enum ButtonVK
	{
		ALL			= -1
		, UP		= XINPUT_GAMEPAD_DPAD_UP
		, RIGHT		= XINPUT_GAMEPAD_DPAD_RIGHT
		, DOWN		= XINPUT_GAMEPAD_DPAD_DOWN
		, LEFT		= XINPUT_GAMEPAD_DPAD_LEFT
		, START		= XINPUT_GAMEPAD_START
		, BACK		= XINPUT_GAMEPAD_BACK
		, LSTICK	= XINPUT_GAMEPAD_LEFT_THUMB
		, RSTICK	= XINPUT_GAMEPAD_RIGHT_THUMB
		, LSHOULDER	= XINPUT_GAMEPAD_LEFT_SHOULDER
		, RSHOULDER	= XINPUT_GAMEPAD_RIGHT_SHOULDER
		, A			= XINPUT_GAMEPAD_A
		, B			= XINPUT_GAMEPAD_B
		, X			= XINPUT_GAMEPAD_X
		, Y			= XINPUT_GAMEPAD_Y
		, LTRIGGER	= 0x10000
		, RTRIGGER	= 0x20000
	};

private:
	CONTROLLER_STATE old_controller_state[4];
	CONTROLLER_STATE now_controller_state[4];

public:
	void Update();

	bool IsConnected(const unsigned int index);

	bool IsButtonPressed(const int key, const unsigned int index);
	bool IsButtonTriggered(const int key, const unsigned int index);
	bool IsButtonReleased(const int key, const unsigned int index);

	bool IsStickPressed(const int stick, const int dir, const unsigned int index);
	bool IsStickTriggered(const int stick, const int dir, const unsigned int index);
	bool IsStickReleased(const int stick, const int dir, const unsigned int index);

	// Output : -1.0f ~ 1.0f
	void GetLeftAnalogStick(float* x, float* y, const unsigned int index);
	void GetRightAnalogStick(float* x, float* y, const unsigned int index);
	
	// Output : 0.0f ~ 1.0f
	float GetLeftTrigger(const unsigned int index);
	float GetRightTrigger(const unsigned int index);

	// Input : 0 ~ 65535
	void SetVibration(const WORD left, const WORD right, const unsigned int index);

private:
	float GetOldLeftTrigger(const unsigned int index);
	float GetOldRightTrigger(const unsigned int index);

};