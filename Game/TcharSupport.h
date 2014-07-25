#pragma once

#include <tchar.h>

int TCHARtoInt(const TCHAR* str);
int CharToInt(const char* str);

void IntToTCHAR(const int num, TCHAR* out);
void IntToChar(const int num, char* out);

bool IsNumber(const TCHAR* str);
bool IsNumber(const char* str);

void ToFileName( TCHAR* str );
void ToFileName( char* str );
int SeperateBySpace(TCHAR* str, TCHAR** output_arr);
void TrimSpaces(TCHAR* str);