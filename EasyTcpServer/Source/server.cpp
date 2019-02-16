#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<windows.h>
#include<WinSock2.h>
#include<stdio.h>

enum CMD
{
	CMD_LOGIN,
	CMD_LOGOUT,
	CMD_ERROR
};

struct DataHeader
{
	short dataLength;
	short cmd;
};

//DataPackage
struct Login
{
	char userName[32];
	char passWord[32];
};

struct LoginResult
{
	int ressult;

};

struct Logout
{
	char userName[32];
};

struct LogoutResult
{
	int ressult;

};

int main()
{
	WORD version = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(version, &data);
	///服务端建立
	//1 建立一个socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _sock)
	{
		printf("错误，服务端建立套接字失败！\n");
	}
	else
	{
		printf("服务端建立套接字成功！\n");
	}
	//2 绑定用于客户端连接的网络端口
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;//inet_addr("127.0.0.1");
	if (bind(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		printf("错误,绑定网络端口失败！\n");
	}
	else
	{
		printf("绑定网络端口成功！\n");
	}
	//3 监听网络端口
	if (listen(_sock, 5) == SOCKET_ERROR)
	{
		printf("错误，监听网络端口失败！\n");
	}
	else
	{
		printf("监听端口成功！\n");
	}
	//4 等待客户端连接
	sockaddr_in _clinetAddr = {};
	int nAddrLen = sizeof(sockaddr_in);
	SOCKET _clientSock = INVALID_SOCKET;
	_clientSock = accept(_sock, (sockaddr*)&_clinetAddr, &nAddrLen);
	if (INVALID_SOCKET == _clientSock)
	{
		printf("错误，接收到无效客户端SOCKET...\n");
	}
	printf("新客户端加入：SOCK=%d,IP= %s\n", int(_clientSock),inet_ntoa(_clinetAddr.sin_addr));


	while (true)
	{
		DataHeader header = {};
		//5 接受客户端请求数据
		int nLen = recv(_clientSock,(char*)&header,sizeof(header),0);
		if (nLen <= 0)
		{
			printf("客户端退出，任务结束。\n");
			break;
		}
		printf("收到命令:%d,数据长度：%d\n",header.cmd,header.dataLength);
		switch (header.cmd)
		{
		case CMD_LOGIN:
		{
			Login login = {};
			recv(_clientSock, (char*)&login, sizeof(login), 0);

			LoginResult ret = {1};
			send(_clientSock, (char*)&header, sizeof(header), 0);
			send(_clientSock, (char*)&ret, sizeof(ret), 0);
		}
		break;
		case CMD_LOGOUT:
		{
			Logout logout = {};
			recv(_clientSock, (char*)&logout, sizeof(logout), 0);

			LogoutResult ret = { 1};
			send(_clientSock, (char*)&header, sizeof(header), 0);
			send(_clientSock, (char*)&ret, sizeof(ret), 0);
		}
		break;
		default:
		{
			header.cmd = CMD_ERROR;
			header.dataLength = 0;
			send(_clientSock, (char*)&header, sizeof(DataHeader), 0);
		}
		break;
		}
	}

	//6 关闭套接字
	closesocket(_sock);
	///
	printf("已退出，任务结束。");
	WSACleanup();
	getchar();

	return 0;
}