#include "JInput.h"
#include "StringInputModule.h"

void StringInputModule::StringInputProc(TCHAR* str, const int str_len, OPTION options)
{
	int str_last_pos = _tcslen(str);

	// backspace
	if (JInput::IsPressedWithDelay(VK_BACK) && str_last_pos > 0)
	{
		str[str_last_pos-1] = 0;
		return;
	}

	if (str_last_pos>=str_len) return;

	// input number
	for (int k='0'; k<='9'; k++)
	{
		if (JInput::IsTriggered(k) && !JInput::IsPressed(VK_SHIFT))
		{
			str[str_last_pos] = k;
			str[str_last_pos+1] = 0;
			return;
		}
	}

	// numpad
	for (int k=0x60; k<=0x69; k++)
	{
		if (JInput::IsTriggered(k))
		{
			str[str_last_pos] = k-0x30;
			str[str_last_pos+1] = 0;
			return;
		}
	}

	// minus
	if((options & NUMBER_ONLY) == NUMBER_ONLY && str_last_pos == 0)
	{
		if (JInput::IsTriggered(VK_SUBTRACT))	{str[str_last_pos]='-';str[str_last_pos+1]=0;}
		if (JInput::IsTriggered(VK_OEM_MINUS))	{str[str_last_pos]='-';str[str_last_pos+1]=0;}
	}

	// dot
	if((options & NUMBER_ONLY) == NUMBER_ONLY && str_last_pos > 0)
	{
		if (JInput::IsTriggered(VK_OEM_PERIOD))
		{
			bool already_used=false;
			for (int i=0; i<str_last_pos && !already_used; i++)
				already_used = str[i]=='.';

			if (!already_used) {str[str_last_pos]='.';str[str_last_pos+1]=0;}
		}
	}

	if ((options & NUMBER_ONLY) == NUMBER_ONLY) return;

	// input alphabet
	for (int k='A'; k<='Z'; k++)
	{
		if (JInput::IsTriggered(k))
		{
			if (JInput::IsPressed(VK_SHIFT))
				str[str_last_pos] = k;
			else
				str[str_last_pos] = k+0x20;
			str[str_last_pos+1] = 0;
			return;
		}
	}

	// input symbols

	if (JInput::IsPressed(VK_SHIFT))
	{
		if (JInput::IsTriggered('1')) {str[str_last_pos]='!';str[str_last_pos+1]=0;}
		if (JInput::IsTriggered('2')) {str[str_last_pos]='@';str[str_last_pos+1]=0;}
		if (JInput::IsTriggered('3')) {str[str_last_pos]='#';str[str_last_pos+1]=0;}
		if (JInput::IsTriggered('4')) {str[str_last_pos]='$';str[str_last_pos+1]=0;}
		if (JInput::IsTriggered('5')) {str[str_last_pos]='%';str[str_last_pos+1]=0;}
		if (JInput::IsTriggered('6')) {str[str_last_pos]='^';str[str_last_pos+1]=0;}
		if (JInput::IsTriggered('7')) {str[str_last_pos]='&';str[str_last_pos+1]=0;}
		if (JInput::IsTriggered('8')) {str[str_last_pos]='*';str[str_last_pos+1]=0;}
		if (JInput::IsTriggered('9')) {str[str_last_pos]='(';str[str_last_pos+1]=0;}
		if (JInput::IsTriggered('0')) {str[str_last_pos]=')';str[str_last_pos+1]=0;}

		if (JInput::IsTriggered(VK_OEM_1))		{str[str_last_pos]=':';str[str_last_pos+1]=0;}
		if (JInput::IsTriggered(VK_OEM_PLUS))	{str[str_last_pos]='+';str[str_last_pos+1]=0;}
		if (JInput::IsTriggered(VK_OEM_COMMA))	{str[str_last_pos]='<';str[str_last_pos+1]=0;}
		if (JInput::IsTriggered(VK_OEM_MINUS))	{str[str_last_pos]='_';str[str_last_pos+1]=0;}
		if (JInput::IsTriggered(VK_OEM_PERIOD))	{str[str_last_pos]='>';str[str_last_pos+1]=0;}
		if (JInput::IsTriggered(VK_OEM_2))		{str[str_last_pos]='?';str[str_last_pos+1]=0;}
		if (JInput::IsTriggered(VK_OEM_3))		{str[str_last_pos]='~';str[str_last_pos+1]=0;}
		if (JInput::IsTriggered(VK_OEM_4))		{str[str_last_pos]='{';str[str_last_pos+1]=0;}
		if (JInput::IsTriggered(VK_OEM_5))		{str[str_last_pos]='|';str[str_last_pos+1]=0;}
		if (JInput::IsTriggered(VK_OEM_6))		{str[str_last_pos]='}';str[str_last_pos+1]=0;}
		if (JInput::IsTriggered(VK_OEM_7))		{str[str_last_pos]='\"';str[str_last_pos+1]=0;}

	}
	else
	{
		if (JInput::IsTriggered(VK_SPACE))		{str[str_last_pos]=' ';str[str_last_pos+1]=0;}
		if (JInput::IsTriggered(VK_OEM_PLUS))	{str[str_last_pos]='=';str[str_last_pos+1]=0;}
		if (JInput::IsTriggered(VK_MULTIPLY))	{str[str_last_pos]='*';str[str_last_pos+1]=0;}
		if (JInput::IsTriggered(VK_ADD))		{str[str_last_pos]='+';str[str_last_pos+1]=0;}
		if (JInput::IsTriggered(VK_SUBTRACT))	{str[str_last_pos]='-';str[str_last_pos+1]=0;}
		if (JInput::IsTriggered(VK_DIVIDE))		{str[str_last_pos]='/';str[str_last_pos+1]=0;}

		if (JInput::IsTriggered(VK_OEM_NEC_EQUAL))	{str[str_last_pos]='=';str[str_last_pos+1]=0;}

		if (JInput::IsTriggered(VK_OEM_1))		{str[str_last_pos]=';';str[str_last_pos+1]=0;}
		if (JInput::IsTriggered(VK_OEM_PLUS))	{str[str_last_pos]='=';str[str_last_pos+1]=0;}
		if (JInput::IsTriggered(VK_OEM_COMMA))	{str[str_last_pos]=',';str[str_last_pos+1]=0;}
		if (JInput::IsTriggered(VK_OEM_MINUS))	{str[str_last_pos]='-';str[str_last_pos+1]=0;}
		if (JInput::IsTriggered(VK_OEM_PERIOD))	{str[str_last_pos]='.';str[str_last_pos+1]=0;}
		if (JInput::IsTriggered(VK_OEM_2))		{str[str_last_pos]='/';str[str_last_pos+1]=0;}
		if (JInput::IsTriggered(VK_OEM_3))		{str[str_last_pos]='`';str[str_last_pos+1]=0;}
		if (JInput::IsTriggered(VK_OEM_4))		{str[str_last_pos]='[';str[str_last_pos+1]=0;}
		if (JInput::IsTriggered(VK_OEM_5))		{str[str_last_pos]='\\';str[str_last_pos+1]=0;}
		if (JInput::IsTriggered(VK_OEM_6))		{str[str_last_pos]=']';str[str_last_pos+1]=0;}
		if (JInput::IsTriggered(VK_OEM_7))		{str[str_last_pos]='\'';str[str_last_pos+1]=0;}
	}

	// °³Çà
	if ((options & MULTI_LINE) == 0) return;

	if (JInput::IsTriggered(VK_RETURN))
	{
		str[str_last_pos] = '\n';
		str[str_last_pos+1] = 0;
	}
}