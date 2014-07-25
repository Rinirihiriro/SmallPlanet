/*
	단순한 수학 연산용 정적 클래스입니다.
*/
#pragma once

class MyMath
{
public:
	static int abs( const int num );
	static float abs( const float num );
	static float pow( const float a, const int b );
	static int numlen( const int num );
};