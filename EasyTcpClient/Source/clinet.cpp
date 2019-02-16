#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define _CRT_SECURE_NO_WARNINGS
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
			printf("接收到exit命令,退出！\n");
			break;
		}
		else if (0 == strcmp(cmdBuf, "login"))
		{
			Login login = {"lyd", "lydmm"};
			DataHeader dh = {sizeof(login),CMD_LOGIN};
			//5 向服务器发送请求
			send(_sock, (const char*)&dh, sizeof(dh), 0);
			send(_sock,(const char*)&login,sizeof(login),0);
			//接受服务器返回数据
			DataHeader retDh = {};
			LoginResult retLogin = {};
			recv(_sock, (char*)&retDh, sizeof(retDh), 0);
			recv(_sock, (char*)&retLogin, sizeof(retLogin), 0);
			printf("LoginResult:%d",retLogin.ressult);
		}
		else if (0 == strcmp(cmdBuf, "logout"))
		{
			Logout logout = { "lyd"};
			DataHeader dh = { sizeof(logout),CMD_LOGOUT};
			//5 向服务器发送请求
			send(_sock, (const char*)&dh, sizeof(dh), 0);
			send(_sock, (const char*)&logout, sizeof(logout), 0);
			//接受服务器返回数据
			DataHeader retDh = {};
			LogoutResult retLogout = {};
			recv(_sock, (char*)&retDh, sizeof(retDh), 0);
			recv(_sock, (char*)&retLogout, sizeof(retLogout), 0);
			printf("LogoutResult:%d", retLogout.ressult);
		}
		else
		{
			printf("不支持命令，请重新输入！\n");
		}
	}

	//7 关闭套接字
	closesocket(_sock);
	///
	WSACleanup();
	printf("已退出，任务结束。");
	getchar();
	return 0;
}