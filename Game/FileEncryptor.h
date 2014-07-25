#pragma once

#include <string>
#include <sstream>

#include "Encryptor.h"
#include "Decryptor.h"

class FileEncryptor
{
private:
	Encryptor mEncryptor;
	Decryptor mDecryptor;
	char * decryptOut;

public:
	FileEncryptor();
	~FileEncryptor();

	bool EncryptAndOverwrite(std::string filepath);
	std::stringstream OpenAndDecrypt(std::string filepath);
	void OpenAndDecrypt(std::string filepath, char** out, int* size);

private:
	char AddAllByte(const char* arr, const int len);
};