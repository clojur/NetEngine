﻿#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#define _WINSOCK_DEPRECATED_NO_WARNINGS

	#define _CRT_SECURE_NO_WARNINGS
	#include<windows.h>
	#include<WinSock2.h>
#else
	#include<unistd.h>
	#include<arpa/inet.h>
	#include<string.h>
	#define SOCKET     int
	#define INVALID_SOCKET  (SOCKET)(~0)
	#define SOCKET_ERROR            (-1)
#endif // _WIN32

#include <algorithm>
#include <vector>
#include <stdio.h>

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

std::vector<SOCKET> g_clients;

int HandleData(SOCKET clientSocket)
{
    //缓冲区
    char szRecv[4096] = {};
    //5 接受客户端请求数据
    int nLen = (int)recv(clientSocket, szRecv, sizeof(DataHeader), 0);
    DataHeader* header = (DataHeader*)szRecv;
    if (nLen <= 0)
    {
        printf("客户端<Socket=%d>退出，任务结束。\n", (int)clientSocket);
        return -1;
    }
    
    switch (header->cmd)
    {
        case CMD_LOGIN:
        {
            recv(clientSocket, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
            Login* login = (Login*)szRecv;
            printf("收到客户端<Socket=%d>请求：CMD_LOGIN 数据长度：%d userName=%s passWord=%s\n", (int)clientSocket, login->dataLength, login->userName, login->passWord);
            LoginResult ret;
            send(clientSocket, (char*)&ret, sizeof(ret), 0);
        }
            break;
        case CMD_LOGOUT:
        {
            recv(clientSocket, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
            Logout* logout = (Logout*)szRecv;
            printf("收到客户端<Socket=%d>请求：CMD_LOGOUT 数据长度：%d userName=%s\n", (int)clientSocket, logout->dataLength, logout->userName);
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
#ifdef _WIN32
    WORD version = MAKEWORD(2, 2);
    WSADATA data;
    WSAStartup(version, &data);
#endif
    ///服务端建立
    //1 建立一个socket
    SOCKET _serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == _serverSock)
    {
        printf("错误，服务端建立套接字失败！\n");
    }
    else
    {
        printf("服务端建立套接字成功！\n");
    }
    //2 绑定用于客户端连接的网络端口
    sockaddr_in _sin = {};
    _sin.sin_family = AF_INET;
    _sin.sin_port = htons(4567);
    
#ifdef _WIN32
    _sin.sin_addr.S_un.S_addr = INADDR_ANY;
#else
    _sin.sin_addr.s_addr = INADDR_ANY;
#endif // _WIN32
    if (bind(_serverSock, (sockaddr*)&_sin, sizeof(_sin)) == SOCKET_ERROR)
    {
        printf("错误,绑定网络端口失败！\n");
    }
    else
    {
        printf("绑定网络端口成功！\n");
    }
    //3 监听网络端口
    if (listen(_serverSock, 5) == SOCKET_ERROR)
    {
        printf("错误，监听网络端口失败！\n");
    }
    else
    {
        printf("监听端口成功！\n");
    }
    
    
    while (true)
    {
        //伯克利 socket
        fd_set fdRead; 
        fd_set fdWrite; 
        fd_set fdExcept; 

		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExcept);
        
        FD_SET(_serverSock, &fdRead);
        FD_SET(_serverSock, &fdWrite);
        FD_SET(_serverSock, &fdExcept);
        
        SOCKET maxSock=_serverSock;
        
        size_t clientCount = g_clients.size();
        for (size_t n = 0; n <clientCount; ++n)
        {
            FD_SET(g_clients[n], &fdRead);
            //查找最大描述符
            if(maxSock<g_clients[n])
                maxSock=g_clients[n];
        }
        
        ///nfds 是一个整数值，是指fd_set集合中所有的SOCKET的范围，而不是数量
        ///即是所有文件描述符（sock）最大值+1 在windows中这个参数可以写0
        timeval tv = { 1,0 };
        int ret = select(maxSock+1, &fdRead, &fdWrite, &fdExcept, &tv);
        
        if (ret < 0)
        {
            printf("select任务结束。\n");
            break;
        }
        if (FD_ISSET(_serverSock, &fdRead))
        {
            FD_CLR(_serverSock, &fdRead);
            //4 等待客户端连接
            sockaddr_in _clinetAddr = {};
            int nAddrLen = sizeof(sockaddr_in);
            SOCKET _clientSock = INVALID_SOCKET;
#ifdef _WIN32
            _clientSock = accept(_serverSock, (sockaddr*)&_clinetAddr, &nAddrLen);
#else
            _clientSock = accept(_serverSock, (sockaddr*)&_clinetAddr, (socklen_t*)&nAddrLen);
#endif
            
            if (INVALID_SOCKET == _clientSock)
            {
                printf("错误，接收到无效客户端SOCKET...\n");
            }
            size_t clientCount = g_clients.size();
            for (size_t n = 0; n <clientCount; ++n)
            {
                NewUserJoin userJoin;
                send(g_clients[n], (const char*)&userJoin, sizeof(NewUserJoin), 0);
            }
            g_clients.push_back(_clientSock);
            printf("新客户端加入：SOCK=%d,IP= %s\n", int(_clientSock), inet_ntoa(_clinetAddr.sin_addr));
        }
        for (size_t i = 0; i<g_clients.size(); ++i)
        {
            if (FD_ISSET(g_clients[i], &fdRead))
            {
                if (-1 == HandleData(g_clients[i]))
                {
                    auto iter = std::find(g_clients.begin(),g_clients.end(),g_clients[i]);
                    if (iter != g_clients.end())
                    {
                        g_clients.erase(iter);
                    }
                }
            }
        }
        
        //空闲时间处理其他业务
        //printf("空闲时间处理其他业务...\n");
    }
    
#ifdef _WIN32
    for (size_t n = 0; n < g_clients.size(); ++n)
    {
        closesocket(g_clients[n]);
    }
    //6 关闭套接字
    closesocket(_serverSock);
    ///
    WSACleanup();
#else
    for (size_t n = 0; n < g_clients.size(); ++n)
    {
        close(g_clients[n]);
    }
    close(_serverSock);
#endif
    printf("已退出，任务结束。");
    getchar();
    
    return 0;
}
