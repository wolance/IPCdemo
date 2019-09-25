//��̬���ݽ���


#include <Windows.h>
#include <stdio.h>
#include "ddeml.h"
#include <string>

#define MAXBUFF 0xFFF
#define NITEM 2									//����ITEM������

const char szApp[]="Server";					//server DDE��������
const char szTopic[]="Topic";					//Server DDEĿ¼����
const char *pszItem[NITEM]={"Item1","Item2"};	//SERVER ITEM�����ַ������飻
int countA = 0;									//��������Static1����ʾ;
int countB = 0;
char ServerData[NITEM][MAXBUFF];						//��ŷ������е����������ݣ�
HCONV hConv=0;									//�Ự�����
DWORD idInst=0;									//DDEMLʵ�������
HWND hWnd;										//���ھ����
HANDLE hInst;									//ʵ�������
HSZ hszApp=0;									//SERVER�����ַ��������
HSZ hszTopic=0;									//SERVERĿ¼�ַ��������
HSZ hszItem[NITEM];								//Server ITEM�ַ��������
BOOL bConnect = FALSE;									// �������ӱ�־��

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
		DdePostAdvise(idInst,hszTopic,hszItem[0]);//֪ͨ���£�
		printf("post data\r\n");
		if(!bConnect)//���û�н�������
		{
			printf("connect ddeb\r\n");
			hConv=DdeConnect(idInst,hszApp,hszTopic,NULL);
			//���ӷ������ˣ�
			if(hConv) //��������ɹ�
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
	case XTYP_CONNECT://�������ӣ�
		printf("recv from ddeb connect\r\n");
		return ((HDDEDATA)TRUE);//����
	case XTYP_ADVDATA: //�����ݵ�����
		printf("recv data from\r\n");
		for(I=0;I<NITEM;I++)	
		{
			if(Item==hszItem[I])
			{
				DdeGetData(hData,(PBYTE)tmp,255,0);//ȡ�����ݣ�
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
		return ((HDDEDATA)DDE_FACK);//��ִ��
	case XTYP_ADVREQ:
	case XTYP_REQUEST://��������
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
		//�������ִ�гɹ���᷵��һ��0ֵ��DMLERR_NO_ERROR��
		//���������᷵�ط���ֵ�����Ա�ʾ���ĳ�������
		printf("DDE SERVER��ʼ��ʧ��!\n");
		return FALSE;
	}
	hszApp=DdeCreateStringHandle(idInst,szApp,0);
	hszTopic=DdeCreateStringHandle(idInst,szTopic,0);
	printf("DDEML �����%x\n",idInst);
	printf("App �����%x\n",hszApp);
	printf("Topic �����%x\n",hszTopic);
	for(int I=0;I<NITEM;I++)
	{
		hszItem[I]=DdeCreateStringHandle(idInst,pszItem[I],0);
		printf("Item ���: %x\n",hszItem[I]);
	}
	//ע�����
	if (!DdeNameService(idInst,hszApp,NULL,DNS_REGISTER))
		printf("����ע��ʧ��\n");

	hSendThread = CreateThread(NULL,0,Send,NULL,NULL,hSendThreadID);
	hRecvThread = CreateThread(NULL,0,Recv,NULL,NULL,hRecvThreadID);
	if( WaitForSingleObject(hSendThread,INFINITE))
		CloseHandle(hSendThread);
	if( WaitForSingleObject(hRecvThread,INFINITE))
		CloseHandle(hRecvThread);

	DdeNameService(idInst,0,0,DNS_UNREGISTER);//ע������
	DdeFreeStringHandle(idInst,hszApp);
	DdeFreeStringHandle(idInst,hszTopic);
	for(int I=0;I<NITEM;I++)
		DdeFreeStringHandle(idInst,hszItem[I]);
	DdeUninitialize(idInst);
	return 0;
}

