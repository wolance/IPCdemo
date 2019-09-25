#include <windows.h>
#include <stdio.h>

HANDLE hSendThread,hRecvThread;
LPDWORD hSendThreadID,hRecvThreadID;

#define NAMEDPIPEA "\\\\.\\pipe\\pipea" 
#define NAMEDPIPEB "\\\\.\\pipe\\pipeb"

DWORD WINAPI Send(LPVOID lpParameter )
{
	while(1)
	{
		if(WaitNamedPipe(NAMEDPIPEA,NMPWAIT_WAIT_FOREVER))
		{
			printf("Connect pipe instance.\n");
			break;
		}
		Sleep(1000);
	}
	HANDLE hPipe = CreateFile(NAMEDPIPEA,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(INVALID_HANDLE_VALUE == hPipe)
	{
		printf("Open the pipe error.%d\n",GetLastError());
		CloseHandle(hPipe);
		hPipe = 0;
		return 0;
	}
	DWORD dwd;
	char buff[0xFFF] = {0};
	while(true)
	{
		gets_s(buff,sizeof(buff));
		WriteFile(hPipe,buff,0xFFF,&dwd,0);
	}
	return 0;
}

DWORD WINAPI Recv(LPVOID lpParameter )
{
	HANDLE hPipe = CreateNamedPipe(NAMEDPIPEB,PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,0,1,1024,1024,0,0);
	if(INVALID_HANDLE_VALUE == hPipe)
	{
		printf("Create Named Pipe Error.%d\n",GetLastError());
		return 0;
	}
	if(!ConnectNamedPipe(hPipe,0))
	{
		printf("Wait for client connection error.%d\n",GetLastError());
		CloseHandle(hPipe);
		hPipe = 0;
		return 0;
	}
	char buff[0xFFF];
	ZeroMemory(buff,sizeof(buff));
	DWORD bytesRead;
	while(true)
	{
		if(ReadFile(hPipe,buff,0xFFF,&bytesRead,NULL)!=NULL)
		{
			printf("receive from B: %s\n",buff);
		}
	}
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
