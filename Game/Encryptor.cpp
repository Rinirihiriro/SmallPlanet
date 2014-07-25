#include "Encryptor.h"
#include "Random.h"

Encryptor::Encryptor()
	:iCounter(0)
{
	Random rand;
	key[0] = rand.Next() & 0xff;
	key[1] = rand.Next() & 0xff;
}

void Encryptor::Encrypt(char* arr, const int size)
{
	for (int i=0; i<size; i++, iCounter++)
	{
		iCounter %= 25;
		EncryptFunc(&arr[i]);
	}
}


void Encryptor::GetKey(char* out)
{
	for (int i=0; i<2; i++)
		out[i] = key[i];
}


void Encryptor::EncryptFunc(char* byte)
{
	char a, b, k=0;
	switch (iCounter%5)
	{
	case 0:
		b = key[1]; break;
	case 1:
		b = !key[1]; break;
	case 2:
		b = ReverseBit(key[1]); break;
	case 3:
		b = !ReverseBit(key[1]); break;
	case 4:
		b = 1; break;
	}
	switch (iCounter/5)
	{
	case 0:
		a = key[0]; break;
	case 1:
		a = !key[0]; break;
	case 2:
		a = ReverseBit(key[0]); break;
	case 3:
		a = !ReverseBit(key[0]); break;
	case 4:
		a = 1; break;
	}
	k = a^b;
	*byte ^= k;
}

char Encryptor::ReverseBit(const char c)
{
	char return_char = 0;
	char in = c;
	for (int i=0; i<8; i++)
	{
		return_char <<= 1;
		return_char |= in & 0x01;
		in >>= 1;
	}
	return return_char;
}