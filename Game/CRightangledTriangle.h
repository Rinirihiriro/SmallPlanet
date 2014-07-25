/*
	충돌처리용 직사각형 구조체
*/
#pragma once

#include <d3dx9.h>

struct CRect;

struct CRightangledTriangle
{
	D3DXVECTOR3 center;	// 삼각형의 직각인 꼭지점
	float width;
	float height;

	CRightangledTriangle( );
	CRightangledTriangle( const float width, const float height );
	CRightangledTriangle( const D3DXVECTOR3 center, const float width, const float height );

	D3DXVECTOR3 realCenter();
	void realCenter(D3DXVECTOR3 center);

	// 충돌은 &연산자로 검출한다.
// 	D3DXVECTOR3 operator&( const CRightangledTriangle& other ) const;
	D3DXVECTOR3 operator&( const CRect& other ) const;
	bool operator&( const D3DXVECTOR3& other ) const;

	operator bool() const;

};