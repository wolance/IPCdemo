//匿名管道
//只适用于父子进程之间的通信

#include <windows.h>
#include <stdio.h>
#include "../include/Common.h"

#define CHILDPROC "AnonymousPipeB.exe"

HANDLE hSendRead,hSendWrite;//
HANDLE hRecvRead,hRecvWrite;//
HANDLE hSendThread,hRecvThread;
LPDWORD hSendThreadID,hRecvThreadID;

DWORD WINAPI Send(LPVOID lpParameter )
{
	char buff[0xFFF];
	DWORD dwd;
	ZeroMemory(buff,sizeof(buff));
	while(true)
	{
		gets_s(buff,sizeof(buff));
		WriteFile(hSendWrite,buff,32,&dwd,0);
	}
	return 0;
}

DWORD WINAPI Recv(LPVOID lpParameter )
{
	char buff[0xFFF];
	ZeroMemory(buff,sizeof(buff));
	DWORD bytesRead;
	while(true)
	{
		if(ReadFile(hRecvRead,buff,4096,&bytesRead,NULL)!=NULL)
		{
			printf("receive from B: %s\n",buff);
		}
	}
	return 0;
}

int main(int argc, char* argv[])
{
	TCHAR szFileFullPath[256];
	::GetModuleFileName(NULL,static_cast<LPTSTR>(szFileFullPath),256);
	SetConsoleTitle(strrchr(szFileFullPath,'\\')+1);
	printf("put\n");
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	if(!CreatePipe(&hSendRead,&hSendWrite,&sa,0)) 
	return printf("CreatePipe Fault\n");
	if(!CreatePipe(&hRecvRead,&hRecvWrite,&sa,0)) 
		return printf("CreatePipe Fault\n");

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	si.cb = sizeof(STARTUPINFO);
	GetStartupInfo(&si);
	si.hStdError = hRecvWrite;
	si.hStdInput = hSendRead;
	si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	si.wShowWindow = SW_SHOWNORMAL;
	si.dwFlags |= STARTF_USESTDHANDLES;
	si.lpTitle = CHILDPROC;
	if(!CreateProcess(NULL,si.lpTitle,NULL,NULL,TRUE,CREATE_NEW_CONSOLE,NULL,NULL,&si,&pi))
	{
		CloseHandle(hSendRead);
		CloseHandle(hSendWrite);
		CloseHandle(hRecvRead);
		CloseHandle(hRecvWrite);
		return printf("CreateProcess Fault\n");
	}
	
	hSendThread = CreateThread(NULL,0,Send,NULL,NULL,hSendThreadID);
	hRecvThread = CreateThread(NULL,0,Recv,NULL,NULL,hRecvThreadID);
	if( WaitForSingleObject(hSendThread,INFINITE))
		CloseHandle(hSendThread);
	if( WaitForSingleObject(hRecvThread,INFINITE))
		CloseHandle(hRecvThread);
	CloseHandle(hSendRead);
	CloseHandle(hSendWrite);
	CloseHandle(hRecvRead);
	CloseHandle(hRecvWrite);
	
	return 0;
}

