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
	//������
	char szRecv[4096] = {};
	//5 ���ܿͻ�����������
	int nLen = recv(serverSocket, szRecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (nLen <= 0)
	{
		printf("��������Ͽ����ӣ����������\n");
		return -1;
	}

	switch (header->cmd)
	{
		case CMD_LOGIN_RESULT:
		{
			recv(serverSocket, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			LoginResult* retLogin = (LoginResult*)szRecv;
			printf("���ܵ���������Ϣ��CMD_LOGIN_RESULT�����ݳ��ȣ�%d,result=%d\n",retLogin->dataLength,retLogin->result);
		}
		break;
		case CMD_LOGOUT_RESULT:
		{
			recv(serverSocket, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			LogoutResult* retLogout = (LogoutResult*)szRecv;
			printf("���ܵ���������Ϣ��CMD_LOGOUT_RESULT�����ݳ��ȣ�%d,result=%d\n", retLogout->dataLength, retLogout->result);
		}
		break;
		case CMD_NEW_USER_JOIN:
		{
			recv(serverSocket, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			NewUserJoin* newUseJoin = (NewUserJoin*)szRecv;
			printf("���ܵ���������Ϣ��CMD_NEW_USER_JOIN�����ݳ��ȣ�%d,����socketID=%d\n", newUseJoin->dataLength, newUseJoin->socketID);
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
		printf("�˳�\n");
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
		printf("��֧�ֵ����\n");
	}

}

int main()
{
	WORD version = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(version, &data);
	///TCP����client
	//1 ����һ��socket
	SOCKET _sock= socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == _sock)
	{
		printf("���󣬿ͻ��˽����׽���ʧ�ܣ�\n");
	}
	else
	{
		printf("�ͻ��˽����׽��ֳɹ���\n");
	}
	//2 ���ӷ�����
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int rec= connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
	if (SOCKET_ERROR == rec)
	{
		printf("���󣬿ͻ�������ʧ��!\n");
	}
	else
	{
		printf("�ͻ������ӳɹ���\n");
	}

	//�����߳�
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
			printf("select�������1\n");
			break;
		}
		if (FD_ISSET(_sock, &fdReads))
		{
			FD_CLR(_sock,&fdReads);

			if (-1 == HandleData(_sock))
			{
				printf("select�������2\n");
				break;
			}
		}

		//����ʱ�䴦������ҵ��(���߳�)
		//printf("����ʱ�䴦������ҵ��...\n");
	}

	//7 �ر��׽���
	closesocket(_sock);
	///
	WSACleanup();
	printf("���˳������������");
	getchar();
	return 0;
}