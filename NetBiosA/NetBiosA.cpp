#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <nb30.h>
#include <memory.h>
#pragma comment(lib,"netapi32.lib")
#define MAXSESSIONS 254//最大会话数为254
#define MAXNAMES 254  //最大名字数为254
UCHAR Lana_Enum(LANA_ENUM *lenum)//获取可用lana
{
	NCB ncb;
	UCHAR ucRc;
	memset(&ncb,0,sizeof(NCB));
	ncb.ncb_command=NCBENUM;
	ncb.ncb_buffer=(PUCHAR)lenum;
	ncb.ncb_length=sizeof(LANA_ENUM);
	ucRc=Netbios(&ncb);
	return(ucRc);
}
/*
UCHAR RestNCB(UCHAR lana)//复位特定的网络适配器
{
	NCB ncb;
	UCHAR ucRc;
	memset(&ncb,0,sizeof(ncb));
	ncb.ncb_command=NCBRESET;
	ncb.ncb_callname[0]=MAXNAMES;
	ncb.ncb_callname[2]=MAXSESSIONS;
	ncb.ncb_lana_num=lana;
	ucRc=Netbios(&ncb);
	return(ucRc);
}
*/
UCHAR AddName(CHAR *Name,UCHAR *NameNum,UCHAR lana)//添加名字
{
	NCB ncb;
	//LANA_ENUM lenum;
	memset(&ncb,0,sizeof(NCB));
	ncb.ncb_command=NCBADDNAME;
	ncb.ncb_lana_num=lana;
	strcpy((char*)ncb.ncb_name,Name);
	Netbios(&ncb);
	*NameNum=ncb.ncb_num;
	return(ncb.ncb_cmd_cplt);
}
/*
UCHAR HangupSession(UCHAR SessionNum)
{
	NCB ncb;
	UCHAR ucRc;
	memset(&ncb,0,sizeof(NCB));
	ncb.ncb_command=NCBHANGUP;
	ncb.ncb_lsn=SessionNum;
	ucRc=Netbios(&ncb);
	return(ucRc);
}
*/
void NetbiosListen(PNCB pncb,HANDLE hEvent,CHAR SourName[],CHAR DestName[])
{
	memset(pncb,0,sizeof(NCB));
	pncb->ncb_command=NCBLISTEN|ASYNCH;
	strcpy((char *)pncb->ncb_name,SourName);
	strcpy((char *)pncb->ncb_callname,DestName);
	pncb->ncb_rto=0;
	pncb->ncb_sto=5;
	pncb->ncb_cmd_cplt=0xff;
	pncb->ncb_event=hEvent;
	Netbios(pncb);
}
void SessionSend(PNCB pncb,UCHAR Num,UCHAR *pBuffer,WORD Length)
{
	memset(pncb,0,sizeof(NCB));
	pncb->ncb_command=NCBSEND;
	pncb->ncb_lsn=Num;
	pncb->ncb_buffer=pBuffer;
	pncb->ncb_length=Length;
	Netbios(pncb);
}
void SessionRecv(PNCB pncb,UCHAR Num,HANDLE hEvent,UCHAR *pBuffer)
{
	memset(pncb,0,sizeof(NCB));
	pncb->ncb_command=NCBRECV|ASYNCH;
	pncb->ncb_lsn=Num;
	pncb->ncb_buffer=pBuffer;
	pncb->ncb_length=512;
	pncb->ncb_cmd_cplt=0xff;
	pncb->ncb_event=hEvent;
	//pncb->ncb_lana_num=lana;
	Netbios(pncb);
}
/*
int main()
{
	NCB ServerNncb;
	char LocalName[16];
	UCHAR NameNum,SessionNum;
	HANDLE hEvent;
	LANA_ENUM lenum;
	char buff[512];
	if (Lana_Enum(&lenum)!=NRC_GOODRET)
	{
		printf("LanaEnum Error!\n");
		return 0;
	}
	if (RestNCB(lenum.lana[0])!=0)
	{
		printf("Reset NCB FALSE!\n");
		return 0;
	}
	printf("Lana_num is: %d\n",lenum.lana[0]);
	printf("Reset NCB OK!\n");
	strcpy(LocalName,"Server");
	if (AddName(LocalName,&NameNum,lenum.lana[0]))
		return 0;
	printf("Add Name OK!\n");
	hEvent=CreateEvent(NULL,FALSE,FALSE,"Server");
	if (hEvent==NULL)
		return 0;
	printf("Create Event OK!\n");
	for(;;)
	{
		printf("READY ! \n");
		NetbiosListen(&ServerNncb,hEvent,LocalName,"*");//侦听客户连接请求；
		WaitForSingleObject(hEvent,INFINITE);
		if (ServerNncb.ncb_cmd_cplt!=0)
			return 0;
		printf("Listen OK!\n");
		SessionNum=ServerNncb.ncb_lsn;
		printf("SessionNum is:%d\n",ServerNncb.ncb_lsn);
		strcpy(buff,"Welcome to Server!\n");
		SessionSend(&ServerNncb,SessionNum,(UCHAR *)buff,sizeof(buff));
		for(;;)
		{	
			memset(buff,0,512);
			SessionRecv(&ServerNncb,SessionNum,hEvent,(UCHAR*)buff);
			WaitForSingleObject(hEvent,INFINITE);
			//printf("Recv:");
			if (ServerNncb.ncb_cmd_cplt==0)
			{
				printf("收到的文件名字是:%s,长度为：%d 字节\n",buff,strlen(buff));
				printf("\n");
				SessionSend(&ServerNncb,SessionNum,(UCHAR *)buff,sizeof(buff));
				continue;
			}
			else
				if (ServerNncb.ncb_cmd_cplt==0)
				{
					printf("收到的文件内容是:%s,长度为：%d 字节\n",buff,strlen(buff));
					printf("\n");
					SessionSend(&ServerNncb,SessionNum,(UCHAR *)buff,sizeof(buff));
					continue;
				}
				else
					break;
		}
	}
}
*/