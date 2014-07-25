#pragma once

#include <Windows.h>
#include <queue>

class TCP_Client
{
public:
	enum BufferOption
	{
		BO_NULL		= 0x00
		, BO_RECV	= 0x01
		, BO_SEND	= 0x02
		, BO_ALL	= 0x03
	};

	enum ConnectionStatus
	{
		CS_NULL
		, CS_READY
		, CS_DISCONNECTED
		, CS_WATING
		, CS_CONNECTED
	};

	struct DataPackage
	{
		unsigned size;
		char* data;
	};

private:
	SOCKET hSocket;
	CRITICAL_SECTION cs;
	int errorcode;

	HANDLE hConnectThread;
	HANDLE hSendThread;
	HANDLE hRecvThread;

	std::queue<DataPackage> mRecvDataQueue;
	std::queue<DataPackage> mSendDataQueue;

public:
	TCP_Client();
	~TCP_Client();

	bool Ready(const char* ip_domain, const unsigned short port);
	void Disconnect();

	bool Recv(DataPackage* packagearr, const unsigned len);
	bool Send(DataPackage* packagearr, const unsigned len);

	void ClearBuffer(const BufferOption option = BO_ALL);

	ConnectionStatus GetConnectionState();
	bool IsConnected();
	int GetLastError();
	void ResetError();

	unsigned ConnectThread();
	unsigned SendThread();
	unsigned RecvThread();

	static unsigned WINAPI ConnectThreadStub(void* arg);
	static unsigned WINAPI SendThreadStub(void* arg);
	static unsigned WINAPI RecvThreadStub(void* arg);

};