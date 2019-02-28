#include"easyTcpClient.h"

#include<stdio.h>
#include<thread>


void sendHandle(EasyTcpClient* pClient)
{
	while (true)
	{
		char cmdBuff[256] = {};
		scanf("%s", cmdBuff);

		if (0 == strcmp(cmdBuff, "exit"))
		{
			printf("任务退出，程序结束\n");
			pClient->quit();
			break;
		}
		else if (0 == strcmp(cmdBuff, "login"))
		{
			Login login;
			strcpy(login.userName, "jiangsi");
			strcpy(login.passWord, "js123");
			pClient->sendData(&login);
		}
		else if (0 == strcmp(cmdBuff, "logout"))
		{
			Logout logout;
			strcpy(logout.userName, "jiangsi");
			pClient->sendData(&logout);
		}
		else
		{
			printf("不支持的命令！\n");
		}
	}
}


int main()
{
	EasyTcpClient client;
	client.connectServer("127.0.0.1", 4567);

	std::thread userSend(sendHandle, &client);
	userSend.detach();

	while (client.isRun())
	{
		client.onRun();
	}

	client.closeSocket();

	return 0;
}
