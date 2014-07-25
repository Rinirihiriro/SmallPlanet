#include "GameData.h"
#include "TextureManager.h"

const int gGoal = 30000;


const BlockData gBlockData[] = {
	  {"Air"		, 0		, false	, false	, 0}
	, {"Dirt"		, 3		, false	, false	, 2}
	, {"Water"		, 1		, true	, true	, 5	, 10	, 0		, 100	, 128	, 7500	, 1000}
	, {"Rock"		, 10	, false	, false	, 2}
	, {"Mineral"	, 15	, false	, true	, 3	, 0		, 1}
	, {"Coal"		, 10	, false	, true	, 3	, 2		, 0		, 0		, 32	, 0		, 0}
	, {"Oil"		, 1		, true	, true	, 5	, 7		, 0		, 1		, 32	, 250	, 100}
	, {"Uranium"	, 75	, false	, true	, 2	, 100	, 0		, 25	, 4		, 1000	, 250}
	, {"Unobtainium", 256	, false	, true	, 1	, 500	, 0		, 250	, 2		, 10000	, 2500}
	, {"Cookie"		, 1		, false	, true	, 5	, 12345	, 0		, 999	, 1		, 20000	, 5000}
	, {"Bedrock"	, 0		, false	, false	, 2}
	, {"Obsidian"	, 1000	, false	, true	, 2	, 0		, 64}
	, {"Magma"		, 0		, true	, false	, 2}
	, {"Unobtainium", 0		, false	, false	, 2}
	, {"Unobtainium", 0		, false	, false}
	, {"Cookie"		, 0		, false	, false}
};

const int gResourceBlock[] = {
	BLOCK_COAL, BLOCK_OIL, BLOCK_URANIUM, BLOCK_WATER, BLOCK_UNOBTAINIUM, BLOCK_COOKIE
};


const BuildingData gBuildingData[] = {
	{"NULL"}
	, {"Light"					, 100	, 100	, 10	, 10	, -1	, 1	, 1	, true}
	, {"Wind Power Plant"		, 250	, 250	, 25	, 50	, 3		, 2	, 3	, false}
	, {"Geothermal Power Plant"	, 250	, 250	, 25	, 50	, 2		, 1	, 1	, false}
	, {"Advanced Camp"			, 1500	, 750	, 250	, 250	, -25	, 3	, 2	, false}
};


const Upgrade gUpgrades[][8] = {
	{
		// Max Health
		 {	10,	0,	0}
		,{	15,	100,50	}
		,{	20,	450,200	}
		,{	40,	1000,600}
	}
	, {
		// Max Oxygen
		{	10,	0,	0}
		,{	15,	120,60	}
		,{	20,	500,250	}
		,{	40,	1000,750}
	}
	, {
		// Cargo Capacity
		{	10,	0,	0}
		,{	15,	150,75	}
		,{	20,	600,300	}
		,{	40,	1200,900}
	}
	, {
		// Health Recovery
		{	10,	0,	0}
		,{	20,	300,300	}
		,{	40,	600,600	}
	}
	, {
		// Oxygen Recovery
		{	10,	0,	0}
		,{	20,	400,300	}
		,{	40,	800,600	}
	}
	, {
		// Jetpack Fuel Tank
		{	10,	0,	0}
		,{	15,	110,55	}
		,{	20,	550,250	}
		,{	40,	1100,600}
	}
	, {
		// Magma-proof
		{	10,	0,	0}
		,{	5,	250,500	}
		,{	1,	700,1000}
	}
	, {
		// Drill Efficency
		{	10,	0,	0}
		,{	30,	500,500	}
		,{	50,	2500,3000}
		,{	70,	7500,9000}
		,{	0}
		,{	320, 0, 0}
	}
	, {
		// Fuel Efficency
		{	10,	0,	0}
		,{	15,	5000, 4000}
		,{	20,	10000,7500}
	}
};

const unsigned char gNumberFontWidth[] = {
	4, 2, 4, 4, 4, 4, 4, 4, 4, 4, 0
	, 1, 1, 3, 3, 3, 3, 4, 5, 4, 5, 5
};

const unsigned char gBigNumberFontWidth[] = {
	8, 6, 8, 8, 8, 8, 8, 8, 8, 8, 2, 2, 7
};

const unsigned char gFontWidth[] = {
	  4, 4, 4, 4, 4, 4, 4, 4, 3, 4
	, 4, 4, 5, 4, 4, 4, 5, 4, 4, 5
	, 4, 5, 5, 5, 5, 5, 3, 3, 3, 3
	, 3, 3, 3, 3, 1, 2, 3, 3, 5, 3
	, 3, 3, 3, 3, 3, 3, 3, 3, 5, 3
	, 3, 3, 1, 1, 1, 3, 4, 4, 0, 0
	, 4, 2, 4, 4, 5, 4, 4, 4, 4, 4
};

int CharToFontIndex(char c)
{
	if (c>='A' && c<='Z')
		return FONT_LARGE_A+c-'A';
	else if (c>='a' && c<='z')
		return FONT_SMALL_A+c-'a';
	else if (c>='0' && c<='9')
		return FONT_NUMBER_0+c-'0';
	switch (c)
	{
	case '.': return FONT_DOT;
	case ',': return FONT_COMMA;
	case '!': return FONT_EXCLAMATION;
	case '?': return FONT_QUESTION;
	case '<': return FONT_LT;
	case '>': return FONT_GT;
	}
	return -1;
}

void DrawFont(const float x, const float y, const char* const string, const D3DXCOLOR* color)
{
	Texture* tx_font = TextureManager::GetInstance().GetTexture("font");
	D3DXVECTOR3 pen = D3DXVECTOR3(x, y, 0);
	int index = 0;

	tx_font->color = color? *color:D3DXCOLOR(0,0,0,0.75f);

	while (string[index])
	{
		char c = string[index];
		int font_ind;
		if (c == ' ')
		{
			pen.x += 3;
		}
		else if (c == '\n')
		{
			pen.y += 9;
			pen.x = x;
		}
		else if((font_ind = CharToFontIndex(c))>0)
		{
			tx_font->Draw(pen,0,0,font_ind);
			pen.x += gFontWidth[font_ind]+1;
		}
		index++;
	}
}


int GetFontLength(const char* const string)
{
	int len = 0;
	int index = 0;
	int font_ind;
	while (string[index])
	{
		font_ind = CharToFontIndex(string[index]);
		if (font_ind >= 0) len += gFontWidth[font_ind]+1;
		index++;
	}
	if (len > 0) len -= 1;
	return len;
}