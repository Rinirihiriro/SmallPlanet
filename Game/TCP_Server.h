#pragma once

#include <Windows.h>

#include <string>
#include <queue>
#include <map>

#define MAX_CLIENT 8

class TCP_Server
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
		, CS_CONNECTED
	};

	struct DataPackage
	{
		unsigned size;
		char* data;
		SOCKET socket;	// 0: 모든 소켓(보낼 때)
	};

private:
	SOCKET hConnectSocket;
	CRITICAL_SECTION cs;
	int errorcode;

	struct ClientData
	{
		SOCKET socket;
		HANDLE thread;
		bool connected;
	};

	HANDLE hConnectThread;
	HANDLE hSendThread;

	std::map<SOCKET, ClientData> mClients;

	std::queue<DataPackage> mRecvDataQueue;
	std::queue<DataPackage> mSendDataQueue;

public:
	TCP_Server();
	~TCP_Server();

	unsigned short Ready(unsigned short port=0);
	void Disconnect();
	void Disconnect(SOCKET socket);

	bool Recv(DataPackage* packagearr, const unsigned len);
	bool Send(DataPackage* packagearr, const unsigned len);

	void ClearBuffer(const BufferOption option = BO_ALL);

	ConnectionStatus GetConnectionState(SOCKET sock = 0);
	bool IsConnected(SOCKET sock = 0);
	int GetClients(SOCKET* out = 0);
	SOCKET GetDisconnectedClient();
	int GetLastError();
	void ResetError();

	unsigned ConnectThread();
	unsigned SendThread();
	unsigned ClientHandleThread(SOCKET socket);

	static unsigned WINAPI ConnectThreadStub(void* arg);
	static unsigned WINAPI SendThreadStub(void* arg);
	static unsigned WINAPI ClientThreadStub(void* arg);
};