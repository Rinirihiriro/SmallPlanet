#include "MyMath.h"

int MyMath::abs( const int num )
{
	return (num<0)? -num:num;
}

float MyMath::abs( const float num )
{
	return (num<0.0f)? -num:num;
}

float MyMath::pow( const float a, const int b )
{
	float n = 1.0f;
	int ab = abs(b);
	for(int i = 0; i<ab; i++) n*=a;
	return (b<0.0f)? 1/n:n;
}

int MyMath::numlen( const int num )
{
	if (num == 0) return 1;
	int tempnum = abs(num);
	int len = 0;
	while (tempnum>0)
	{
		len++;
		tempnum/=10;
	}
	return len;
}