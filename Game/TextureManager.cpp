#include <stdio.h>
#include <fstream>
#include <algorithm>

#include "MyException.h"
#include "Essential.h"
// #include "CriticalSection.h"

#include "TextureManager.h"

TextureManager* TextureManager::obj = NULL;

// CRITICAL_SECTION TextureManager::cs;
// bool TextureManager::bThreadEnd = false;

const char* szTextureMapFileName = "Manager/TextureMap%02d.jtm";

/*
DWORD WINAPI TextureManager::LoadingThreadProc( LPVOID lpParameter )
{
	while (true)
	{
		{
			CriticalSection cst(&cs);
			// 로드
			for each (int id in obj->vLoadingMap)
				obj->LoadTextureMap(id);

			// 언로드
			for each (int id in obj->vUnloadingMap)
			{
				obj->vMapNum.erase(std::find(obj->vMapNum.begin(), obj->vMapNum.end(), id));
				std::vector<std::map<std::string, TextureData>::iterator> erasevec;
				for (auto it = obj->TextureMap.begin(); it != obj->TextureMap.end(); it++)
				{
					if (it->second.iMapNum == id)
					{
						SAFE_DELETE(it->second.txTexture);
						erasevec.push_back(it);
					}
				}

				while (!erasevec.empty())
				{
					obj->TextureMap.erase(erasevec.back());
					erasevec.pop_back();
				}
			}
		}
		{
			CriticalSection cst(&cs);
			if (bThreadEnd)
				break;
		}
		Sleep(1);
	}

	return 0;
}
*/

TextureManager::TextureManager()
{
	D3DXVECTOR3 center(0,0,0);
	txBlank = new Texture(800, 600, D3DFMT_X8R8G8B8, 0, 0, &center);
	txBlank->ClearTexture(0xffffffff);

// 	LoadingThread = CreateThread(0, 0, LoadingThreadProc, 0, 0, 0);
// 	InitializeCriticalSection(&cs);
}


void TextureManager::LoadTextureMap(const int mapnum)
{
	/*
		** 텍스처 맵 파일 구조
		index_key file_path center_x center_y v_frame h_frame color_key
	*/
	
	std::string index_key;
	char file_path_buffer[256];
	TCHAR file_path[256];
	D3DXVECTOR3 center;
	UINT vframe, hframe;
	DWORD color_key;

	char filename[256];
	sprintf_s(filename, 256, szTextureMapFileName, mapnum);
	std::ifstream ifs(filename);

	if (ifs.is_open())
	{
		while (!ifs.eof())
		{
			// index_key
			ifs>>index_key;
			if (ifs.eof()) break;
			// 주석행 무시
			if (index_key.substr(0, 2) != "//")
			{
				ZeroMemory(&file_path, sizeof(file_path));
				ifs>>file_path_buffer>>center.x>>center.y>>vframe>>hframe>>color_key;
				center.z = 0;
				MultiByteToWideChar(CP_ACP, 0, file_path_buffer, -1, file_path, 256);
				Texture* tx = new Texture(file_path, (center.x==-1&&center.y==-1)? NULL:&center, vframe, hframe, color_key);
				if (TextureMap[index_key].txTexture)	// 이미 로드됐다면
					SAFE_DELETE(TextureMap[index_key].txTexture);
				TextureMap[index_key] = TextureData(tx, mapnum);
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


TextureManager::~TextureManager()
{
/*
	{
		CriticalSection cst(&cs);
		bThreadEnd = true;
	}
	while (true)
	{
		DWORD exitcode;
		GetExitCodeThread(LoadingThread, &exitcode);
		if (exitcode != STILL_ACTIVE) break;
		Sleep(1);
	}
*/
	SAFE_DELETE(txBlank);
	UnloadAllTextures();
}

TextureManager& TextureManager::GetInstance()
{
	if (obj == NULL)
	{
		obj = new TextureManager();
	}
	return *obj;
}

void TextureManager::Release()
{
	SAFE_DELETE(obj);
}

void TextureManager::LoadTextures( const int mapnum )
{
	if (IsLoadedMap(mapnum)) return;
//	CriticalSection cst(&cs);
	vMapNum.push_back(mapnum);
//	vLoadingMap.push_back(mapnum);
	LoadTextureMap(mapnum);
}

void TextureManager::UnloadTextures( const int mapnum )
{
	if (!IsLoadedMap(mapnum)) return;
//	CriticalSection cst(&cs);
//	vUnloadingMap.push_back(mapnum);

	vMapNum.erase(std::find(vMapNum.begin(), vMapNum.end(), mapnum));
	std::vector<std::map<std::string, TextureData>::iterator> erasevec;
	for (std::map<std::string, TextureData>::iterator it = TextureMap.begin()
		; it != TextureMap.end(); it++)
	{
		if (it->second.iMapNum == mapnum)
		{
			SAFE_DELETE(it->second.txTexture);
			erasevec.push_back(it);
		}
	}

	while (!erasevec.empty())
	{
		TextureMap.erase(erasevec.back());
		erasevec.pop_back();
	}

}

void TextureManager::UnloadAllTextures()
{
	if (vMapNum.empty()) return;
	vMapNum.clear();
	for each (std::pair<std::string, TextureData> p in TextureMap)
		SAFE_DELETE(p.second.txTexture);
	TextureMap.clear();
}

Texture* TextureManager::GetTexture( const std::string key )
{
	if (key=="null")
		return NULL;
	return TextureMap[key].txTexture;
}

Texture* TextureManager::GetBlank() const
{
	return txBlank;
}

std::vector<int> TextureManager::GetTextureMapIds() const
{
	return vMapNum;
}


bool TextureManager::IsLoadedMap(const int mapnum) const
{
	// CriticalSection cst(&cs);
	return std::find(vMapNum.begin(), vMapNum.end(), mapnum) != vMapNum.end();
}

/*
bool TextureManager::IsLoading() const
{
	CriticalSection cst(&cs);
	return !vLoadingMap.empty() && !vUnloadingMap.empty();
}
*/