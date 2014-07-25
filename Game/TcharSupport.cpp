#include <cstring>
#include "TcharSupport.h"
#include "MyMath.h"

int TCHARtoInt(const TCHAR* str)
{
	if (!IsNumber(str)) return -1;
	int result=0;
	int _10 = 1;
	int len = _tcslen(str);
	int sign = (str[0]=='-')? -1:1;

	for (int i=0; i<len; i++)
	{
		if (sign<0 && i == len-1) break;
		result += (str[len-i-1]-'0')*_10*sign;
		_10 *= 10;
	}

	return result;
}

int CharToInt(const char* str)
{
	if (!IsNumber(str)) return -1;
	int result=0;
	int _10 = 1;
	int len = strlen(str);
	int sign = (str[0]=='-')? -1:1;

	for (int i=0; i<len; i++)
	{
		if (sign<0 && i == len-1) break;
		result += (str[len-i-1]-'0')*_10*sign;
		_10 *= 10;
	}

	return result;
}

void IntToTCHAR(const int num, TCHAR* out)
{
	int index = 0;
	int number = num;
	if (num<0)
	{
		out[index++] = '-';
		number = -number;
	}
	int _10 = (int)MyMath::pow(10, MyMath::numlen(num)-1);
	while (_10 != 0)
	{
		out[index++] = number/_10%10 + '0';
		_10 /= 10;
	}
	out[index] = 0;
}

void IntToChar(const int num, char* out)
{
	int index = 0;
	int number = num;
	if (num<0)
	{
		out[index++] = '-';
		number = -number;
	}
	int _10 = (int)MyMath::pow(10, MyMath::numlen(num)-1);
	while (_10 != 0)
	{
		out[index++] = number/_10%10 + '0';
		_10 /= 10;
	}
	out[index] = 0;
}

bool IsNumber(const TCHAR* str)
{
	int len = (int)_tcslen(str);
	if (len==0)
	{
		return false;
	}
	for (int i=0; i<len; i++)
	{
		if (i==0 && str[i]=='-' && len>1)
		{
			continue;
		}
		if (str[i] < '0' || str[i] > '9')
		{
			return false;
		}
	}
	return true;
}

bool IsNumber(const char* str)
{
	int len = (int)strlen(str);
	if (len==0)
	{
		return false;
	}
	for (int i=0; i<len; i++)
	{
		if (i==0 && str[i]=='-' && len>1)
		{
			continue;
		}
		if (str[i] < '0' || str[i] > '9')
		{
			return false;
		}
	}
	return true;
}

void ToFileName( TCHAR* str )
{
	int len = _tcslen(str)+1;
	for (int i=0; i<len; i++)
	{
		if (str[i] == ' ')
		{
			str[i] = '_';
		}
		else if (				// 파일명에 들어갈 수 없는 기호들
			str[i] == '\\'
			|| str[i] == '/'
			|| str[i] == ':'
			|| str[i] == '*'
			|| str[i] == '?'
			|| str[i] == '\"'
			|| str[i] == '<'
			|| str[i] == '>'
			|| str[i] == '|'
			)
		{
			str[i] = '_';
		}
	}
}

void ToFileName( char* str )
{
	int len = strlen(str);
	for (int i=0; i<len; i++)
	{
		if (str[i] == ' ')
		{
			str[i] = '_';
		}
		else if (				// 파일명에 들어갈 수 없는 기호들
			str[i] == '\\'
			|| str[i] == '/'
			|| str[i] == ':'
			|| str[i] == '*'
			|| str[i] == '?'
			|| str[i] == '\"'
			|| str[i] == '<'
			|| str[i] == '>'
			|| str[i] == '|'
			)
		{
			str[i] = '_';
		}
	}
}

// 메모리 누수 주의
int SeperateBySpace(TCHAR* str, TCHAR** output_arr)
{
	int total_len = _tcslen(str);
	int part_len = 0;
	int arr_size = 0;
	TCHAR* part_str = NULL;

	for (int i=0; i<total_len; i++)
	{
		if (str[i] == ' ' || str[i] == '\t')
		{
			if (part_len != 0)
			{
				part_str = new TCHAR[part_len+1];
				for (int j=0; j<part_len; j++)
					part_str[j] = str[i-part_len+j];
				part_str[part_len] = 0;
				output_arr[arr_size++] = part_str;
				part_len = 0;
			}
		}
		else
		{
			part_len += 1;
		}
	}

	if (part_len != 0)
	{
		part_str = new TCHAR[part_len+1];
		for (int j=0; j<part_len; j++)
			part_str[j] = str[total_len-part_len+j];
		part_str[part_len] = 0;
		output_arr[arr_size++] = part_str;
	}

	return arr_size;
}


void TrimSpaces(TCHAR* str)
{
	int len = _tcslen(str);
	// 앞
	while (str[0] == ' ' || str[0] == '\t')
	{
		for (int i = 0; i<len-1; i++) str[i] = str[i+1];
		len-=1;
		str[len] = 0;
	}
	// 뒤
	while (str[len-1] == ' ' || str[len-1] == '\t')
	{
		str[len-1] = 0;
		len-=1;
	}
}