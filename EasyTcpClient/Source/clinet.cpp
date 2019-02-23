#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define _CRT_SECURE_NO_WARNINGS
#include<windows.h>
#include<WinSock2.h>
#include<stdio.h>
#include<thread>

enum CMD
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_NEW_USER_JOIN,
	CMD_ERROR
};

struct DataHeader
{
	short dataLength;
	short cmd;
};

//DataPackage
struct Login : public DataHeader
{
	Login()
	{
		dataLength = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char passWord[32];
};

struct LoginResult : public DataHeader
{
	LoginResult()
	{
		dataLength = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
	}
	int result;

};

struct Logout : public DataHeader
{
	Logout()
	{
		dataLength = sizeof(Logout);
		cmd = CMD_LOGOUT;
	}
	char userName[32];
};

struct LogoutResult : public DataHeader
{
	LogoutResult()
	{
		dataLength = sizeof(LogoutResult);
		cmd = CMD_LOGOUT_RESULT;
		result = 0;
	}
	int result;

};

struct NewUserJoin : public DataHeader
{
	NewUserJoin()
	{
		dataLength = sizeof(NewUserJoin);
		cmd = CMD_NEW_USER_JOIN;
		socketID = 0;
	}
	int socketID;

};

int HandleData(SOCKET serverSocket)
{
	//缓冲区
	char szRecv[4096] = {};
	//5 接受客户端请求数据
	int nLen = recv(serverSocket, szRecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (nLen <= 0)
	{
		printf("与服务器断开连接，任务结束。\n");
		return -1;
	}

	switch (header->cmd)
	{
		case CMD_LOGIN_RESULT:
		{
			recv(serverSocket, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			LoginResult* retLogin = (LoginResult*)szRecv;
			printf("接受到服务器消息：CMD_LOGIN_RESULT，数据长度：%d,result=%d\n",retLogin->dataLength,retLogin->result);
		}
		break;
		case CMD_LOGOUT_RESULT:
		{
			recv(serverSocket, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			LogoutResult* retLogout = (LogoutResult*)szRecv;
			printf("接受到服务器消息：CMD_LOGOUT_RESULT，数据长度：%d,result=%d\n", retLogout->dataLength, retLogout->result);
		}
		break;
		case CMD_NEW_USER_JOIN:
		{
			recv(serverSocket, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			NewUserJoin* newUseJoin = (NewUserJoin*)szRecv;
			printf("接受到服务器消息：CMD_NEW_USER_JOIN，数据长度：%d,加入socketID=%d\n", newUseJoin->dataLength, newUseJoin->socketID);
		}
		break;
	}

	return 0;
}

void cmdThread(SOCKET serverSocket)
{
	char cmdBuf[256] = {};
	scanf("%s",cmdBuf);

	if (0 == strcmp(cmdBuf, "exit"))
	{
		printf("退出\n");
		return;
	}
	else if (0 == strcmp(cmdBuf, "login"))
	{
		Login login;
		strcpy(login.userName, "jiangsi");
		strcpy(login.passWord, "js6630232");
		send(serverSocket, (const char*)&login, sizeof(login), 0);
	}
	else if (0 == strcmp(cmdBuf, "logout"))
	{
		Logout logout;
		strcpy(logout.userName, "jiangsi");
		send(serverSocket, (const char*)&logout, sizeof(logout), 0);
	}
	else
	{
		printf("不支持的命令！\n");
	}

}

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

	//启动线程
	std::thread t1(cmdThread, _sock);

	while (true)
	{
		fd_set fdReads;
		FD_ZERO(&fdReads);
		FD_SET(_sock, &fdReads);
		timeval tv = { 1,0 };
		int ret= select(_sock,&fdReads,0,0,&tv);
		if (ret < 0)
		{
			printf("select任务结束1\n");
			break;
		}
		if (FD_ISSET(_sock, &fdReads))
		{
			FD_CLR(_sock,&fdReads);

			if (-1 == HandleData(_sock))
			{
				printf("select任务结束2\n");
				break;
			}
		}

		//空闲时间处理其他业务(另开线程)
		//printf("空闲时间处理其他业务...\n");
	}

	//7 关闭套接字
	closesocket(_sock);
	///
	WSACleanup();
	printf("已退出，任务结束。");
	getchar();
	return 0;
}