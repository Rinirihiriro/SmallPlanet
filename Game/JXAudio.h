/*
	XAudio2�� �ʱ�ȭ�ϰ� �����ϴ� Ŭ����
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