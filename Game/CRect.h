/*
	�浹ó���� �簢�� ����ü
*/
#pragma once

#include <d3dx9.h>

struct CRightangledTriangle;

struct CRect
{
	D3DXVECTOR3 center;
	float width;
	float height;

	CRect( );
	CRect( const float width, const float height );
	CRect( const D3DXVECTOR3 center, const float width, const float height );

	// �浹�� &�����ڷ� �����Ѵ�.
	D3DXVECTOR3 operator&( const CRect& other ) const;
	D3DXVECTOR3 operator&( const CRightangledTriangle& other ) const;
	bool operator&( const D3DXVECTOR3& other ) const;
	
	operator bool() const;

};