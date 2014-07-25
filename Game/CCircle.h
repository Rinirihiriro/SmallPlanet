/*
	충돌 처리용 원에 대한 구조체
*/
#pragma once

#include <d3dx9.h>

struct CCircle
{

	D3DXVECTOR3 pos;
	float radius;

	CCircle();
	CCircle( const D3DXVECTOR3 pos, const float radius );

	// 충돌은 &연산자로 검출한다.
	bool operator&( const CCircle& other ) const;
	bool operator&( const D3DXVECTOR3& other) const;

	operator bool() const;
};