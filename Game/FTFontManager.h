#pragma once

#include <string>
#include "FTFont.h"

typedef std::pair<std::string, FTFont*> StrFTFontPair;

class FTFontManager
{
private:
	static FTFontManager* obj;
	std::map<std::string, FTFont*> FontMap;
	int map_num;

private:
	FTFontManager();

	void LoadFontMap(const int mapnum);

public:
	static FTFontManager& GetInstance();
	static void Release();
	~FTFontManager();

	void LoadFonts(const int mapnum);
	void UnLoadFonts();

	FTFont& GetFont(const std::string key);
	int GetMapNum() const;
};