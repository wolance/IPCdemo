// DLLDLL.cpp : 定义 DLL 应用程序的导出函数。
//
#include "DLLDLL.h"
#include <Windows.h>

#pragma data_seg("HL_IPC_DLLDLL")
unsigned int flag = 0;//这里任何变量都必须初始化，否则将在数据段以外，无法共享
#pragma data_seg()

//flag = 0 表示无数据改变
//flag = 1 表示A改变了数据
//flag = 2 表示B改变了数据

__declspec(allocate("HL_IPC_DLLDLL")) char g_HL_IPC_DLLDLL[0xFFF] = {0};

#pragma comment(linker, "/Section:HL_IPC_DLLDLL,RWS")

#define INTERVAL 100 //写入读取的间隔时间，为了防止丢数据

EXPORT void SetDllString(char dst,char* str)
{
	Sleep(INTERVAL);
	strcpy(g_HL_IPC_DLLDLL,str);
	if(dst == 'A') flag = 1;
	if(dst == 'B') flag = 2;
}

EXPORT void GetDllString(char* str)
{
	if(str == 0) return;
	strcpy(str,g_HL_IPC_DLLDLL);
}

EXPORT void WaitGetDllString(char dst,char* str)
{
	while(1)
	{
		if( (flag == 1 && dst == 'A') ||
			(flag == 2 && dst == 'B'))
		{
			GetDllString(str);
			break;
		}
		else Sleep(INTERVAL);
	}
	flag = 0;
}
