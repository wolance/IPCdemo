#include <windows.h>
#include <stdio.h>

HANDLE hSendRead,hRecvWrite;
HANDLE hRecvThread;
LPDWORD hRecvThreadID;

DWORD WINAPI Recv(LPVOID lpParameter )
{
	char buff[0xFFF];
	char tmpbuff[0xFFF];
	DWORD dwd;
	ZeroMemory(buff,sizeof(buff));
	DWORD bytesRead;
	while(true)
	{
		if(ReadFile(hSendRead,buff,4096,&bytesRead,NULL)!=NULL)
		{
			printf("receive from A: %s\n",buff);
			printf("send to A: msg ok\n");
			sprintf_s(tmpbuff,sizeof(tmpbuff),"%s [OK]",buff);
			WriteFile(hRecvWrite,tmpbuff,32,&dwd,0);
		}
	}
	return 0;
}


int main(int argc, char* argv[])
{
	char buff[0xFFF];
	ZeroMemory(buff,sizeof(buff));
	
	hSendRead = GetStdHandle(STD_INPUT_HANDLE);
	hRecvWrite = GetStdHandle(STD_ERROR_HANDLE);

	hRecvThread = CreateThread(NULL,0,Recv,NULL,NULL,hRecvThreadID);
	
	if( WaitForSingleObject(hRecvThread,INFINITE))
	CloseHandle(hRecvThread);
	CloseHandle(hSendRead);
	CloseHandle(hRecvWrite);
 	return 0;
}

