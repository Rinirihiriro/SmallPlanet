
#include "JMain.h"
#include "MyException.h"

#include "BGM.h"


BGM::BGM( LPCSTR filename, const bool loop, const long loop_start )
	:loop_start(loop_start), loop(loop), loopback(false), buffer_index(0)
	, playing(false), pausing(false), delta_volume(0), fadeout_and_stop(true)
{
// 	try
// 	{
		int result;
		result = ov_fopen( filename, &ovf );
		// 파일을 연다.
		if(result != 0)
		{
			switch (result)
			{
			case OV_EREAD:
				throw MyException(L"A read from media returned an error", filename, result);
				break;
			case OV_ENOTVORBIS:
				throw MyException(L"Bitstream does not contain any Vorbis data.", filename, result);
				break;
			case OV_EVERSION:
				throw MyException(L"Vorbis version mismatch.", filename, result);
				break;
			case OV_EBADHEADER:
				throw MyException(L"Invalid Vorbis bitstream header.", filename, result);
				break;
			case OV_EFAULT:
				throw MyException(L"Internal logic fault; indicates a bug or heap/stack corruption.", filename, result);
				break;
			default:
				throw MyException(L"알 수 없는 Ogg 파일 읽기 오류입니다.", filename, result);
			}
		}

		// channels와 rate를 얻기 위해 info를 얻어온다.
		vorbis_info* vi = ov_info( &ovf, -1 );

		// 웨이브 포멧 생성
		WAVEFORMATEX wf;
		ZeroMemory( &wf, sizeof(wf) );

		wf.cbSize			= sizeof(wf);
		wf.nChannels		= vi->channels;
		wf.wBitsPerSample	= 16;
		wf.nSamplesPerSec	= vi->rate;
		wf.nAvgBytesPerSec	= wf.nSamplesPerSec * wf.nChannels * 2;
		wf.nBlockAlign		= wf.nChannels * 2;
		wf.wFormatTag		= 1;

		// 보이스 생성
		JMain::GetInstance().CreateXAudio2SourceVoice( &source_voice, &wf );

		// 오디오 데이터는 모두 스트리밍으로 불러온다.
// 	}
// 	catch (MyException e)
// 	{
// 		throw e;
// 	}
}

BGM::~BGM()
{
	source_voice->Stop();
	ov_clear(&ovf);
	source_voice->DestroyVoice();
}

void BGM::Streaming()
{
	if (!playing) return;

	XAUDIO2_VOICE_STATE state;
	source_voice->GetState(&state);
	if (state.BuffersQueued < BUFFER_NUM)
	{
		// 루프한다면
		if (loopback)
		{
			ov_pcm_seek(&ovf, loop_start);
			loopback = false;
		}

		// 파일을 읽는다.
		int bitstream = 0;
		int read_size = 0;	// 읽은 양
		int result = 0;
		ZeroMemory( &audio_data[buffer_index], sizeof(audio_data[buffer_index]) );

		do
		{
			result = ov_read( &ovf, (char*)audio_data[buffer_index]+read_size, MAX_BUFFER_SIZE-read_size
				, 0, 2, 1, &bitstream);
			read_size += result;	// 일반적으로 result는 읽은 바이트 값이다.
		}while( result > 0 && read_size < MAX_BUFFER_SIZE );	// EOF(0)또는 최대 버퍼 크기까지 읽는다.

		// result 값이 음수라면 에러
		if (result < 0)
		{
			switch (result)
			{
			case OV_HOLE:
				throw MyException(L"There was an interruption in the data.", result);
				break;
			case OV_EBADLINK:
				throw MyException(L"An invalid stream section was supplied to libvorbisfile, or the requested link is corrupt.", result);
				break;
			case OV_EINVAL:
				throw MyException(L"The initial file headers couldn't be read or are corrupt", result);
				break;
			default:
				throw MyException(L"알 수 없는 Ogg 읽기 에러입니다.", result);
			}
		}

		// EOF를 만나면 루프 백
		if (result == 0 && loop)
		{
			loopback = true;
		}

		// 버퍼 생성
		ZeroMemory(&buffer[buffer_index], sizeof(buffer[buffer_index]));
		buffer[buffer_index].AudioBytes = read_size;
		buffer[buffer_index].pAudioData = audio_data[buffer_index];
		if (result != 0 && !loop)
		{
			buffer[buffer_index].Flags = XAUDIO2_END_OF_STREAM;
		}

		source_voice->SubmitSourceBuffer(&buffer[buffer_index]);

		buffer_index = (buffer_index + 1) % BUFFER_NUM;

	}
}

void BGM::Update()
{
	if (delta_volume != 0)
	{
		SetVolume(GetVolume() + delta_volume);
		if (GetVolume() >= 1.0f)
		{
			SetVolume( 1.0f );
			delta_volume = 0;
		}
		else if (GetVolume() <= 0.0f)
		{
			SetVolume( 0.0f );
			delta_volume = 0;
			if (fadeout_and_stop)
			{	
				Stop();
				SetVolume( 1.0f );
			}
		}
	}
}

void BGM::Play()
{
	if (playing && !pausing) return;
	Stop();
	playing = true;
	pausing = false;
	source_voice->Start();
}

void BGM::Pause()
{
	if (!playing) return;
	playing = false;
	pausing = true;
	source_voice->Stop();
}

void BGM::Resume()
{
	if (!pausing) return;
	playing = true;
	pausing = false;
	source_voice->Start();
}

void BGM::Stop()
{
	if (!playing && !pausing) return;
	playing = false;
	pausing = false;
	source_voice->Stop();
	source_voice->FlushSourceBuffers();
	ov_pcm_seek(&ovf, 0);
	Streaming();
}

void BGM::FadeIn( const int frame )
{
	if (!playing) SetVolume( 0.0f );
	delta_volume = 1.0f / frame;
	SetVolume(GetVolume() + delta_volume);
	if (pausing) Resume();
	else if (!playing) Play();
}

void BGM::FadeOut( const int frame )
{
	if (!playing) SetVolume( 0.0f );
	delta_volume = -1.0f / frame;
	SetVolume(GetVolume() + delta_volume);
	fadeout_and_stop = false;
}

void BGM::FadeOutAndStop( const int frame )
{
	FadeOut( frame );
	fadeout_and_stop = true;
}

void BGM::SetLoop( const bool loop )
{
	this->loop = loop;
}

void BGM::SetVolume( const float volume )
{
	source_voice->SetVolume( volume );
}

float BGM::GetVolume() const
{
	float v;
	source_voice->GetVolume(&v);
	return v;
}

bool BGM::IsLoop() const
{
	return loop;
}

bool BGM::IsPlaying() const
{
	return playing;
}

bool BGM::IsPausing() const
{
	return pausing;
}