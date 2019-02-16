#define WIN32_LEAN_AND_MEAN

#include<windows.h>
#include<WinSock2.h>

int main()
{
	WORD version=MAKEWORD(2,2);
	WSADATA data;
	WSAStartup(version, &data);
	///

	///
 	WSACleanup();
	return 0;
}