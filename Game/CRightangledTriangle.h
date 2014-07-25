/*
	�浹ó���� ���簢�� ����ü
*/
#pragma once

#include <d3dx9.h>

struct CRect;

struct CRightangledTriangle
{
	D3DXVECTOR3 center;	// �ﰢ���� ������ ������
	float width;
	float height;

	CRightangledTriangle( );
	CRightangledTriangle( const float width, const float height );
	CRightangledTriangle( const D3DXVECTOR3 center, const float width, const float height );

	D3DXVECTOR3 realCenter();
	void realCenter(D3DXVECTOR3 center);

	// �浹�� &�����ڷ� �����Ѵ�.
// 	D3DXVECTOR3 operator&( const CRightangledTriangle& other ) const;
	D3DXVECTOR3 operator&( const CRect& other ) const;
	bool operator&( const D3DXVECTOR3& other ) const;

	operator bool() const;

};