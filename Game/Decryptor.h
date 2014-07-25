#pragma once

// ������ ��ȣȭ
class Decryptor
{
private:
	unsigned char key[2];
	unsigned int iCounter;

public:
	Decryptor(const char* key);
	Decryptor();

	void Decrypt(char* arr, const int size);

	void SetKey(const char* key);

private:
	void DecryptFunc(char* byte);
	char ReverseBit(const char c);
};