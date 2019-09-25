//Server2.cpp
//非阻塞服务器邮件槽，用于接收客户发送的广播信息
#include <windows.h>
#include <stdio.h>
#include <conio.h>

HANDLE hSendThread,hRecvThread;
LPDWORD hSendThreadID,hRecvThreadID;

#define MAILSLOTNAMEA "\\\\.\\Mailslot\\HLIPC\\MailslotsA"
#define MAILSLOTNAMEB "\\\\.\\Mailslot\\HLIPC\\MailslotsB"

DWORD WINAPI Send(LPVOID lpParameter )
{
	char buff[0xFFF];
	ZeroMemory(buff,sizeof(buff));
	while(true)
	{
		gets_s(buff,sizeof(buff));
		HANDLE Mailslot;
		DWORD BytesWritten;
		Mailslot = CreateFile(MAILSLOTNAMEB,GENERIC_WRITE, 
			FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		if (INVALID_HANDLE_VALUE == Mailslot)
		{
			printf("CreateFile failed with error %d\n", GetLastError());
			return 0;
		}
		if (0 == WriteFile(Mailslot, buff, strlen(buff), &BytesWritten, NULL))
		{
			printf("WriteFile failed with error %d\n", GetLastError());
			return 0;
		}
		CloseHandle(Mailslot);
	}
	return 0;
}

DWORD WINAPI Recv(LPVOID lpParameter )
{
	char buffer[2048] = {0};
	DWORD NumberOfBytesRead;
	DWORD Ret;
	HANDLE Mailslot;
	Mailslot = CreateMailslot(MAILSLOTNAMEA,2048,MAILSLOT_WAIT_FOREVER,NULL);
	if (INVALID_HANDLE_VALUE == Mailslot)
	{
		printf("Failed to create a MailSlot %d\n",GetLastError());
		return -1;
	}
	while (0 != (Ret = ReadFile(Mailslot,buffer,2048,&NumberOfBytesRead,NULL)))
	{
		printf("Received %d bytes %s\n", NumberOfBytesRead,buffer);
	}
	CloseHandle(Mailslot);
	return 0;
}

int main(int argc, char *argv[])
{
	hSendThread = CreateThread(NULL,0,Send,NULL,NULL,hSendThreadID);
	hRecvThread = CreateThread(NULL,0,Recv,NULL,NULL,hRecvThreadID);
	if( WaitForSingleObject(hSendThread,INFINITE))
		CloseHandle(hSendThread);
	if( WaitForSingleObject(hRecvThread,INFINITE))
		CloseHandle(hRecvThread);

	return 0;
}
