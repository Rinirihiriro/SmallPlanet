#include "FileEncryptor.h"

#include <fstream>

#include "MyException.h"
#include "Essential.h"

FileEncryptor::FileEncryptor()
	:decryptOut(0)
{
}

FileEncryptor::~FileEncryptor()
{
	SAFE_DELETE_ARR(decryptOut);
}

bool FileEncryptor::EncryptAndOverwrite(std::string filepath)
{
	std::ifstream ifs(filepath, std::ios::binary);
	if (!ifs.is_open()) return false;
	ifs.seekg(0, std::ios::end);
	int len = (int)ifs.tellg();
	ifs.seekg(0, std::ios::beg);
	decryptOut = new char[len];
	ZeroMemory(decryptOut, len);
	ifs.read(decryptOut, len);
	ifs.close();
	mEncryptor.Encrypt(decryptOut, len);
	char checkbyte = AddAllByte(decryptOut, len);
	std::ofstream ofs(filepath, std::ios::binary);
	char key[2]={0,};
	mEncryptor.GetKey(key);
	ofs.write(key, 2);
	ofs.write(decryptOut, len);
	ofs.write(&checkbyte, 1);
	ofs.close();
	SAFE_DELETE_ARR(decryptOut);
	return true;
}

std::stringstream FileEncryptor::OpenAndDecrypt(std::string filepath)
{
	std::stringstream ss;
	OpenAndDecrypt(filepath, NULL, NULL);
	if (decryptOut==NULL)
	{
		ss.setstate(std::ios::eofbit);
		return ss;
	}
	
	ss.str(decryptOut);
	SAFE_DELETE_ARR(decryptOut);
	return ss;
}

void FileEncryptor::OpenAndDecrypt(std::string filepath, char** out, int* size)
{
	std::ifstream ifs(filepath, std::ios::binary);
	std::stringstream ss;
	if (ifs.is_open())
	{
		ifs.seekg(0, std::ios::end);
		int len = (int)ifs.tellg();
		len -= 3;
		ifs.seekg(0, std::ios::beg);
		SAFE_DELETE_ARR(decryptOut);
		decryptOut = new char[len];
		ZeroMemory(decryptOut, len);
		char key[2]={0,};
		ifs.read(key, 2);
		ifs.read(decryptOut, len);
		char checkbyte;
		ifs.read(&checkbyte, 1);
		ifs.close();
		if (checkbyte != AddAllByte(decryptOut, len))
		{
			if(out) *out = NULL;
			if(size) *size = -1;
			return;
		}
		mDecryptor.SetKey(key);
		mDecryptor.Decrypt(decryptOut, len);
		if(out) *out = decryptOut;
		if(size) *size = len;
	}
	else
	{
		if(out) *out = NULL;
		if(size) *size = -1;
	}
}

char FileEncryptor::AddAllByte(const char* arr, const int len)
{
	char out=0;
	for (int i=0; i<len; i++)
	{
		out += arr[i];
	}
	return out;
}