#include <windows.h>
#include <stdio.h>
#include <conio.h>

#define SENDMMFShare "RECVMMFShare"
#define RECVMMFShare "SENDMMFShare"

HANDLE hSendThread,hRecvThread;
LPDWORD hSendThreadID,hRecvThreadID;

HANDLE OpenOrCreateFileMapping(DWORD maxsize,LPCSTR lpname )
{
	HANDLE fileMap = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE,FALSE, lpname);
	if (!fileMap) 
	{
		fileMap = CreateFileMapping(INVALID_HANDLE_VALUE,NULL, PAGE_READWRITE, 0, maxsize,lpname);
	}
	return fileMap;
}

DWORD WINAPI Send(LPVOID lpParameter )
{
	char buff[0xFFF];
	ZeroMemory(buff,sizeof(buff));
	HANDLE s_hFileMap = NULL;
	char* pBuff = NULL;
	s_hFileMap = OpenOrCreateFileMapping(1024,SENDMMFShare);
	if(!s_hFileMap) 
	{
		printf("Can't create file mapping.\n");
		return 0;
	}
	if(GetLastError() == ERROR_ALREADY_EXISTS) 
	{
		printf("Mapping already exists - not created.\n");
		CloseHandle(s_hFileMap);
		return 0;
	}
	pBuff = (char*)MapViewOfFile(s_hFileMap,FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0xFFF);
	if(!pBuff)
	{
		printf("Can't map view of file.");
		CloseHandle(s_hFileMap);
		return 0;
	}
	while(true)
	{
		gets_s(buff,sizeof(buff));
		strncpy(pBuff,buff,0xFFF);
	}
	CloseHandle(s_hFileMap);
	UnmapViewOfFile(pBuff);
	return 0;
}

DWORD WINAPI Recv(LPVOID lpParameter )
{
	char buffer[2048] = {0};
	HANDLE hFileMapT = OpenOrCreateFileMapping(1024,RECVMMFShare);
	if (!hFileMapT) 
	{
		printf("Can't open mapping.");
		return 0;
	}
	char* pBuff = (char*)MapViewOfFile(hFileMapT, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	if (!pBuff) 
	{
		printf("Can't map view.");
		CloseHandle(hFileMapT);
		return 0;
	}
	while(1)
	{
		if(strlen(pBuff)>0) 
		{
			printf("From A data: %s\n",pBuff);
			ZeroMemory(pBuff,1024);
		}
		Sleep(200);
	}
	UnmapViewOfFile(pBuff);
	CloseHandle(hFileMapT);
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
