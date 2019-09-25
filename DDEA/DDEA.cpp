//动态数据交换


#include <Windows.h>
#include <stdio.h>
#include "ddeml.h"
#include <string>

#define MAXBUFF 0xFFF
#define NITEM 2									//定义ITEM的数量

const char szApp[]="Server";					//server DDE服务名；
const char szTopic[]="Topic";					//Server DDE目录名；
const char *pszItem[NITEM]={"Item1","Item2"};	//SERVER ITEM名称字符串数组；
int countA = 0;									//记数，在Static1中显示;
int countB = 0;
char ServerData[NITEM][MAXBUFF];						//存放服务器中的数据项内容；
HCONV hConv=0;									//会话句柄；
DWORD idInst=0;									//DDEML实例句柄；
HWND hWnd;										//窗口句柄；
HANDLE hInst;									//实例句柄；
HSZ hszApp=0;									//SERVER服务字符串句柄；
HSZ hszTopic=0;									//SERVER目录字符串句柄；
HSZ hszItem[NITEM];								//Server ITEM字符串句柄；
BOOL bConnect = FALSE;									// 建立连接标志；

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
		strcpy(ServerData[0],buff);
		DdePostAdvise(idInst,hszTopic,hszItem[0]);//通知更新；
		printf("post data\r\n");
		if(!bConnect)//如果没有建立连接
		{
			printf("connect ddeb\r\n");
			hConv=DdeConnect(idInst,hszApp,hszTopic,NULL);
			//连接服务器端；
			if(hConv) //如果建立成功
			{
				DWORD dwResult;
				bConnect=TRUE;
				for(int I=0;I<NITEM;I++)
					DdeClientTransaction(NULL,0,hConv,hszItem[I],CF_TEXT,XTYP_ADVSTART,
					TIMEOUT_ASYNC,&dwResult);
			}
			else
				printf("error:%d\r\n",DdeGetLastError(idInst));
		}
	}
	return 0;
}

DWORD WINAPI Recv(LPVOID lpParameter )
{
	return 0;
}

HDDEDATA CALLBACK DdeCallback(UINT wType,UINT wFmt,HCONV hConv,HSZ Topic,HSZ Item,
	HDDEDATA hData,DWORD lData1,DWORD lData2)
{
	int I;
	char tmp[255];
	switch(wType)
	{
	case XTYP_ADVSTART:
	case XTYP_CONNECT://请求连接；
		printf("recv from ddeb connect\r\n");
		return ((HDDEDATA)TRUE);//允许；
	case XTYP_ADVDATA: //有数据到来；
		printf("recv data from\r\n");
		for(I=0;I<NITEM;I++)	
		{
			if(Item==hszItem[I])
			{
				DdeGetData(hData,(PBYTE)tmp,255,0);//取得数据；
				switch(I)
				{
				case 0:
					printf("ITEM0: %d\r\n",tmp);
					break;
				case 1:
					printf("ITEM1: %d\r\n",tmp);
					break;
				}
			}
		}
		return ((HDDEDATA)DDE_FACK);//回执；
	case XTYP_ADVREQ:
	case XTYP_REQUEST://数据请求；
		printf("send data to\r\n");
		for(I=0;I<NITEM;I++)
		{
			if(Item==hszItem[I])
			{
				return DdeCreateDataHandle(idInst,(PBYTE)(LPCTSTR)ServerData[I],
					strlen(ServerData[I]),0,Item,wFmt,0);
			}
		}
	}
	return(0);
}

int main(int argc, char* argv[])
{
	if (DdeInitialize(&idInst,(PFNCALLBACK)DdeCallback,APPCMD_FILTERINITS|
		CBF_FAIL_ALLSVRXACTIONS|CBF_SKIP_ALLNOTIFICATIONS,0))				
	{
		//如果函数执行成功则会返回一个0值，DMLERR_NO_ERROR；
		//如果错误则会返回非零值，用以标示它的出错类型
		printf("DDE SERVER初始化失败!\n");
		return FALSE;
	}
	hszApp=DdeCreateStringHandle(idInst,szApp,0);
	hszTopic=DdeCreateStringHandle(idInst,szTopic,0);
	printf("DDEML 句柄：%x\n",idInst);
	printf("App 句柄：%x\n",hszApp);
	printf("Topic 句柄：%x\n",hszTopic);
	for(int I=0;I<NITEM;I++)
	{
		hszItem[I]=DdeCreateStringHandle(idInst,pszItem[I],0);
		printf("Item 句柄: %x\n",hszItem[I]);
	}
	//注册服务；
	if (!DdeNameService(idInst,hszApp,NULL,DNS_REGISTER))
		printf("名字注册失败\n");

	hSendThread = CreateThread(NULL,0,Send,NULL,NULL,hSendThreadID);
	hRecvThread = CreateThread(NULL,0,Recv,NULL,NULL,hRecvThreadID);
	if( WaitForSingleObject(hSendThread,INFINITE))
		CloseHandle(hSendThread);
	if( WaitForSingleObject(hRecvThread,INFINITE))
		CloseHandle(hRecvThread);

	DdeNameService(idInst,0,0,DNS_UNREGISTER);//注销服务
	DdeFreeStringHandle(idInst,hszApp);
	DdeFreeStringHandle(idInst,hszTopic);
	for(int I=0;I<NITEM;I++)
		DdeFreeStringHandle(idInst,hszItem[I]);
	DdeUninitialize(idInst);
	return 0;
}

