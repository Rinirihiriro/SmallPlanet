#include "MyException.h"
#include "CriticalSection.h"

#include "BGMManager.h"
#include "Essential.h"

#include <fstream>

BGMManager* BGMManager::obj = NULL;

CRITICAL_SECTION BGMManager::cs;
bool BGMManager::bThreadEnd = false;

const char* szBGMMapFileName = "Manager/BGMMap%02d.jbm";


DWORD WINAPI BGMManager::StreamingThreadProc( LPVOID lpParameter )
{
	while (true)
	{
// 		try
		{
			CriticalSection cst(&cs);
			for each (auto it in obj->BGMMap)
			{
				it.second->Streaming();
				it.second->Update();
			}
 		}
// 		catch (MyException e)
// 		{
// 			throw e;
// 		}
		{
			CriticalSection cst(&cs);
			if (bThreadEnd)
			{
				break;
			}
		}
		Sleep(1);
	}

	return 0;
}

BGMManager::BGMManager()
{
	this->map_num = -1;
	StreamingThread = CreateThread(0, 0, StreamingThreadProc, 0, 0, 0);
	InitializeCriticalSection(&cs);
}


void BGMManager::LoadBGMMap(const int mapnum)
{
	/*
		** BGM 甘 颇老 备炼
		index_key file_name loop(0/1) loopback_point
	*/
	
	std::string index_key;
	char bgm_name[256];
	int loop, loopback_point;

	char filename[256];
	sprintf_s(filename, 256, szBGMMapFileName, mapnum);
	std::ifstream ifs(filename);

	if (ifs.is_open())
	{
		while (true)
		{
			// index_key
			ifs>>index_key;
			if (ifs.eof()) break;
			// 林籍青 公矫
			if (index_key.substr(0, 2) != "//")
			{
				ifs>>bgm_name>>loop>>loopback_point;
				BGMMap.insert(StrBGMPair(index_key, new BGM(bgm_name, loop>0, loopback_point)));
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


BGMManager::~BGMManager()
{
	{
		CriticalSection cst(&cs);
		bThreadEnd = true;
	}
	while (true)
	{
		DWORD exitcode;
		GetExitCodeThread(StreamingThread, &exitcode);
		if (exitcode != STILL_ACTIVE) break;
		Sleep(1);
	}
	UnloadBGMs();
	DeleteCriticalSection(&cs);
}

BGMManager& BGMManager::GetInstance()
{
	if (obj == NULL)
	{
		obj = new BGMManager();
	}
	return *obj;
}

void BGMManager::Release()
{
	SAFE_DELETE(obj);
}


void BGMManager::LoadBGMs( const int mapnum )
{
	CriticalSection cst(&cs);
	if (this->map_num == mapnum) return;
	UnloadBGMs();
	this->map_num = mapnum;
	LoadBGMMap(mapnum);
}

void BGMManager::UnloadBGMs()
{
	if (this->map_num == -1) return;

	CriticalSection cst(&cs);
	for (std::map<std::string, BGM*>::iterator it = BGMMap.begin(); it!=BGMMap.end(); it++) SAFE_DELETE((*it).second);
	BGMMap.clear();
	this->map_num = -1;
}

// void BGMManager::UpdateBGMs()
// {
// // 	try
// // 	{
// 		CriticalSection crt(&cs);
// 		std::map<std::string, BGM*>::iterator it;
// 		for(it=obj->BGMMap.begin(); it!=obj->BGMMap.end(); it++)
// 		{
// 			it->second->Update();
// 		}
// // 	}
// // 	catch (MyException e)
// // 	{
// // 		throw e;
// // 	}
// }

void BGMManager::PlayBGM( const std::string index )
{
// 	try
// 	{
		CriticalSection crt(&cs);
		BGMMap[index]->Play();
		BGMMap[index]->SetVolume(1);
// 	}
// 	catch (MyException e)
// 	{
// 		throw e;
// 	}
}

void BGMManager::StopBGM( const std::string index )
{
// 	try
// 	{
		CriticalSection crt(&cs);
		BGMMap[index]->Stop();
// 	}
// 	catch (MyException e)
// 	{
// 		throw e;
// 	}
}

void BGMManager::PauseBGM( const std::string index )
{
// 	try
// 	{
		CriticalSection crt(&cs);
		BGMMap[index]->Pause();
// 	}
// 	catch (MyException e)
// 	{
// 		throw e;
// 	}
}

void BGMManager::ResumeBGM( const std::string index )
{
// 	try
// 	{
		CriticalSection crt(&cs);
		BGMMap[index]->Resume();
// 	}
// 	catch (MyException e)
// 	{
// 		throw e;
// 	}
}

void BGMManager::PlaySingleBGM( const std::string index )
{
// 	try
// 	{
		CriticalSection crt(&cs);
		StopAllBGMs();
		BGMMap[index]->Play();
// 	}
// 	catch (MyException e)
// 	{
// 		throw e;
// 	}
}

void BGMManager::StopAllBGMs()
{
// 	try
// 	{
		CriticalSection crt(&cs);
		std::map<std::string, BGM*>::iterator it;
		for(it=obj->BGMMap.begin(); it!=obj->BGMMap.end(); it++)
		{
			it->second->Stop();
		}
// 	}
// 	catch (MyException e)
// 	{
// 		throw e;
// 	}
}

void BGMManager::ResumePausingBGMs()
{
// 	try
// 	{
		CriticalSection crt(&cs);
		std::map<std::string, BGM*>::iterator it;
		for(it=obj->BGMMap.begin(); it!=obj->BGMMap.end(); it++)
		{
			if (it->second->IsPausing())
				it->second->Resume();
		}
// 	}
// 	catch (MyException e)
// 	{
// 		throw e;
// 	}
}

void BGMManager::FadeInBGM( const std::string index, const int frame )
{
// 	try
// 	{
		CriticalSection crt(&cs);
		BGMMap[index]->FadeIn(frame);
// 	}
// 	catch (MyException e)
// 	{
// 		throw e;
// 	}
}

void BGMManager::FadeInAllBGMs( const int frame )
{
// 	try
// 	{
		CriticalSection crt(&cs);
		for each (auto it in BGMMap)
		{
			if (it.second->IsPlaying())
			it.second->FadeIn(frame);
		}
// 	}
// 	catch (MyException e)
// 	{
// 		throw e;
// 	}
}

void BGMManager::FadeOutBGM( const std::string index, const int frame )
{
// 	try
// 	{
		CriticalSection crt(&cs);
		BGMMap[index]->FadeOut(frame);
// 	}
// 	catch (MyException e)
// 	{
// 		throw e;
// 	}
}

void BGMManager::FadeOutAllBGMs( const int frame )
{
// 	try
// 	{
		CriticalSection crt(&cs);
		std::map<std::string, BGM*>::iterator it;
		for(it=obj->BGMMap.begin(); it!=obj->BGMMap.end(); it++)
		{
			it->second->FadeOut(frame);
		}
// 	}
// 	catch (MyException e)
// 	{
// 		throw e;
// 	}
}

void BGMManager::FadeOutBGMAndStop( const std::string index, const int frame )
{
// 	try
// 	{
		CriticalSection crt(&cs);
		BGMMap[index]->FadeOutAndStop(frame);
// 	}
// 	catch (MyException e)
// 	{
// 		throw e;
// 	}
}

void BGMManager::FadeOutAllBGMsAndStop( const int frame )
{
// 	try
// 	{
		CriticalSection crt(&cs);
		std::map<std::string, BGM*>::iterator it;
		for(it=obj->BGMMap.begin(); it!=obj->BGMMap.end(); it++)
		{
			it->second->FadeOutAndStop(frame);
		}
// 	}
// 	catch (MyException e)
// 	{
// 		throw e;
// 	}
}

void BGMManager::SetSingleBGMLoop( const std::string index, const bool loop )
{
// 	try
// 	{
		CriticalSection crt(&cs);
		BGMMap[index]->SetLoop( loop );
// 	}
// 	catch (MyException e)
// 	{
// 		throw e;
// 	}
}

void BGMManager::SetBGMsLoop( const bool loop )
{
// 	try
// 	{
		CriticalSection crt(&cs);
		std::map<std::string, BGM*>::iterator it;
		for(it=obj->BGMMap.begin(); it!=obj->BGMMap.end(); it++)
		{
			it->second->SetLoop(loop);
		}
// 	}
// 	catch (MyException e)
// 	{
// 		throw e;
// 	}
}

void BGMManager::SetSingleBGMVolume( const std::string index, const float volume )
{
// 	try
// 	{
		CriticalSection crt(&cs);
		BGMMap[index]->SetVolume( volume );
// 	}
// 	catch (MyException e)
// 	{
// 		throw e;
// 	}
}

void BGMManager::SetBGMsVolume( const float volume )
{
// 	try
// 	{
		CriticalSection crt(&cs);
		std::map<std::string, BGM*>::iterator it;
		for(it=obj->BGMMap.begin(); it!=obj->BGMMap.end(); it++)
		{
			it->second->SetVolume(volume);
		}
// 	}
// 	catch (MyException e)
// 	{
// 		throw e;
// 	}
}

float BGMManager::GetBGMVolume( const std::string index )
{
// 	try
// 	{
		CriticalSection crt(&cs);
		return BGMMap[index]->GetVolume();
// 	}
// 	catch (MyException e)
// 	{
// 		throw e;
// 	}
}

bool BGMManager::IsBGMLoop( const std::string index )
{
// 	try
// 	{
		CriticalSection crt(&cs);
		return BGMMap[index]->IsLoop();
// 	}
// 	catch (MyException e)
// 	{
// 		throw e;
// 	}
}

bool BGMManager::IsSingleBGMPlaying( const std::string index )
{
// 	try
// 	{
		CriticalSection crt(&cs);
		return BGMMap[index]->IsPlaying();
// 	}
// 	catch (MyException e)
// 	{
// 		throw e;
// 	}
}

bool BGMManager::IsBGMPlaying()
{
// 	try
// 	{
		CriticalSection crt(&cs);
		std::map<std::string, BGM*>::iterator it;
		for(it=obj->BGMMap.begin(); it!=obj->BGMMap.end(); it++)
		{
			if (it->second->IsPlaying()) return true;
		}
		return false;
// 	}
// 	catch (MyException e)
// 	{
// 		throw e;
// 	}
}

bool BGMManager::IsBGMPausing( const std::string index )
{
// 	try
// 	{
		CriticalSection crt(&cs);
		return BGMMap[index]->IsPausing();
// 	}
// 	catch (MyException e)
// 	{
// 		throw e;
// 	}
}


std::vector<std::string> BGMManager::GetPlayingBGMs()
{
	CriticalSection crt(&cs);
	std::vector<std::string> out;
	std::map<std::string, BGM*>::iterator it;
	for each(auto it in BGMMap)
	{
		if (it.second->IsPlaying())
			out.push_back(it.first);
	}
	if (out.empty())
		out.push_back("null");
	return out;
}


int BGMManager::GetMapNum() const{ return this->map_num; };
