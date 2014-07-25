#pragma once

#include <d3dx9math.h>
#define BLOCK_SIZE 16

enum BlockType {
	BLOCK_AIR
	, BLOCK_DIRT
	, BLOCK_WATER
	, BLOCK_ROCK
	, BLOCK_MINERAL
	, BLOCK_COAL
	, BLOCK_OIL
	, BLOCK_URANIUM
	, BLOCK_UNOBTAINIUM
	, BLOCK_COOKIE
	, BLOCK_BEDROCK
	, BLOCK_OBSIDIAN
	, BLOCK_MAGMA
	, BLOCK_UNOBTAINIUM_PLATFORM
	, BLOCK_COOKIE_PLATFORM
	, BLOCK_TYPE_NUM
};

enum ResourceType {
	RES_COAL
	, RES_OIL
	, RES_URANIUM
	, RES_HYDROGEN
	, RES_UNOPTAINIUM
	, RES_COOKIE
	, RES_TYPE_NUM
};

enum UpgradeType {
	UPGRADE_MAX_HEALTH
	, UPGRADE_MAX_OXYGEN
	, UPGRADE_CARGO_CAPACITY
	, UPGRADE_HEALTH_RECOVERY
	, UPGRADE_OXYGEN_RECOVERY
	, UPGRADE_JETPACK_FUEL_TANK
	, UPGRADE_MAGMA_PROOF
	, UPGRADE_DIRLL_EFFICENCY
	, UPGRADE_FUEL_EFFICENCY
	, UPGRADE_NUM
};

enum BuildingType {
	BUILDING_NULL
	, BUILDING_LIGHT
	, BUILDING_POWER_WIND
	, BUILDING_POWER_HEAT
	, BUILDING_SMALL_CAMP
};

enum NumberFontMap {
	NUMBER_DOT = 11
	, NUMBER_COMMA
	, NUMBER_PLUS
	, NUMBER_MINUS
	, NUMBER_PLUSMINUS
	, NUMBER_SLASH
	, NUMBER_S
	, NUMBER_M
	, NUMBER_B
	, NUMBER_T
	, NUMBER_Q

	, BIG_NUMBER_DOT = 10
	, BIG_NUMBER_COMMA
	, BIG_NUMBER_X
};

enum FontMap {
	FONT_LARGE_A = 0
	, FONT_SMALL_A = 26
	, FONT_DOT = 52
	, FONT_COMMA
	, FONT_EXCLAMATION
	, FONT_QUESTION
	, FONT_LT
	, FONT_GT
	, FONT_NUMBER_0 = 60
};

struct BlockData {
	char* name;
	int durability;
	bool fluid;
	bool collectable;
	int particle_num;

	int fuel_rate;
	int mineral_rate;
	int processing_power;
	int processing_speed;

	int research_fuel;
	int research_mineral;
};
struct ResourceData {
	int research_fuel;
	int research_mineral;
	int fuel_rate;
	int processing_power;
	float processing_speed;
};
struct BuildingData {
	char* name;
	int research_fuel;
	int research_mineral;
	int fuel;
	int mineral;
	int power_per_sec;
	int width;
	int height;
	bool hover;
};
struct Upgrade {
	int effect;	// 10 = 1.0, 15 = 1.5, 20 = 2.0 ...
	int fuel;
	int mineral;
};
struct Position {
	int x;
	int y;
	Position():x(0), y(0){}
	Position(const int x, const int y):x(x), y(y){}
};

extern const int gGoal;
extern const BlockData gBlockData[];
extern const int gResourceBlock[];
extern const BuildingData gBuildingData[];
extern const Upgrade gUpgrades[][8];	// 시작은 언제나 {10, 0, 0}, 끝은 언제나 {0, 0, 0}
extern const unsigned char gNumberFontWidth[];
extern const unsigned char gBigNumberFontWidth[];
extern const unsigned char gFontWidth[];

int CharToFontIndex(char c);
void DrawFont(const float x, const float y, const char* const string, const D3DXCOLOR* color = NULL);
int GetFontLength(const char* const string);
