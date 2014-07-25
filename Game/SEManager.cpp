#include "MyException.h"
#include "SEManager.h"
#include "Essential.h"

#include <fstream>


SEManager* SEManager::obj = NULL;

const char* szSEMapFileName = "Manager/SEMap%02d.jsm";


SEManager::SEManager()
{

}


void SEManager::LoadSEMap(const int mapnum)
{
	/*
		** SE 甘 颇老 备炼
		index_key file_name
	*/
	
	std::string index_key;
	char se_name[256];

	char filename[256];
	sprintf_s(filename, 256, szSEMapFileName, mapnum);
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
				ifs>>se_name;
				SEMap[index_key] = new SE(se_name);
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


SEManager::~SEManager()
{
	UnloadSEs();
}

SEManager& SEManager::GetInstance()
{
	if (obj == NULL)
		obj = new SEManager();
	return *obj;
}

void SEManager::Release()
{
	SAFE_DELETE(obj);
}

// void SEManager::SEMap01()
// {
// // 	SEMap.insert( StrSEPair("player_shoot", new SE( "Sounds/SE/player_shoot.ogg" )) );
// // 	SEMap.insert( StrSEPair("player_hit", new SE( "Sounds/SE/player_hit.ogg" )) );
// // 	SEMap.insert( StrSEPair("bomb", new SE( "Sounds/SE/bomb.ogg" )) );
// // 
// // 	SEMap.insert( StrSEPair("boss_shoot", new SE( "Sounds/SE/boss_shoot.ogg" )) );
// // 	SEMap.insert( StrSEPair("boss_hit", new SE( "Sounds/SE/boss_hit.ogg" )) );
// // 
// // 	SEMap.insert( StrSEPair("panelon", new SE( "Sounds/SE/panelon.ogg" )) );
// // 	SEMap.insert( StrSEPair("paneldown", new SE( "Sounds/SE/paneldown.ogg" )) );
// // 	SEMap.insert( StrSEPair("panelselect", new SE( "Sounds/SE/panelselect.ogg" )) );
// // 
// // 	SEMap.insert( StrSEPair("fade", new SE( "Sounds/SE/fade.ogg" )) );
// // 
// // 	SEMap.insert( StrSEPair("clear", new SE( "Sounds/SE/clear.ogg" )) );
// // 
// // 	SEMap.insert( StrSEPair("result_each1", new SE( "Sounds/SE/result_each1.ogg" )) );
// // 	SEMap.insert( StrSEPair("result_each2", new SE( "Sounds/SE/result_each2.ogg" )) );
// // 	SEMap.insert( StrSEPair("result_each3", new SE( "Sounds/SE/result_each3.ogg" )) );
// // 	SEMap.insert( StrSEPair("result_each4", new SE( "Sounds/SE/result_each4.ogg" )) );
// // 
// // 	SEMap.insert( StrSEPair("result_end1", new SE( "Sounds/SE/result_end1.ogg" )) );
// // 	SEMap.insert( StrSEPair("result_end2", new SE( "Sounds/SE/result_end2.ogg" )) );
// // 	SEMap.insert( StrSEPair("result_end3", new SE( "Sounds/SE/result_end3.ogg" )) );
// }

void SEManager::LoadSEs( const int mapnum )
{
	if (this->map_num == mapnum) return;
	UnloadSEs();
	this->map_num = mapnum;
	LoadSEMap(mapnum);
}

void SEManager::UnloadSEs()
{
	if (this->map_num == -1) return;

	this->map_num = -1;
	for (std::map<std::string, SE*>::iterator it = SEMap.begin(); it!=SEMap.end(); it++) SAFE_DELETE((*it).second);
	SEMap.clear();
}

void SEManager::PlaySE( const std::string index, const bool play_if_stop )
{
	if (!SEMap[index]) return;
	if (play_if_stop)
		SEMap[index]->PlayIfStop();
	else
		SEMap[index]->Play();
}

void SEManager::StopSE( const std::string index )
{
	if (!SEMap[index]) return;
	SEMap[index]->Stop();
}

void SEManager::PauseSE( const std::string index )
{
	if (!SEMap[index]) return;
	SEMap[index]->Pause();
}

void SEManager::ResumeSE( const std::string index )
{
	if (!SEMap[index]) return;
	SEMap[index]->Resume();
}

int SEManager::GetMapNum() const
{
	return this->map_num;
}