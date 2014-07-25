#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#include "JMain.h"
#include "MyException.h"

#include "SE.h"

// #include <crtdbg.h>

SE::SE( LPCSTR filename )
{
// 	try
// 	{
		OggVorbis_File ovf;
		int result;
		result = ov_fopen( filename, &ovf );
		// _RPT1(_CRT_WARN, "filename : %s\n", filename);
		// ������ ����.
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
				throw MyException(L"Unknown Ogg File Error. Check the file exists.", filename, result);
			}
		}

		// channels�� rate�� ��� ���� info�� ���´�.
		vorbis_info* vi = ov_info( &ovf, -1 );

		// ���̺� ���� ����
		WAVEFORMATEX wf;
		ZeroMemory( &wf, sizeof(wf) );

		wf.cbSize			= sizeof(wf);
		wf.nChannels		= vi->channels;
		wf.wBitsPerSample	= 16;
		wf.nSamplesPerSec	= vi->rate;
		wf.nAvgBytesPerSec	= wf.nSamplesPerSec * wf.nChannels * 2;
		wf.nBlockAlign		= wf.nChannels * 2;
		wf.wFormatTag		= 1;

		// ������ �д´�.
		int bitstream = 0;
		int read_size = 0;	// ���� ��
		ZeroMemory( &audio_data, sizeof(audio_data) );

		do
		{
			result = ov_read( &ovf, (char*)audio_data+read_size, SE_MAX_BUFFER_SIZE
				, 0, 2, 1, &bitstream);
			read_size += result;	// �Ϲ������� result�� ���� ����Ʈ ���̴�.
			// _RPT2(_CRT_WARN, "result : %d, read_size : %d\n", result, read_size);
		}while( result != 0 );	// EOF(0)���� �д´�.

		// result ���� ������� ����
		if (result < 0)
		{
			switch (result)
			{
			case OV_HOLE:
				throw MyException(L"There was an interruption in the data.", filename, result);
				break;
			case OV_EBADLINK:
				throw MyException(L"An invalid stream section was supplied to libvorbisfile, or the requested link is corrupt.", filename, result);
				break;
			case OV_EINVAL:
				throw MyException(L"The initial file headers couldn't be read or are corrupt", filename, result);
				break;
			default:
				throw MyException(L"Unknown Ogg File Error. Check the file exists.", filename, result);
			}
		}
		ov_clear( &ovf );

		// ���۸� �����.
		ZeroMemory( &buffer, sizeof(buffer));
		buffer.Flags = XAUDIO2_END_OF_STREAM;
		buffer.AudioBytes = read_size;
		buffer.pAudioData = audio_data;

		// ���̽� ����
		for (int i=0; i<SE_MAX_SOURCE_VOICE; i++)
		{
			JMain::GetInstance().CreateXAudio2SourceVoice( &source_voice[i], &wf );
		}
// 	}
// 	catch (MyException e)
// 	{
// 		throw e;
// 	}
}

SE::~SE()
{
	for (int i=0; i<SE_MAX_SOURCE_VOICE; i++)
	{
		source_voice[i]->Stop();
		source_voice[i]->DestroyVoice();
	}
}

void SE::PlayIfStop()
{
	XAUDIO2_VOICE_STATE vs;
	source_voice[0]->GetState(&vs);
	if (vs.BuffersQueued==0)
	{
		source_voice[0]->SubmitSourceBuffer( &buffer );
		source_voice[0]->Start();
	}
}

void SE::Play()
{
	XAUDIO2_VOICE_STATE vs;
	int i;
	for (i=0; i<SE_MAX_SOURCE_VOICE; i++)
	{
		source_voice[i]->GetState(&vs);
		if (vs.BuffersQueued == 0)
		{
			source_voice[i]->SubmitSourceBuffer( &buffer );
			source_voice[i]->Start();
			break;
		}
	}
	if (i == SE_MAX_SOURCE_VOICE)
	{
		i--;
		source_voice[i]->Stop();
		source_voice[i]->FlushSourceBuffers();
		source_voice[i]->SubmitSourceBuffer( &buffer );
		source_voice[i]->Start();
	}
}

void SE::Pause()
{
	for (int i=0; i<SE_MAX_SOURCE_VOICE; i++)
	{
		source_voice[i]->Stop();
	}
}

void SE::Resume()
{
	for (int i=0; i<SE_MAX_SOURCE_VOICE; i++)
	{
		source_voice[i]->Start();
	}
}

void SE::Stop()
{
	for (int i=0; i<SE_MAX_SOURCE_VOICE; i++)
	{
		source_voice[i]->Stop();
		source_voice[i]->FlushSourceBuffers();
	}
}
