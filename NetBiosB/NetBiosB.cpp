#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <nb30.h>
#include <memory.h>
#pragma comment(lib,"netapi32.lib")
#define MAXSESSIONS 254//最大回话数为254
#define MAXNAMES 254  //最大名字数为254
#if 0
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
UCHAR RestNCB(UCHAR lana)//复位特定的网络适配器
{
    NCB ncb;
    UCHAR ucRc;
    memset(&ncb,0,sizeof(NCB));
    ncb.ncb_command=NCBRESET;
    ncb.ncb_callname[0]=MAXSESSIONS;
    ncb.ncb_callname[2]=MAXNAMES;
    ncb.ncb_callname[3]=TRUE;
    ncb.ncb_lana_num=lana;
    ucRc=Netbios(&ncb);
    return(ucRc);
}

UCHAR AddName(UCHAR *Name,UCHAR *NameNum,UCHAR lana)//添加名字
{
    NCB ncb;
    LANA_ENUM lenum;
    memset(&ncb,0,sizeof(NCB));
    ncb.ncb_command=NCBADDNAME;
    ncb.ncb_lana_num=lana;
    strcpy((char*)ncb.ncb_name,(char*)Name);
    Netbios(&ncb);
    *NameNum=ncb.ncb_num;
    return(ncb.ncb_cmd_cplt);
}
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
UCHAR NetbiosCall(PNCB pncb,CHAR SourName[],CHAR DestName[])
{
    memset(pncb,0,sizeof(NCB));
    pncb->ncb_command=NCBCALL;
    strcpy((char*)pncb->ncb_name,SourName);
    strcpy((char*)pncb->ncb_callname,DestName);
    pncb->ncb_rto=0;
    pncb->ncb_sto=10;
    Netbios(pncb);
	return 0;
}
void SessionSend(PNCB pncb,UCHAR Num,UCHAR *pBuffer,WORD Length,UCHAR lana)
{
    memset(pncb,0,sizeof(NCB));
    pncb->ncb_command=NCBSEND;
    pncb->ncb_lsn=Num;        //会话编号
    pncb->ncb_buffer=pBuffer;
    pncb->ncb_length=Length;
	pncb->ncb_lana_num=lana;
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
    Netbios(pncb);
}
int main()
{   
	NCB ServerNncb;
    NCB ClientNcb;
    UCHAR LocalName[16];
    UCHAR NameNum,SessionNum;
    LANA_ENUM lenum;
    HANDLE hEvent;
    char buff[512];
    if (Lana_Enum(&lenum)!=NRC_GOODRET)
    {
        printf("LanaEnum Error!\n");
        return 0;
    }
    printf("Lana_num is: %d\n",lenum.lana[0]);
    if (RestNCB(lenum.lana[0])!=0)
    {
        printf("ResetNcb Error!");
		return 0;
    }
    printf("Reset NCB OK!\n");
    strcpy((char*)LocalName,"Client");
    if (AddName(LocalName,&NameNum,lenum.lana[0]))
    return 0;
    printf("Add Name OK!\n");
    hEvent=CreateEvent(NULL,FALSE,FALSE,"Client");//初始化为无效状态、自动恢复为无效状态
    if(hEvent==NULL)
    return 0;
    printf("Create Event OK!\n");
    printf("CALL...\n");
    for(;;)
    {
        NetbiosCall(&ClientNcb,(char*)LocalName,"Server");
        if (ClientNcb.ncb_cmd_cplt!=0)
        {
            printf("CALL.....\n");
            continue;
        }
        SessionNum=ClientNcb.ncb_lsn;//呼叫成功，得到本地会话号
        break;
    }
    SessionRecv(&ClientNcb,SessionNum,hEvent,(UCHAR *)buff);//收到数据，事件触发
    WaitForSingleObject(hEvent,0xffffffff);//等待服务器发来欢迎消息
    printf(buff);
    printf("\n");
	for(;;)
    {
        printf("Please input name to send:");
        memset(buff,0,512);
        scanf("%s",buff);
        if(strcmp(buff,"exit")==0)
            break;
		printf("Please input message to send:");
        memset(buff,0,512);
        scanf("%s",buff);
        if(strcmp(buff,"exit")==0)
            break;
        SessionSend(&ClientNcb,SessionNum,(UCHAR*)buff,strlen(buff),lenum.lana[0]);//会话发送
        memset(buff,0,512);
        SessionRecv(&ServerNncb,SessionNum,hEvent,(UCHAR*)buff);
        WaitForSingleObject(hEvent,0xffffffff);
		printf("%s \n",buff);
            printf("\n");
			memset(buff,0,512);
			continue;
		printf("%s \n",buff);
            printf("\n");
			memset(buff,0,512);
			continue;
	}                                                  //实际要发送的字节数
    HangupSession(SessionNum);//拆除会话
    return 0;
}
#endif