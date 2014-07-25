/*
	�浹 ó���� ���� ���� ����ü
*/
#pragma once

#include <d3dx9.h>

struct CCircle
{

	D3DXVECTOR3 pos;
	float radius;

	CCircle();
	CCircle( const D3DXVECTOR3 pos, const float radius );

	// �浹�� &�����ڷ� �����Ѵ�.
	bool operator&( const CCircle& other ) const;
	bool operator&( const D3DXVECTOR3& other) const;

	operator bool() const;
};