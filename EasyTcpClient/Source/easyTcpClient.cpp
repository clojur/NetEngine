#include "easyTcpClient.h"

EasyTcpClient::EasyTcpClient()
{
	_sock = INVALID_SOCKET;
}

EasyTcpClient::~EasyTcpClient()
{
	closeSocket();
}

void EasyTcpClient::initSocket()
{
	if (_sock != INVALID_SOCKET)
	{
		printf("<socket=%d>�رվ�����...\n", _sock);
		closeSocket();
	}

#ifdef _WIN32
	WORD version = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(version, &data);
#endif
	///TCP����client
	//����һ��socket
	_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _sock)
	{
		printf("���󣬿ͻ��˽����׽���ʧ�ܣ�\n");
	}
	else
	{
		printf("�ͻ��˽����׽��ֳɹ���\n");
	}
}

int EasyTcpClient::connectServer(const char * ip, short port)
{
	if (_sock == INVALID_SOCKET)
	{
		initSocket();
	}

	//���ӷ�����
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(port);
#ifdef _WIN32
	_sin.sin_addr.S_un.S_addr = inet_addr(ip);
#else
	_sin.sin_addr.s_addr = inet_addr(ip);
#endif // _WIN32
	int rec = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
	if (SOCKET_ERROR == rec)
	{
		printf("���󣬿ͻ�������ʧ��!\n");
	}
	else
	{
		printf("�ͻ������ӳɹ���\n");
	}
	return rec;
}

void EasyTcpClient::closeSocket()
{
	if (_sock != INVALID_SOCKET)
	{
#ifdef _WIN32
		//�ر��׽���
		closesocket(_sock);
		///
		WSACleanup();
#else
		close(_sock);
#endif // _WIN32

		_sock = INVALID_SOCKET;
	}
}

bool EasyTcpClient::onRun()
{
	if (isRun())
	{
		fd_set fdReads;
		FD_ZERO(&fdReads);
		FD_SET(_sock, &fdReads);
		timeval tv = { 1,0 };
		int ret = select(_sock + 1, &fdReads, 0, 0, &tv);
		if (ret < 0)
		{
			printf("<socket=%d>select�������1\n", _sock);
			return false;
		}
		if (FD_ISSET(_sock, &fdReads))
		{
			FD_CLR(_sock, &fdReads);

			if (-1 == recvDataHandler())
			{
				printf("<socket=%d>select�������1\n", _sock);
				return false;
			}
		}
		return true;
	}

	return false;

}

bool EasyTcpClient::isRun()
{
	return _sock != INVALID_SOCKET;
}

void EasyTcpClient::quit()
{
	_sock = INVALID_SOCKET;
}

//�������� ������ ��ְ�
int EasyTcpClient::recvDataHandler()
{
	//������
	char szRecv[4096] = {};
	//5 ���ܿͻ�����������
	int nLen = (int)recv(_sock, szRecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (nLen <= 0)
	{
		printf("��������Ͽ����ӣ����������\n");
		return -1;
	}

	recv(_sock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
	onNetMsg(header);

	return 0;
}

//��Ӧ������Ϣ
void EasyTcpClient::onNetMsg(DataHeader * header)
{
	switch (header->cmd)
	{
	case CMD_LOGIN_RESULT:
	{

		LoginResult* retLogin = (LoginResult*)header;
		printf("���ܵ���������Ϣ��CMD_LOGIN_RESULT�����ݳ��ȣ�%d,result=%d\n", retLogin->dataLength, retLogin->result);
	}
	break;
	case CMD_LOGOUT_RESULT:
	{
		
		LogoutResult* retLogout = (LogoutResult*)header;
		printf("���ܵ���������Ϣ��CMD_LOGOUT_RESULT�����ݳ��ȣ�%d,result=%d\n", retLogout->dataLength, retLogout->result);
	}
	break;
	case CMD_NEW_USER_JOIN:
	{
		NewUserJoin* newUseJoin = (NewUserJoin*)header;
		printf("���ܵ���������Ϣ��CMD_NEW_USER_JOIN�����ݳ��ȣ�%d,����socketID=%d\n", newUseJoin->dataLength, newUseJoin->socketID);
	}
	break;
	}
}

int EasyTcpClient::sendData(DataHeader * header)
{
	if (isRun() && header)
	{
		return send(_sock, (const char*)header, header->dataLength,0);
	}
	return SOCKET_ERROR;
}
