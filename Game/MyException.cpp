#include "MyException.h"

#ifdef _DEBUG
#include <crtdbg.h>
#endif

MyException::MyException( HRESULT hr )
	:name( L"Exception" ), message( L"Unknown Exception occured." ), comment_t( L"" ), hr(hr)
{
#ifdef _DEBUG
	_RPTFW3(_CRT_ERROR, L"%s // %s // %s", name, message, comment_t);
#endif
}

MyException::MyException( LPCTSTR message, HRESULT hr )
	:name( L"Exception" ), message( message ), comment_t( L"" ), hr(hr)
{
#ifdef _DEBUG
	_RPTFW3(_CRT_ERROR, L"%s // %s // %s", name, message, comment_t);
#endif
}

MyException::MyException( LPCTSTR message, LPCSTR comment, HRESULT hr )
	:name( L"Exception" ), message( message ), comment( comment ), comment_t(NULL), hr(hr)
{
#ifdef _DEBUG
	TCHAR comment_temp[512];
	MultiByteToWideChar(CP_ACP, 0, comment, -1, comment_temp, 256);
	_RPTFW3(_CRT_ERROR, L"%s // %s // %s", name, message, comment_temp);
#endif
}

MyException::MyException( LPCTSTR message, LPCTSTR comment, HRESULT hr )
	:name( L"Exception" ), message( message ), comment_t( comment ), hr(hr)
{
#ifdef _DEBUG
	_RPTFW3(_CRT_ERROR, L"%s // %s // %s", name, message, comment_t);
#endif
}