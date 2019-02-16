#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define _CRT_SECURE_NO_WARNINGS
#include<windows.h>
#include<WinSock2.h>
#include<stdio.h>
int main()
{
	WORD version = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(version, &data);
	///TCP——client
	//1 建立一个socket
	SOCKET _sock= socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == _sock)
	{
		printf("错误，客户端建立套接字失败！\n");
	}
	else
	{
		printf("客户端建立套接字成功！\n");
	}
	//2 连接服务器
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int rec= connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
	if (SOCKET_ERROR == rec)
	{
		printf("错误，客户端连接失败!\n");
	}
	else
	{
		printf("客户端连接成功！\n");
	}


	while (true)
	{
		//3输入请求命令
		char cmdBuf[128] = {};
		scanf("%s", cmdBuf);
		//4处理请求
		if (0 == strcmp(cmdBuf, "exit"))
		{
			break;
		}
		else
		{
			//5 向服务器发送请求
			send(_sock,cmdBuf,(int)strlen(cmdBuf)+1,0);
		}

		//6 接受服务器信息 recv
		char recvBuf[128];
		int nLen = recv(_sock, recvBuf, 128, 0);

		if (nLen>0)
			printf("接受到服务端数据：%s\n", recvBuf);
	}

	//7 关闭套接字
	closesocket(_sock);
	///
	WSACleanup();
	printf("已退出，任务结束。");
	getchar();
	return 0;
}