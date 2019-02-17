#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define _CRT_SECURE_NO_WARNINGS
#include<windows.h>
#include<WinSock2.h>
#include<stdio.h>

enum CMD
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
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
			Login login;
			strcpy(login.userName,"jiangsi");
			strcpy(login.passWord, "js6630232");
			//5 ���������������
			send(_sock,(const char*)&login,sizeof(login),0);
			//���ܷ�������������
			LoginResult retLogin;
			recv(_sock, (char*)&retLogin, sizeof(retLogin), 0);
			printf("LoginResult:%d\n",retLogin.result);
		}
		else if (0 == strcmp(cmdBuf, "logout"))
		{
			Logout logout;
			strcpy(logout.userName, "jiangsi");
			//5 ���������������
			send(_sock, (const char*)&logout, sizeof(logout), 0);
			//���ܷ�������������
			LogoutResult retLogout = {};
			recv(_sock, (char*)&retLogout, sizeof(retLogout), 0);
			printf("LogoutResult:%d\n", retLogout.result);
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