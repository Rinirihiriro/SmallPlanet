#include "FontManager.h"
#include "MyException.h"
#include "Essential.h"

#include <tchar.h>
#include <algorithm>
#include <fstream>

FontManager* FontManager::obj = NULL;
const JDirectX* FontManager::pDirectx = NULL;

const char* szFontMapFileName = "Manager/FontMap%02d.jfm";

FontManager::FontManager()
{
	LoadFontResource();
}


void FontManager::LoadFontResource()
{
	WIN32_FIND_DATA fd;
	HANDLE handle = FindFirstFile(L"Fonts/*.*", &fd);
	if (handle != INVALID_HANDLE_VALUE)
	{
		do 
		{
			TCHAR font_path[256]={0};
			_stprintf_s(font_path, 256, L"Fonts/%s", fd.cFileName);
			FontResVec.push_back(font_path);
			AddFontResourceEx(font_path, FR_PRIVATE, NULL);
		} while (FindNextFile(handle, &fd));
	}
	FindClose(handle);
}

void FontManager::LoadFontMap(const int mapnum)
{
	/*
		** 폰트 맵 파일 구조
		index_key font_name size bold(0/1) italic(0/1)
	*/
	
	std::string index_key;
	char font_name_buffer[256];
	TCHAR font_name[256];
	int size, bold, italic;

	char filename[256];
	sprintf_s(filename, 256, szFontMapFileName, mapnum);
	std::ifstream ifs(filename);

	if (ifs.is_open())
	{
		while (!ifs.eof())
		{
			// index_key
			ifs>>index_key;
			// 주석행 무시
			if (index_key.substr(0, 2) != "//")
			{
				ZeroMemory(&font_name, sizeof(font_name));
				
				ifs>>font_name_buffer>>size>>bold>>italic;
				MultiByteToWideChar(CP_ACP, 0, font_name_buffer, -1, font_name, 256);
				
				FontMap.insert(StrFontPair(index_key
					, new Font(pDirectx->CreateFont(font_name, size, (bold==0)? FW_NORMAL:FW_BOLD, italic>0))));
			}
			else
			{
				char buf[256];
				ifs.getline(buf, 256);
			}
		}
		ifs.close();
	}
	else
		throw FileNotOpened(filename);
}


FontManager& FontManager::GetInstance()
{
	if (obj==NULL)
	{
		obj = new FontManager();
	}
	return *obj;
}

void FontManager::Release()
{
	SAFE_DELETE(obj);
}

FontManager::~FontManager()
{
	UnLoadFonts();
	for (auto it=FontResVec.begin(); it!=FontResVec.end(); it++)
		RemoveFontResourceEx(it->c_str(), FR_PRIVATE, NULL);
	FontResVec.clear();
}

void FontManager::LoadFonts(const int mapnum)
{
	if (this->map_num == mapnum) return;
	UnLoadFonts();
	this->map_num = mapnum;
	LoadFontMap(mapnum);
}

void FontManager::UnLoadFonts()
{
	if (this->map_num == -1) return;
	this->map_num = -1;

	for (auto it = FontMap.begin(); it!=FontMap.end(); it++) SAFE_DELETE(it->second);
	FontMap.clear();
}


Font& FontManager::GetFont(const std::string key)
{
	return *FontMap[key];
}

int FontManager::GetMapNum() const
{
	return this->map_num;
}


void FontManager::OnLostDevice()
{
	for (auto it = FontMap.begin(); it!=FontMap.end(); it++)
		(*it).second->OnLostDevice();
}

void FontManager::OnResetDevice()
{
	for (auto it = FontMap.begin(); it!=FontMap.end(); it++)
		(*it).second->OnResetDevice();
}


void FontManager::SetCDirectX(const JDirectX* cdirectx)
{
	FontManager::pDirectx = cdirectx;
}