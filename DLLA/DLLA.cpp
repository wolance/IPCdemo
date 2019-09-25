// DLLA.cpp : 定义控制台应用程序的入口点。
//
#include <windowsx.h>
#include <windows.h>
#include <stdio.h>

typedef int(*dll_SetDllString)(char dst,char* str);
typedef int(*dll_WaitGetDllString)(char dst,char* str);

dll_SetDllString SetDllString;
dll_WaitGetDllString WaitGetDllString;

HANDLE hSendThread,hRecvThread;
LPDWORD hSendThreadID,hRecvThreadID;

DWORD WINAPI Send(LPVOID lpParameter )
{
	char buff[0xFFF];
	ZeroMemory(buff,sizeof(buff));
	while(true)
	{
		gets_s(buff,sizeof(buff));
		SetDllString('A',buff);
	}
	return 0;
}

DWORD WINAPI Recv(LPVOID lpParameter )
{
	char buff[0xFFF];
	ZeroMemory(buff,sizeof(buff));
	
	while(1)
	{
		WaitGetDllString('B',buff);
		printf("From B data: %s\n",buff);
	}

	return 0;
}

int main(int argc, char* argv[])
{
	HINSTANCE dll = LoadLibrary("DLLDLL.dll");
	if(!dll){
		printf ("LoadLibrary failed in DLLDLL.dll\n");
		return -1;
	}
	
	SetDllString = (dll_SetDllString)GetProcAddress(dll,  "SetDllString");
	if (SetDllString == NULL) 
	{
		FreeLibrary(dll);
		return printf ("no 'SetDllString()' function in DLLDLL.dll\n");
	}
	WaitGetDllString = (dll_WaitGetDllString)GetProcAddress(dll,  "WaitGetDllString");
	if (WaitGetDllString == NULL) 
	{
		FreeLibrary(dll);
		return printf ("no 'WaitGetDllString()' function in DLLDLL.dll\n");
	}

	hSendThread = CreateThread(NULL,0,Send,NULL,NULL,hSendThreadID);
	hRecvThread = CreateThread(NULL,0,Recv,NULL,NULL,hRecvThreadID);
	if( WaitForSingleObject(hSendThread,INFINITE))
		CloseHandle(hSendThread);
	if( WaitForSingleObject(hRecvThread,INFINITE))
		CloseHandle(hRecvThread);

	FreeLibrary(dll);
	return 0;
}

