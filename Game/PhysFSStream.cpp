#include "PhysFSStream.h"

#include <Windows.h>

#include "Essential.h"

PhysFSStream::PhysFSStream()
	:bLoad(false)
{
}

PhysFSStream::PhysFSStream(const std::string& zipfile, const std::string datafile, const bool widechar)
	:bLoad(false)
{
	if (!widechar) load(zipfile, datafile);
	else load_w(zipfile, datafile);
}

void PhysFSStream::load(const std::string& zipfile, const std::string datafile)
{
	PHYSFS_addToSearchPath(zipfile.c_str(), 1);
	if (PHYSFS_exists(datafile.c_str())>0)
	{
		std::string str;
		char* buf=NULL;
		PHYSFS_File *file = PHYSFS_openRead(datafile.c_str());
		PHYSFS_sint64 len = PHYSFS_fileLength(file);
		if (len>0)
		{
			buf = new char[(unsigned)len+1];
			ZeroMemory(buf, (unsigned)len+1);
			PHYSFS_read(file, buf, (PHYSFS_uint32)len, 1);
			str.assign(buf);
			SAFE_DELETE_ARR(buf);
			stream.str(str);
			bLoad = true;
		}
		PHYSFS_close(file);
	}
	PHYSFS_removeFromSearchPath(zipfile.c_str());
}

void PhysFSStream::load_w(const std::string& zipfile, const std::string datafile)
{
	PHYSFS_addToSearchPath(zipfile.c_str(), 1);
	if (PHYSFS_exists(datafile.c_str())>0)
	{
		std::wstring str;
		char* buf=NULL;
		wchar_t* wbuf=NULL;
		int buflen;
		PHYSFS_File *file = PHYSFS_openRead(datafile.c_str());
		PHYSFS_sint64 len = PHYSFS_fileLength(file);
		if (len>0)
		{
			buf = new char[(unsigned)len+1];
			ZeroMemory(buf, (unsigned)len+1);
			PHYSFS_read(file, buf, (unsigned)len, 1);
			buflen = MultiByteToWideChar(CP_ACP, 0, buf, -1, wbuf, 0);
			wbuf = new wchar_t[buflen+1];
			ZeroMemory(wbuf, buflen+1);
			MultiByteToWideChar(CP_ACP, 0, buf, -1, wbuf, buflen);
			str.assign(wbuf);
			SAFE_DELETE_ARR(buf);
			SAFE_DELETE_ARR(wbuf);
			wstream.str(str);
			bLoad = true;
		}
		PHYSFS_close(file);
	}
	PHYSFS_removeFromSearchPath(zipfile.c_str());
}

bool PhysFSStream::is_load()
{
	return bLoad;
}

void PhysFSStream::imbue_w(const std::locale& loc)
{
	wstream.imbue(loc);
}

void PhysFSStream::clear()
{
	stream.clear();
	wstream.clear();
	stream.str("");
	wstream.str(L"");
	bLoad = false;
}