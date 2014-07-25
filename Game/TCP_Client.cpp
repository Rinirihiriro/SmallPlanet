#include "TCP_Client.h"

#include <process.h>

#include "MyException.h"
#include "CriticalSection.h"
#include "Essential.h"


TCP_Client::TCP_Client()
	:hSocket(0), hConnectThread(0), hRecvThread(0), hSendThread(0), errorcode(0)
{
	InitializeCriticalSection(&cs);
}

TCP_Client::~TCP_Client()
{
	Disconnect();
	ClearBuffer();
	DeleteCriticalSection(&cs);
}


bool TCP_Client::Ready(const char* ip_domain, const unsigned short port)
{
	if (IsConnected()) return false;
	CriticalSection cst(&cs);

	SOCKADDR_IN serverAddr = {0,};
	unsigned long ipAddr = 0;

	// 초기화
	ClearBuffer();
	hConnectThread = hSendThread = hRecvThread = 0;
	errorcode = 0;

	// 소켓 생성
	hSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
	{
		hSocket = 0;
		throw NetworkError(L"TCP socket not created", WSAGetLastError());
	}

	// ip 또는 도메인에서 주소 가져오기
	ipAddr = inet_addr(ip_domain);
	if (ipAddr == INADDR_NONE)
	{
		hostent *host_entry;
		host_entry = gethostbyname(ip_domain);
		if (!host_entry)
		{
			closesocket(hSocket);
			hSocket = 0;
			throw NetworkError(L"Host not found", WSAGetLastError());
		}
		ipAddr = ((SOCKADDR_IN*)host_entry->h_addr_list[0])->sin_addr.s_addr;
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ipAddr;
	serverAddr.sin_port = htons(port);

	// 넌-블로킹 모드
	u_long mode = 1;
	ioctlsocket(hSocket, FIONBIO, &mode);	// non-blocking socket

	// 연결
	if (connect(hSocket, (LPSOCKADDR)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAEWOULDBLOCK)
			hConnectThread = (HANDLE)_beginthreadex(0, 0, TCP_Client::ConnectThreadStub, this, 0, 0);
		else
		{
			errorcode = WSAGetLastError();
			shutdown(hSocket, 1);
			closesocket(hSocket);
			hSocket = 0;
			return false;
		}
	}
	else
	{
// 		mode = 0;
// 		ioctlsocket(hSocket, FIONBIO, &mode);	// blocking socket

		hSendThread = (HANDLE)_beginthreadex(NULL, 0, TCP_Client::SendThreadStub, this, 0, NULL);
		hRecvThread = (HANDLE)_beginthreadex(NULL, 0, TCP_Client::RecvThreadStub, this, 0, NULL);
	}

	return true;
}

void TCP_Client::Disconnect()
{
	if (!IsConnected()) return;

	{
		CriticalSection cst(&cs);

		shutdown(hSocket, 1);
		closesocket(hSocket);
		hSocket = 0;
	}

	HANDLE threads[] = {hSendThread, hRecvThread};
	WaitForMultipleObjects(2, threads, TRUE, INFINITE);
	ResetError();
}


bool TCP_Client::Recv(DataPackage* packagearr, const unsigned len)
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

bool TCP_Client::Send(DataPackage* packagearr, const unsigned len)
{
	if (!IsConnected()) return false;
	CriticalSection cst(&cs);

	for (unsigned i=0; i<len; i++)
	{
		mSendDataQueue.push(packagearr[i]);
	}
	return true;
}


void TCP_Client::ClearBuffer(const BufferOption option)
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


TCP_Client::ConnectionStatus TCP_Client::GetConnectionState()
{
	CriticalSection cst(&cs);

	if (hSocket)
	{
		if (hConnectThread)
			return CS_WATING;
		else
			return CS_CONNECTED;
	}
	else if (hRecvThread && hSendThread)
		return CS_DISCONNECTED;
	else
		return CS_READY;
}

bool TCP_Client::IsConnected()
{
	return GetConnectionState() == CS_CONNECTED;
}


int TCP_Client::GetLastError()
{
	CriticalSection cst(&cs);
	return errorcode;
}

void TCP_Client::ResetError(){errorcode = 0;}


unsigned TCP_Client::ConnectThread()
{
	fd_set fdset;
	TIMEVAL timeout;
	int fd_num;

	{
		CriticalSection cst(&cs);
		FD_ZERO(&fdset);
		FD_SET(hSocket, &fdset);
	}

	timeout.tv_sec = 10;
	timeout.tv_usec = 0;

	fd_num = select(0, 0, &fdset, 0, &timeout);
	if (fd_num > 0)
	{
// 		u_long mode = 0;
// 		ioctlsocket(hSocket, FIONBIO, &mode);	// blocking socket

		hSendThread = (HANDLE)_beginthreadex(NULL, 0, TCP_Client::SendThreadStub, this, 0, NULL);
		hRecvThread = (HANDLE)_beginthreadex(NULL, 0, TCP_Client::RecvThreadStub, this, 0, NULL);
	}
	else	// 연결 실패 또는 타임아웃
	{
		if (fd_num < 0)
			errorcode = WSAGetLastError();
		else
			errorcode = -1;

		shutdown(hSocket, 1);
		closesocket(hSocket);
		hSocket = 0;
	}

	hConnectThread = 0;
	return 0;
}


unsigned TCP_Client::SendThread()
{
	int errcode, senderr;
	DataPackage pack;
	while (IsConnected())
	{
		{
			CriticalSection cst(&cs);
			while (!mSendDataQueue.empty())
			{
				pack = mSendDataQueue.front();
				send(hSocket, (char*)&pack.size, 4, 0);
				senderr = send(hSocket, pack.data, pack.size, 0);
				if (senderr == SOCKET_ERROR)
				{
					errcode = WSAGetLastError();
					if (errcode != WSAEWOULDBLOCK)
					{
						errorcode = errcode;
						break;
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

unsigned TCP_Client::RecvThread()
{
	int errcode;
	int recvlen;
	unsigned datalen;
	char* buf = NULL;
	DataPackage pack;

	fd_set set, setcpy;
	TIMEVAL time = {0,};
	int select_result;

	time.tv_sec = 10;
	time.tv_usec = 0;

	FD_ZERO(&set);
	FD_SET(hSocket, &set);

	while (IsConnected())
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
			recvlen = recv(hSocket, (char*)&datalen, 4, 0);
		}
		if (recvlen == SOCKET_ERROR)
		{
			errcode = WSAGetLastError();
			if (errcode != WSAEWOULDBLOCK)
			{
				errorcode = errcode;
				break;
			}
			else continue;
		}
		else if (recvlen == 0)	// 연결 종료
		{
			break;
		}
		buf = new char[datalen];
		recvlen = 0;
		// 입력받은 길이만큼 데이터를 모두 읽어들인다.
		{
			CriticalSection cst(&cs);
			while (datalen > (unsigned)recvlen)
			{
				errcode = recv(hSocket, buf+recvlen, datalen-recvlen, 0);

				if (errcode == SOCKET_ERROR)
				{
					errcode = WSAGetLastError();
					if (errcode != WSAEWOULDBLOCK)
					{
						SAFE_DELETE_ARR(buf);
						errorcode = errcode;
						break;
					}
					else continue;
				}
				else if (errcode == 0)	// 연결 종료
				{
					SAFE_DELETE_ARR(buf);
					break;
				}
				else
					recvlen += errcode;
			}
		}
		if (!buf) break;

		// 큐에 데이터를 넣는다.
		pack.size = datalen;
		pack.data = buf;
		{
			CriticalSection cst(&cs);
			mRecvDataQueue.push(pack);
		}
		Sleep(1);
	}

	Disconnect();
	return 0;
}


unsigned WINAPI TCP_Client::ConnectThreadStub(void* arg)
{
	return ((TCP_Client*)arg)->ConnectThread();
}

unsigned WINAPI TCP_Client::SendThreadStub(void* arg)
{
	return ((TCP_Client*)arg)->SendThread();
}

unsigned WINAPI TCP_Client::RecvThreadStub(void* arg)
{
	return ((TCP_Client*)arg)->RecvThread();
}
