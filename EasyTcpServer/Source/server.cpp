#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<windows.h>
#include<WinSock2.h>
#include<vector>
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

std::vector<SOCKET> g_clients;

int HandleData(SOCKET clientSocket)
{
	//������
	char szRecv[1024];
	//5 ���ܿͻ�����������
	int nLen = recv(clientSocket, szRecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (nLen <= 0)
	{
		printf("�ͻ����˳������������\n");
		return -1;
	}

	switch (header->cmd)
	{
	case CMD_LOGIN:
	{
		recv(clientSocket, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		Login* login = (Login*)szRecv;
		printf("�յ����CMD_LOGIN ���ݳ��ȣ�%d userName=%s passWord=%s\n", login->dataLength, login->userName, login->passWord);
		LoginResult ret;
		send(clientSocket, (char*)&ret, sizeof(ret), 0);
	}
	break;
	case CMD_LOGOUT:
	{
		recv(clientSocket, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		Logout* logout = (Logout*)szRecv;
		printf("�յ����CMD_LOGOUT ���ݳ��ȣ�%d userName=%s\n", logout->dataLength, logout->userName);
		LogoutResult ret;
		send(clientSocket, (char*)&ret, sizeof(ret), 0);
	}
	break;
	default:
	{
		header->cmd = CMD_ERROR;
		header->dataLength = 0;
		send(clientSocket, (char*)&header, sizeof(DataHeader), 0);
	}
	break;
	}

	return 0;
}

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


	while (true)
	{
		//������ socket
		fd_set fdRead; FD_ZERO(&fdRead);
		fd_set fdWrite; FD_ZERO(&fdWrite);
		fd_set fdExcept; FD_ZERO(&fdExcept);

		FD_SET(_sock, &fdRead);
		FD_SET(_sock, &fdWrite);
		FD_SET(_sock, &fdExcept);

		size_t clientCount = g_clients.size();
		for (size_t n = 0; n <clientCount; ++n)
		{
			FD_SET(g_clients[n], &fdRead);
		}

		///nfds ��һ������ֵ����ָfd_set���������е�SOCKET�ķ�Χ������������
		///���������ļ���������sock�����ֵ+1 ��windows�������������д0
		int ret= select(_sock + 1, &fdRead, &fdWrite, &fdExcept, nullptr);
		if (ret < 0)
		{
			printf("select���������\n");
			break;
		}
		if (FD_ISSET(_sock, &fdRead))
		{
			FD_CLR(_sock, &fdRead);
			//4 �ȴ��ͻ�������
			sockaddr_in _clinetAddr = {};
			int nAddrLen = sizeof(sockaddr_in);
			SOCKET _clientSock = INVALID_SOCKET;
			_clientSock = accept(_sock, (sockaddr*)&_clinetAddr, &nAddrLen);
			if (INVALID_SOCKET == _clientSock)
			{
				printf("���󣬽��յ���Ч�ͻ���SOCKET...\n");
			}
			g_clients.push_back(_clientSock);
			printf("�¿ͻ��˼��룺SOCK=%d,IP= %s\n", int(_clientSock), inet_ntoa(_clinetAddr.sin_addr));
		}

		for (size_t n = 0; n < fdRead.fd_count; ++n)
		{
			if (-1 == HandleData(fdRead.fd_array[n]))
			{
				auto iter = find(g_clients.begin(), g_clients.end(), fdRead.fd_array[n]);
				if (iter != g_clients.end())
					g_clients.erase(iter);
			}
		}
	}

	for (size_t n = 0; n < g_clients.size(); ++n)
	{
		closesocket(g_clients[n]);
	}
	//6 �ر��׽���
	closesocket(_sock);
	///
	printf("���˳������������");
	WSACleanup();
	getchar();

	return 0;
}