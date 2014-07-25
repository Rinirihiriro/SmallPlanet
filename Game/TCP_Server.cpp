#include "TCP_Server.h"

#include <process.h>

#include "MyException.h"
#include "CriticalSection.h"
#include "Random.h"
#include "Essential.h"


TCP_Server::TCP_Server()
	:hConnectSocket(0), hConnectThread(0), hSendThread(0), errorcode(0)
{
	InitializeCriticalSection(&cs);
}

TCP_Server::~TCP_Server()
{
	Disconnect();
	ClearBuffer();
	DeleteCriticalSection(&cs);
}


unsigned short TCP_Server::Ready(unsigned short port)
{
	if (IsConnected()) return 0;
	CriticalSection cst(&cs);
	SOCKADDR_IN serverAddr;

	hConnectThread = hSendThread = 0;
	errorcode = 0;
	ClearBuffer();

	hConnectSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (hConnectSocket == INVALID_SOCKET)
	{
		hConnectSocket = 0;
		throw NetworkError(L"TCP socket not created", WSAGetLastError());
	}

	// 넌-블로킹 모드
	u_long mode = 1;
	ioctlsocket(hConnectSocket, FIONBIO, &mode);	// non-blocking socket

	// 포트 생성
	if (port<=0x400)	// 예약된 포트번호들을 골랐다면 랜덤으로 다른 포트 선정
	{
		Random rand;
		port = (unsigned short)(rand.Next()%(0x10000-0x400-1)+0x400+1);
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(port);
	if (bind(hConnectSocket, (LPSOCKADDR)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		closesocket(hConnectSocket);
		hConnectSocket = 0;
		throw NetworkError(L"Bind Error", WSAGetLastError());
	}

	if (listen(hConnectSocket, MAX_CLIENT) == SOCKET_ERROR)
	{
		closesocket(hConnectSocket);
		hConnectSocket = 0;
		throw NetworkError(L"Listen Error", WSAGetLastError());
	}

// 	mode = 0;
// 	ioctlsocket(hConnectSocket, FIONBIO, &mode);	// blocking socket

	hConnectThread = (HANDLE)_beginthreadex(0, 0, TCP_Server::ConnectThreadStub, this, 0, 0);
	hSendThread = (HANDLE)_beginthreadex(0, 0, TCP_Server::SendThreadStub, this, 0, 0);

	return port;
}

void TCP_Server::Disconnect()
{
	if (!IsConnected()) return;
	
	HANDLE *threads;
	int threadnum;

	{
		CriticalSection cst(&cs);
		int i=0;

		threadnum = mClients.size()+2;
		threads = new HANDLE[threadnum];
		for each (auto pair in mClients)
		{
			shutdown(pair.second.socket, 1);
			closesocket(pair.second.socket);
			pair.second.socket = 0;
			pair.second.connected = false;
			threads[i++] = pair.second.thread;
		}
		mClients.clear();

		shutdown(hConnectSocket, 1);
		closesocket(hConnectSocket);
		hConnectSocket = 0;
	}
	threads[threadnum-2] = hConnectThread;
	threads[threadnum-1] = hSendThread;

	WaitForMultipleObjects(threadnum, threads, TRUE, INFINITE);
	SAFE_DELETE_ARR(threads);
	ResetError();
}

void TCP_Server::Disconnect(SOCKET socket)
{
	if (!socket) return;

	ClientData client;

	{
		CriticalSection cst(&cs);
		client = mClients[socket];
		if (client.socket)
		{
			shutdown(client.socket, 1);
			closesocket(client.socket);
			mClients[client.socket].socket = 0;
			mClients[client.socket].connected = false;
		}
		mClients.erase(mClients.find(socket));
	}

	WaitForSingleObject(client.thread, INFINITE);
}


bool TCP_Server::Recv(DataPackage* packagearr, const unsigned len)
{
	if (!IsConnected()) return false;
	CriticalSection cst(&cs);

	if (mRecvDataQueue.empty()) return false;

	for (unsigned i=0; i<len; i++)
	{
		packagearr[i] = mRecvDataQueue.front();
		mRecvDataQueue.pop();
		if (mRecvDataQueue.empty()) break;
	}
	return true;
}

bool TCP_Server::Send(DataPackage* packagearr, const unsigned len)
{
	if (!IsConnected()) return false;
	CriticalSection cst(&cs);

	for (unsigned i=0; i<len; i++)
	{
		mSendDataQueue.push(packagearr[i]);
	}
	return true;
}


void TCP_Server::ClearBuffer(const BufferOption option)
{
	CriticalSection cst(&cs);
	DataPackage pack;

	while ((option|BO_RECV) && !mRecvDataQueue.empty())
	{
		pack = mRecvDataQueue.front();
		SAFE_DELETE_ARR(pack.data);
		mRecvDataQueue.pop();
	}

	while ((option|BO_SEND) && !mSendDataQueue.empty())
	{
		pack = mSendDataQueue.front();
		SAFE_DELETE_ARR(pack.data);
		mSendDataQueue.pop();
	}
}

TCP_Server::ConnectionStatus TCP_Server::GetConnectionState(SOCKET sock)
{
	if (sock == 0) sock = hConnectSocket;
	else sock = mClients[sock].socket;
	
	if (!sock) return CS_NULL;

	CriticalSection cst(&cs);

	if (sock)
		return CS_CONNECTED;
	else if (hConnectSocket)
		return CS_DISCONNECTED;
	else
		return CS_READY;
}

bool TCP_Server::IsConnected(SOCKET sock)
{
	return GetConnectionState(sock) == CS_CONNECTED;
}

int TCP_Server::GetClients(SOCKET* out)
{
	if (!out) return mClients.size();
	int i = 0;
	for each (auto pair in mClients)
		out[i++] = pair.first;
	return 0;
}

SOCKET TCP_Server::GetDisconnectedClient()
{
	CriticalSection cst(&cs);

	for each (auto pair in mClients)
		if (!pair.second.connected) return pair.first;
	return 0;
}

int TCP_Server::GetLastError()
{
	CriticalSection cst(&cs);
	return errorcode;
}

void TCP_Server::ResetError(){errorcode = 0;}



unsigned TCP_Server::ConnectThread()
{
	SOCKADDR_IN clientAddr;
	int addr_size = sizeof(clientAddr);
	ClientData client;
	client.connected = true;

	while (IsConnected())
	{
		Sleep(1);
		
		{
			CriticalSection cst(&cs);
			if (mClients.size() >= MAX_CLIENT)
				continue;

			client.socket = accept(hConnectSocket, (LPSOCKADDR)&clientAddr, &addr_size);
			if (client.socket == INVALID_SOCKET)
			{
				if (WSAGetLastError() == WSAEWOULDBLOCK)
					continue;
				else
				{
					errorcode = WSAGetLastError();
					continue;
				}
			}

			// 스레드, 클라이언트 데이터 생성
			unsigned arg[] = {(unsigned)this, (unsigned)client.socket};
			mClients[client.socket] = client;
			client.thread = (HANDLE)_beginthreadex(0, 0, TCP_Server::ClientThreadStub, arg, 0, 0);
		}
	}

	return 0;
}

unsigned TCP_Server::SendThread()
{
	int senderr, errcode;
	DataPackage pack;

	while (IsConnected())
	{
		{
			CriticalSection cst(&cs);
			while (!mSendDataQueue.empty())
			{
				pack = mSendDataQueue.front();
				if (pack.socket == 0)
				{
					for each (auto pair in mClients)
					{
						send(pair.second.socket, (char*)&pack.size, 4, 0);
						send(pair.second.socket, pack.data, pack.size, 0);
					}
				}
				else if (mClients[pack.socket].socket)
				{
					send(pack.socket, (char*)&pack.size, 4, 0);
					senderr = send(pack.socket, pack.data, pack.size, 0);
					if (senderr == SOCKET_ERROR)
					{
						errcode = WSAGetLastError();
						if (errorcode == WSAENOTCONN)	// 연결이 끊어진 소켓은 흘려보낸다
						{
							errorcode = errcode;
						}
						else if (errcode != WSAEWOULDBLOCK)	// 에러
						{
							errorcode = errcode;
							break;
						}
					}
				}
				SAFE_DELETE_ARR(pack.data);
				mSendDataQueue.pop();
			}
		}
		Sleep(1);
	}

	return 0;
}

unsigned TCP_Server::ClientHandleThread(SOCKET socket)
{
	ClientData client;
	int recvlen;
	DataPackage pack;
	int errcode;

	fd_set set, setcpy;
	TIMEVAL time = {0,};
	int select_result;

	time.tv_sec = 10;
	time.tv_usec = 0;

	{
		CriticalSection cst(&cs);
		client = mClients[socket];
		pack.socket = client.socket;
	}

	FD_ZERO(&set);
	FD_SET(client.socket, &set);

	while (IsConnected(client.socket))
	{
		setcpy = set;
		select_result = select(0, &setcpy, 0, 0, &time);
		if (select_result == SOCKET_ERROR)			// 에러
		{
			errorcode = WSAGetLastError();
			break;
		}
		else if (select_result == 0) continue;		// 타임아웃

		ZeroMemory(&pack, sizeof(pack));
		{
			CriticalSection cst(&cs);
			// 데이터의 크기를 얻는다
			recvlen = recv(client.socket, (char*)&pack.size, 4, 0);
		}

		if (recvlen == SOCKET_ERROR)
		{
			errcode = WSAGetLastError();
			if (errcode != WSAEWOULDBLOCK)	// 에러
			{
				errorcode = errcode;
				break;
			}
			else
				continue;	// 데이터 없음
		}
		else if (recvlen == 0)	// 연결 종료
		{
			break;
		}
		pack.data = new char[pack.size];
		recvlen = 0;

		// 데이터 길이만큼 입력
		{
			CriticalSection cst(&cs);
			while (pack.size > (unsigned)recvlen)
			{
				errcode = recv(client.socket, pack.data+recvlen, pack.size-recvlen, 0);

				if (errcode == SOCKET_ERROR)
				{
					errcode = WSAGetLastError();
					if (errcode != WSAEWOULDBLOCK)	// 에러
					{
						SAFE_DELETE_ARR(pack.data);
						errorcode = errcode;
						break;
					}
					else continue;	// 데이터 없음
				}
				else if (errcode == 0)	// 연결 종료
				{
					SAFE_DELETE_ARR(pack.data);
					break;
				}
				else
					recvlen += errcode;
			}
		}
		if (!pack.data && errorcode == 0) break;	// 에러
		else if (!pack.data) continue;			// 연결 종료

		// 큐에 데이터를 넣는다.
		{
			CriticalSection cst(&cs);
			mRecvDataQueue.push(pack);
		}
	}

	if (client.socket)
	{
		shutdown(client.socket, 1);
		closesocket(client.socket);
		mClients[client.socket].socket = 0;
		mClients[client.socket].connected = false;
	}
	// 연결이 끊긴 사실을 알리기 위해 지우지 않는다.

	return 0;
}


unsigned WINAPI TCP_Server::ConnectThreadStub(void* arg)
{
	return ((TCP_Server*)arg)->ConnectThread();
}

unsigned WINAPI TCP_Server::SendThreadStub(void* arg)
{
	return ((TCP_Server*)arg)->SendThread();
}

unsigned WINAPI TCP_Server::ClientThreadStub(void* arg)
{
	return ((TCP_Server*)((unsigned*)arg)[0])->ClientHandleThread((SOCKET)((unsigned*)arg)[1]);
}
