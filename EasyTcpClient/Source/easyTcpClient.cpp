#include "easyTcpClient.h"

EasyTcpClient::EasyTcpClient()
{
	_sock = INVALID_SOCKET;
}

EasyTcpClient::~EasyTcpClient()
{
	closeSocket();
}

void EasyTcpClient::initSocket()
{
	if (_sock != INVALID_SOCKET)
	{
		printf("<socket=%d>关闭旧连接...\n", _sock);
		closeSocket();
	}

#ifdef _WIN32
	WORD version = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(version, &data);
#endif
	///TCP——client
	//建立一个socket
	_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _sock)
	{
		printf("错误，客户端建立套接字失败！\n");
	}
	else
	{
		printf("客户端建立套接字成功！\n");
	}
}

int EasyTcpClient::connectServer(const char * ip, short port)
{
	if (_sock == INVALID_SOCKET)
	{
		initSocket();
	}

	//连接服务器
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(port);
#ifdef _WIN32
	_sin.sin_addr.S_un.S_addr = inet_addr(ip);
#else
	_sin.sin_addr.s_addr = inet_addr(ip);
#endif // _WIN32
	int rec = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
	if (SOCKET_ERROR == rec)
	{
		printf("错误，客户端连接失败!\n");
	}
	else
	{
		printf("客户端连接成功！\n");
	}
	return rec;
}

void EasyTcpClient::closeSocket()
{
	if (_sock != INVALID_SOCKET)
	{
#ifdef _WIN32
		//关闭套接字
		closesocket(_sock);
		///
		WSACleanup();
#else
		close(_sock);
#endif // _WIN32

		_sock = INVALID_SOCKET;
	}
}

bool EasyTcpClient::onRun()
{
	if (isRun())
	{
		fd_set fdReads;
		FD_ZERO(&fdReads);
		FD_SET(_sock, &fdReads);
		timeval tv = { 1,0 };
		int ret = select(_sock + 1, &fdReads, 0, 0, &tv);
		if (ret < 0)
		{
			printf("<socket=%d>select任务结束1\n", _sock);
			return false;
		}
		if (FD_ISSET(_sock, &fdReads))
		{
			FD_CLR(_sock, &fdReads);

			if (-1 == recvDataHandler())
			{
				printf("<socket=%d>select任务结束1\n", _sock);
				return false;
			}
		}
		return true;
	}

	return false;

}

bool EasyTcpClient::isRun()
{
	return _sock != INVALID_SOCKET;
}

void EasyTcpClient::quit()
{
	_sock = INVALID_SOCKET;
}

//接受数据 处理黏包 拆分包
int EasyTcpClient::recvDataHandler()
{
	//缓冲区
	char szRecv[4096] = {};
	//5 接受客户端请求数据
	int nLen = (int)recv(_sock, szRecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (nLen <= 0)
	{
		printf("与服务器断开连接，任务结束。\n");
		return -1;
	}

	recv(_sock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
	onNetMsg(header);

	return 0;
}

//响应网络消息
void EasyTcpClient::onNetMsg(DataHeader * header)
{
	switch (header->cmd)
	{
	case CMD_LOGIN_RESULT:
	{

		LoginResult* retLogin = (LoginResult*)header;
		printf("接受到服务器消息：CMD_LOGIN_RESULT，数据长度：%d,result=%d\n", retLogin->dataLength, retLogin->result);
	}
	break;
	case CMD_LOGOUT_RESULT:
	{
		
		LogoutResult* retLogout = (LogoutResult*)header;
		printf("接受到服务器消息：CMD_LOGOUT_RESULT，数据长度：%d,result=%d\n", retLogout->dataLength, retLogout->result);
	}
	break;
	case CMD_NEW_USER_JOIN:
	{
		NewUserJoin* newUseJoin = (NewUserJoin*)header;
		printf("接受到服务器消息：CMD_NEW_USER_JOIN，数据长度：%d,加入socketID=%d\n", newUseJoin->dataLength, newUseJoin->socketID);
	}
	break;
	}
}

int EasyTcpClient::sendData(DataHeader * header)
{
	if (isRun() && header)
	{
		return send(_sock, (const char*)header, header->dataLength,0);
	}
	return SOCKET_ERROR;
}
