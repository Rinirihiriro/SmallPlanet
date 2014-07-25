/*
	XAudio2를 초기화하고 관리하는 클래스
*/
#pragma once

#include <XAudio2.h>

class JXAudio
{
public:
	IXAudio2* pXAudio;
	IXAudio2MasteringVoice* pXAudioMastering;
	
public:
	JXAudio();
	~JXAudio();

};