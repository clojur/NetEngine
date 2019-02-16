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


	while (true)
	{
		//3������������
		char cmdBuf[128] = {};
		scanf("%s", cmdBuf);
		//4��������
		if (0 == strcmp(cmdBuf, "exit"))
		{
			printf("���յ�exit����,�˳���\n");
			break;
		}
		else if (0 == strcmp(cmdBuf, "login"))
		{
			Login login = {"lyd", "lydmm"};
			DataHeader dh = {sizeof(login),CMD_LOGIN};
			//5 ���������������
			send(_sock, (const char*)&dh, sizeof(dh), 0);
			send(_sock,(const char*)&login,sizeof(login),0);
			//���ܷ�������������
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
			//5 ���������������
			send(_sock, (const char*)&dh, sizeof(dh), 0);
			send(_sock, (const char*)&logout, sizeof(logout), 0);
			//���ܷ�������������
			DataHeader retDh = {};
			LogoutResult retLogout = {};
			recv(_sock, (char*)&retDh, sizeof(retDh), 0);
			recv(_sock, (char*)&retLogout, sizeof(retLogout), 0);
			printf("LogoutResult:%d", retLogout.ressult);
		}
		else
		{
			printf("��֧��������������룡\n");
		}
	}

	//7 �ر��׽���
	closesocket(_sock);
	///
	WSACleanup();
	printf("���˳������������");
	getchar();
	return 0;
}