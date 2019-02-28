#ifndef _easyTcpClient_h
#define _easyTcpClient_h

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#define _WINSOCK_DEPRECATED_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
	#include<windows.h>
	#include<WinSock2.h>
#else
	#include<unistd.h>
	#include<arpa/inet.h>
	#include<string.h>
	#define SOCKET     int
	#define INVALID_SOCKET  (SOCKET)(~0)
	#define SOCKET_ERROR            (-1)
#endif // _WIN32

#include<stdio.h>
#include<thread>

#include "messageHeader.h"

class EasyTcpClient
{
public:
	EasyTcpClient();
	virtual ~EasyTcpClient();

	void	initSocket();
	int		connectServer(const char* ip,short port);
	void	closeSocket();
	bool	onRun();
	bool	isRun();
	void	quit();

	int      recvDataHandler();
	void	 onNetMsg(DataHeader* header);
	int		 sendData(DataHeader* header);

private:
	SOCKET _sock;
};

#endif