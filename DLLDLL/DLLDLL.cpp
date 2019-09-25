// DLLDLL.cpp : ���� DLL Ӧ�ó���ĵ���������
//
#include "DLLDLL.h"
#include <Windows.h>

#pragma data_seg("HL_IPC_DLLDLL")
unsigned int flag = 0;//�����κα����������ʼ�������������ݶ����⣬�޷�����
#pragma data_seg()

//flag = 0 ��ʾ�����ݸı�
//flag = 1 ��ʾA�ı�������
//flag = 2 ��ʾB�ı�������

__declspec(allocate("HL_IPC_DLLDLL")) char g_HL_IPC_DLLDLL[0xFFF] = {0};

#pragma comment(linker, "/Section:HL_IPC_DLLDLL,RWS")

#define INTERVAL 100 //д���ȡ�ļ��ʱ�䣬Ϊ�˷�ֹ������

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
