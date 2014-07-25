/*
	게임 내의 예외 슈퍼 클래스입니다.
*/
#pragma once

#include <Windows.h>

class MyException
{
public:
	HRESULT hr;
	LPCTSTR name;
	LPCTSTR message;
	LPCSTR comment;
	LPCTSTR comment_t;

public:
	MyException( HRESULT hr = 0 );
	MyException( LPCTSTR message, HRESULT hr = 0 );
	MyException( LPCTSTR message, LPCSTR comment, HRESULT hr = 0 );
	MyException( LPCTSTR message, LPCTSTR comment, HRESULT hr = 0 );
};

class ObjectWasNotCreated :public MyException
{
public:
	ObjectWasNotCreated( LPCSTR comment, HRESULT hr = 0 )
		:MyException(L"An object was not created :", comment, hr)
	{
		name = L"ObjectWasNotCreated";
	}
	ObjectWasNotCreated( LPCTSTR comment, HRESULT hr = 0 )
		:MyException(L"An object was not created :", comment, hr)
	{
		name = L"ObjectWasNotCreated";
	}
};

class FileNotOpened :public MyException
{
public:
	FileNotOpened( LPCSTR comment, HRESULT hr = 0 )
		:MyException(L"A file was not opened :", comment, hr)
	{
		name = L"FileNotOpened";
	}
	FileNotOpened( LPCTSTR comment, HRESULT hr = 0 )
		:MyException(L"A file was not opened :", comment, hr)
	{
		name = L"FileNotOpened";
	}
};

class FileVersionError :public MyException
{
public:
	FileVersionError( LPCSTR comment, HRESULT hr = 0 )
		:MyException(L"Invalid file version :", comment, hr)
	{
		name = L"FileVersionError";
	}
	FileVersionError( LPCTSTR comment, HRESULT hr = 0 )
		:MyException(L"Invalid file version :", comment, hr)
	{
		name = L"FileVersionError";
	}
};

class NetworkError :public MyException
{
public:
	NetworkError( LPCSTR comment, HRESULT hr = 0 )
		:MyException(L"Network Error :", comment, hr)
	{
		name = L"NetworkError";
	}
	NetworkError( LPCTSTR comment, HRESULT hr = 0 )
		:MyException(L"Network Error :", comment, hr)
	{
		name = L"NetworkError";
	}
};