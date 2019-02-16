#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<windows.h>
#include<WinSock2.h>
#include<stdio.h>

struct DataPackage
{
	int age;
	char name[32];
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

	char _recvBuf[128] = {};

	while (true)
	{
		//5 ���ܿͻ�����������
		int nLen = recv(_clientSock,_recvBuf,128,0);
		if (nLen <= 0)
		{
			printf("�ͻ����˳������������\n");
			break;
		}
		printf("�յ�����:%s\n", _recvBuf);
		//6 ��������
		if (0 == strcmp(_recvBuf, "getInfo"))
		{
			//7 ��ͻ��˷���һ������
			DataPackage dp = {80,"�Ź���"};
			send(_clientSock, (const char*)&dp, sizeof(data), 0);
		}
		else
		{
			//7 ��ͻ��˷���һ������
			char msgBuf[] = "?????.\n";
			send(_clientSock, msgBuf, (int)strlen(msgBuf) + 1, 0);
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