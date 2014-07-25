#pragma once

#include <Windows.h>
#include <ft2build.h>
#include FT_FREETYPE_H

class JFreeType
{
private:
	FT_Library ftLibrary;

private:
	void Initialize();
	void CleanUp();

public:
	JFreeType();
	~JFreeType();

	void CreateFont(LPCSTR fontPath, const long faceIndex, FT_Face* out);
};