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
			break;
		}
		else
		{
			//5 ���������������
			send(_sock,cmdBuf,(int)strlen(cmdBuf)+1,0);
		}

		//6 ���ܷ�������Ϣ recv
		char recvBuf[128];
		int nLen = recv(_sock, recvBuf, 128, 0);

		if (nLen>0)
			printf("���ܵ���������ݣ�%s\n", recvBuf);
	}

	//7 �ر��׽���
	closesocket(_sock);
	///
	WSACleanup();
	printf("���˳������������");
	getchar();
	return 0;
}