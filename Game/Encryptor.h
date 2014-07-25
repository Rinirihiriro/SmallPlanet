#pragma once

class Encryptor
{
private:
	unsigned char key[2];
	unsigned int iCounter;

public:
	Encryptor();
	void Encrypt(char* arr, const int size);

	void GetKey(char* out);

private:
	void EncryptFunc(char* byte);
	char ReverseBit(const char c);
};
