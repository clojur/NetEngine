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
	//char buf[128] = {};
	//printf("请输入服务器IP：");
	//scanf("%s",buf);
	
	EasyTcpClient client1;
	client1.connectServer("192.168.81.132", 4567);

	EasyTcpClient client2;
	client2.connectServer("192.168.81.130", 4567);

	EasyTcpClient client3;
	client3.connectServer("192.168.81.1", 4567);


	std::thread userSend1(sendHandle, &client1);
	userSend1.detach();

	std::thread userSend2(sendHandle, &client2);
	userSend2.detach();

	std::thread userSend3(sendHandle, &client3);
	userSend3.detach();

	while (client1.isRun()|| client2.isRun()|| client3.isRun())
	{
		client1.onRun();
		client2.onRun();
		client3.onRun();
	}

	client1.closeSocket();
	client2.closeSocket();
	client3.closeSocket();
	return 0;
}
