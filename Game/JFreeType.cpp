#include "JFreeType.h"

#include "MyException.h"

#define FT_FAILED( x ) ( x != 0 )
#define FT_SUCCEEDED( x ) ( x == 0 )

JFreeType::JFreeType()
{
	Initialize();
}

JFreeType::~JFreeType()
{
	CleanUp();
}


void JFreeType::Initialize()
{
	int result;
	result = FT_Init_FreeType(&ftLibrary);
	if (FT_FAILED(result))
	{
		throw ObjectWasNotCreated(L"FreeType Library", result);
	}
}

void JFreeType::CleanUp()
{
	FT_Done_FreeType(ftLibrary);
}


void JFreeType::CreateFont(LPCSTR fontPath, const long faceIndex, FT_Face* out)
{
	int result;
	result = FT_New_Face(ftLibrary, fontPath, faceIndex, out);
	if (FT_FAILED(result))
	{
		throw ObjectWasNotCreated(fontPath, result);
	}
}