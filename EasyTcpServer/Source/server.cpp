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
	///����˽���
	//1 ����һ��socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _sock)
	{
		printf("���󣬷���˽����׽���ʧ�ܣ�\n");
	}
	else
	{
		printf("����˽����׽��ֳɹ���\n");
	}
	//2 �����ڿͻ������ӵ�����˿�
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;//inet_addr("127.0.0.1");
	if (bind(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		printf("����,������˿�ʧ�ܣ�\n");
	}
	else
	{
		printf("������˿ڳɹ���\n");
	}
	//3 ��������˿�
	if (listen(_sock, 5) == SOCKET_ERROR)
	{
		printf("���󣬼�������˿�ʧ�ܣ�\n");
	}
	else
	{
		printf("�����˿ڳɹ���\n");
	}
	//4 �ȴ��ͻ�������
	sockaddr_in _clinetAddr = {};
	int nAddrLen = sizeof(sockaddr_in);
	SOCKET _clientSock = INVALID_SOCKET;
	_clientSock = accept(_sock, (sockaddr*)&_clinetAddr, &nAddrLen);
	if (INVALID_SOCKET == _clientSock)
	{
		printf("���󣬽��յ���Ч�ͻ���SOCKET...\n");
	}
	printf("�¿ͻ��˼��룺SOCK=%d,IP= %s\n", int(_clientSock),inet_ntoa(_clinetAddr.sin_addr));


	while (true)
	{
		DataHeader header = {};
		//5 ���ܿͻ�����������
		int nLen = recv(_clientSock,(char*)&header,sizeof(header),0);
		if (nLen <= 0)
		{
			printf("�ͻ����˳������������\n");
			break;
		}
		printf("�յ�����:%d,���ݳ��ȣ�%d\n",header.cmd,header.dataLength);
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

	//6 �ر��׽���
	closesocket(_sock);
	///
	printf("���˳������������");
	WSACleanup();
	getchar();

	return 0;
}