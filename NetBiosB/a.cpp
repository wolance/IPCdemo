#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <nb30.h>
#include <memory.h>
#pragma comment(lib,"netapi32.lib")
#define MAXSESSIONS 254
#define MAXNAMES 254
#define NBCheck(x) if (NRC_GOODRET != x.ncb_retcode) { \
	printf("Line %d: Got 0x%x from NetBios()\n", \
	__LINE__, x.ncb_retcode); \
}
#define PNBCheck(x) if (NRC_GOODRET != x->ncb_retcode) { \
	printf("Line %d: Got 0x%x from NetBios()\n", \
	__LINE__, x->ncb_retcode); \
}
#define Online 1// A user is on line
#define Offline 2// A user is off line
#define Listen 3// A user is waiting for connection
#define Message 4// A user is on line

	typedef struct DataGram
{// the format of a datagram
	UCHAR flag;// the type of message
	char message[256];//the content of message
}DataGram,*PDataGram;
char LocalName[NCBNAMSZ]; //local name
char GroupName[NCBNAMSZ]; //group name

UCHAR NameNum;
UCHAR GroupNameNum;
#define DGLEN sizeof(struct DataGram)

void MakeNetbiosName(char *achDest, char *szSrc)
{
	int cchSrc;
	cchSrc =strlen(szSrc);
	if (cchSrc > NCBNAMSZ) cchSrc = NCBNAMSZ;
	memset(achDest, ' ', NCBNAMSZ);
	memcpy(achDest, szSrc, cchSrc);
	if (cchSrc!=NCBNAMSZ) achDest[cchSrc]='\0';
}

/******************** the initialization of the network device *****************************/
UCHAR LanaEnum(LANA_ENUM *lenum)
{
	NCB ncb;
	UCHAR uRetCode;
	memset( &ncb, 0, sizeof(NCB) );
	ncb.ncb_command = NCBENUM;
	ncb.ncb_buffer = (UCHAR *)lenum;
	ncb.ncb_length = sizeof(LANA_ENUM);
	uRetCode = Netbios( &ncb );
	NBCheck(ncb)
		return uRetCode;
}

UCHAR RestNCB(UCHAR jiangxue_lana)
{
	NCB jiangxue_ncb;
	memset(&jiangxue_ncb,0,sizeof(NCB));
	jiangxue_ncb.ncb_command=NCBRESET;
	jiangxue_ncb.ncb_callname[0]=MAXSESSIONS;
	jiangxue_ncb.ncb_callname[2]=MAXNAMES;
	// jiangxue_ncb.ncb_callname[3]=TRUE;
	jiangxue_ncb.ncb_lana_num=jiangxue_lana;
	Netbios(&jiangxue_ncb);
	NBCheck(jiangxue_ncb)
		return(jiangxue_ncb.ncb_cmd_cplt);
}
/*************** the status description of the network device ****************/
UCHAR Astatus(UCHAR jiangxue_lana, PVOID pBuffer, int cbBuffer,char *szName)
{
	NCB jiangxue_ncb;
	ZeroMemory(&jiangxue_ncb,sizeof(NCB));
	jiangxue_ncb.ncb_command=NCBASTAT;
	jiangxue_ncb.ncb_lana_num=jiangxue_lana;
	jiangxue_ncb.ncb_buffer=(PUCHAR)pBuffer;
	jiangxue_ncb.ncb_length=cbBuffer;
	strcpy((char *)jiangxue_ncb.ncb_callname,szName);
	Netbios(&jiangxue_ncb);
	NBCheck(jiangxue_ncb)
		return(NRC_GOODRET==jiangxue_ncb.ncb_retcode);
}

BOOL Adapterlist(UCHAR nlana, char *szName)
{//get the state of each adapter with some protocol.
	int cbBuffer;
	ADAPTER_STATUS *pStatus;
	NAME_BUFFER *pNames;
	int i;
	HANDLE hHeap;
	char cname[16];
	hHeap=GetProcessHeap();
	//allocate the largest buffer that might be needed.
	cbBuffer=sizeof(ADAPTER_STATUS)+255*sizeof(NAME_BUFFER);
	pStatus=(ADAPTER_STATUS *)HeapAlloc(hHeap,0, cbBuffer);
	if (NULL==pStatus) return FALSE;
	if(!Astatus(nlana,(PVOID)pStatus,cbBuffer,szName))
	{
		HeapFree(hHeap, 0, pStatus);
		return FALSE;
	}
	//list the adapter status structure and names
	if(pStatus->adapter_type==0xFE)
		strcpy(cname,"Ethernet");
	else strcpy(cname,"The other type");
	printf("The %s Number on LANA %d with version %d.%d is: %02x%02x%02x%02x%02x%02x\n",
		cname, nlana,
		pStatus->rev_major,
		pStatus->rev_minor,
		pStatus->adapter_address[0],
		pStatus->adapter_address[1],
		pStatus->adapter_address[2],
		pStatus->adapter_address[3],
		pStatus->adapter_address[4],
		pStatus->adapter_address[5] );
	printf("The number of names in the local names table is %d\n",pStatus->name_count);
	pNames=(NAME_BUFFER *)(pStatus+1);
	for (i=0;i<pStatus->name_count;i++)
		printf("\t%.*s\n",NCBNAMSZ,pNames[i].name);
	HeapFree(hHeap,0,pStatus);
	return TRUE;
}

/*************** naming service of the communicating parties ****************/
UCHAR AddName(char * kemin_LocalName, UCHAR kemin_lana, UCHAR *kemin_LocalNameNum)
{
	NCB kemin_ncb;
	memset(&kemin_ncb,0,sizeof(NCB));
	kemin_ncb.ncb_command=NCBADDNAME;
	kemin_ncb.ncb_lana_num=kemin_lana;
	strcpy((char *)kemin_ncb.ncb_name,kemin_LocalName);
	Netbios(&kemin_ncb);
	NBCheck(kemin_ncb)
		*kemin_LocalNameNum=kemin_ncb.ncb_num;
	return(kemin_ncb.ncb_cmd_cplt);
}

UCHAR AddGroupName(char * limin_GroupName, UCHAR limin_lana, UCHAR *limin_GroupNameNum)
{
	NCB limin_ncb;
	memset(&limin_ncb,0,sizeof(NCB));
	limin_ncb.ncb_command=NCBADDGRNAME;
	limin_ncb.ncb_lana_num=limin_lana;
	strcpy((char *)limin_ncb.ncb_name,limin_GroupName);
	Netbios(&limin_ncb);
	NBCheck(limin_ncb)
		*limin_GroupNameNum=limin_ncb.ncb_num;
	return(limin_ncb.ncb_cmd_cplt);
}

UCHAR DeleteName(char *liufei_LocalName, UCHAR liufei_lana)
{
	NCB liufei_ncb;
	memset(&liufei_ncb,0,sizeof(NCB));
	liufei_ncb.ncb_command=NCBDELNAME;
	liufei_ncb.ncb_lana_num=liufei_lana;
	strcpy((char *)liufei_ncb.ncb_name,liufei_LocalName);
	Netbios(&liufei_ncb);
	NBCheck(liufei_ncb)
		return(liufei_ncb.ncb_cmd_cplt);
}

/*********** connection-orented session service of the communicating parties ************/
UCHAR NetbiosListen(int lana, PNCB pncb, char *SourName)
{
	memset(pncb,0,sizeof(NCB));
	pncb->ncb_command=NCBLISTEN;//synchronous call
	strcpy((char *)pncb->ncb_name,SourName);
	strcpy((char *)pncb->ncb_callname,"*");
	pncb->ncb_lana_num=lana;
	//pncb->ncb_rto=0;
	//pncb->ncb_sto=5;
	return Netbios(pncb);
}

UCHAR NetbiosCall(PNCB yixiong_pncb,UCHAR lana,char *yixiong_SourName, char *yixiong_DestName)
{
	memset(yixiong_pncb,0,sizeof(NCB));
	yixiong_pncb->ncb_command=NCBCALL;
	yixiong_pncb->ncb_lana_num=lana;
	strcpy((char *)yixiong_pncb->ncb_name, yixiong_SourName);
	strcpy((char *)yixiong_pncb->ncb_callname, yixiong_DestName);
	//yixiong_pncb->ncb_rto=0;
	//yixiong_pncb->ncb_sto=10;
	return Netbios(yixiong_pncb);
}

UCHAR SessionSend(UCHAR lana, UCHAR zhangke_SessionNum, char *pBuffer, WORD Length)
{
	NCB zhangke_ncb;
	memset(&zhangke_ncb,0,sizeof(NCB));
	zhangke_ncb.ncb_lana_num = lana;
	zhangke_ncb.ncb_command=NCBSEND;
	zhangke_ncb.ncb_lsn=zhangke_SessionNum;
	zhangke_ncb.ncb_buffer=(UCHAR *)pBuffer;
	zhangke_ncb.ncb_length=Length;
	return Netbios(&zhangke_ncb);
}

UCHAR SessionRecv(UCHAR lana, UCHAR guojun_SessionNum, char *pBuffer)
{
	NCB guojun_ncb;
	memset(&guojun_ncb,0,sizeof(NCB));
	guojun_ncb.ncb_lana_num = lana;
	guojun_ncb.ncb_command=NCBRECV;
	guojun_ncb.ncb_lsn=guojun_SessionNum;
	guojun_ncb.ncb_buffer=(UCHAR *)pBuffer;
	guojun_ncb.ncb_length=512;
	return Netbios(&guojun_ncb);
}

UCHAR HangupSession(UCHAR wanglinjia_SessionNum)
{
	NCB wanglinjia_ncb;
	UCHAR ucRc;
	memset(&wanglinjia_ncb,0,sizeof(NCB));
	wanglinjia_ncb.ncb_command=NCBHANGUP;
	wanglinjia_ncb.ncb_lsn=wanglinjia_SessionNum;
	ucRc=Netbios(&wanglinjia_ncb);
	return(ucRc);
}

/*********** connection-less datagram service of the communicating parties ************/
UCHAR SDGSend(UCHAR lana,char *wugang_DestName, UCHAR wugang_LocalNameNum, char *pBuffer, WORD Length)
{
	NCB wugang_ncb;
	UCHAR ucRc;
	memset(&wugang_ncb,0,sizeof(NCB));
	wugang_ncb.ncb_command=NCBDGSEND;
	strcpy((char *)wugang_ncb.ncb_callname, wugang_DestName);
	wugang_ncb.ncb_num=wugang_LocalNameNum;
	wugang_ncb.ncb_buffer=(UCHAR *)pBuffer;
	wugang_ncb.ncb_length=Length;
	wugang_ncb.ncb_lana_num = lana;
	ucRc=Netbios(&wugang_ncb);
	NBCheck(wugang_ncb)
		return(ucRc);
}

UCHAR ADGSend(UCHAR lana,char *DestName, UCHAR LocalNameNum, HANDLE hEvent, char *pBuffer, WORD Length)
{
	NCB ncb;
	memset(&ncb,0,sizeof(NCB));
	ncb.ncb_command=NCBDGSEND|ASYNCH;
	ncb.ncb_lana_num=lana;
	strcpy((char *)ncb.ncb_callname, DestName);
	ncb.ncb_num=LocalNameNum;
	ncb.ncb_buffer=(UCHAR *)pBuffer;
	ncb.ncb_length=Length;
	ncb.ncb_cmd_cplt=0xff;
	ncb.ncb_event=hEvent;
	Netbios(&ncb);
	return ncb.ncb_cmd_cplt;
}
UCHAR SDGRecv(UCHAR lana,UCHAR wangyan_Num,char *DestName, char *pBuffer, WORD *len)
{
	NCB wangyan_ncb;
	memset(&wangyan_ncb,0,sizeof(NCB));
	wangyan_ncb.ncb_command=NCBDGRECV;
	wangyan_ncb.ncb_lana_num=lana;
	wangyan_ncb.ncb_num=wangyan_Num;
	wangyan_ncb.ncb_buffer=(UCHAR *)pBuffer;
	wangyan_ncb.ncb_length=*len;
	Netbios(&wangyan_ncb);
	NBCheck(wangyan_ncb)
		strcpy(DestName,(char *)wangyan_ncb.ncb_callname);
	return wangyan_ncb.ncb_cmd_cplt;
}

/******* connection-less datagram broadcast service of the communicating parties ********/
UCHAR DatagramSendBC(UCHAR lana, UCHAR num,char * buf, WORD len)
{//send the broadcasting datagram from the given name-based number
	NCB ncb;
	ZeroMemory(&ncb,sizeof(NCB));
	ncb.ncb_command=NCBDGSENDBC;
	ncb.ncb_lana_num=lana;
	ncb.ncb_num=num;
	ncb.ncb_buffer=(PUCHAR)buf;
	ncb.ncb_length=len;
	Netbios(&ncb);
	NBCheck(ncb)
		return ncb.ncb_retcode;
}

UCHAR DatagramRecvBC(UCHAR lana, UCHAR num,char *buf, WORD *len, char *Callname)
{//receive the broadcasting datagram from the given name-based number
	NCB ncb;
	ZeroMemory(&ncb,sizeof(NCB));
	ncb.ncb_command=NCBDGRECVBC;
	ncb.ncb_lana_num=lana;
	ncb.ncb_num=num;
	ncb.ncb_buffer=(PUCHAR)buf;
	ncb.ncb_length=*len;
	Netbios(&ncb);
	NBCheck(ncb)
		*len=ncb.ncb_length;
	strcpy(Callname,(char *)ncb.ncb_callname);
	return ncb.ncb_retcode;
}

/******* command processing ********/
int CommandTranslating(const char *Command)//return the different value for the different command
{
	if(strcmp(Command,"lookup")==0) return 1;//look up the status of the locall adapter
	if(strcmp(Command,"session")==0) return 2;//session communication
	if(strcmp(Command,"sendtoname")==0) return 3;//unicast or multicast communication
	if(strcmp(Command,"sendtoall")==0) return 4;//broadcast communication
	if(strcmp(Command,"exit")==0) return 5;// quit
	return 0;
}

UCHAR Elana[10];
UCHAR Elength=0;
LANA_ENUM lenum;
/*********** connection-orented session service of the communicating parties ************/
/*
void CALLBACK ListenCallBack(PNCB);
int SlaveListen(int lana, char *SourName)
{
int retcode;
PNCB pncb=NULL;
pncb=(PNCB)malloc(sizeof(NCB));// heap allocation
memset(pncb,0,sizeof(NCB));
pncb->ncb_command=NCBLISTEN|ASYNCH;//asynchronous call
strcpy((char *)pncb->ncb_name,SourName);
strcpy((char *)pncb->ncb_callname,"*");
pncb->ncb_lana_num=lana;
pncb->ncb_post=ListenCallBack;//asynchronous function
//pncb->ncb_rto=0;
//pncb->ncb_sto=5;
retcode=Netbios(pncb);
return retcode;
}

void SServerSession(PNCB);

void CALLBACK ListenCallBack(PNCB pncb)//asynchronous call function
{
if(pncb->ncb_retcode!=NRC_GOODRET) return;
SlaveListen(Elana[0],LocalName);
SServerSession(pncb);
return;
}
*/
DWORD WINAPI SessionReceiveThread(LPVOID param)//the thread to receive the data for the session
{
	PNCB pncb=(PNCB)param;
	char buffer[512];
	UCHAR SessionNum;
	UCHAR retcode;
	char DestName[NCBNAMSZ];
	SessionNum=pncb->ncb_lsn;
	memset(DestName,'\0',NCBNAMSZ);
	strcpy(DestName, (char *)pncb->ncb_callname);
	for(;;)
	{
		memset(buffer,'\0', 512);
		retcode=SessionRecv(Elana[0], SessionNum, buffer);
		if(retcode==0)
		{
			printf("\n%s says to you(on session):",DestName);
			puts(buffer);
			printf("please input message to send to %s:",DestName);
			continue;
		}
		else
		{
			printf("\n%s leaves the connection to you!\n",DestName);
			HangupSession(SessionNum);
			break;
		}
	}
	return 1;
}

void ServerSession(PNCB pncb)
{
	HANDLE hSThread;
	DWORD dwSExitCode;
	DWORD id;
	char sbuffer[512];
	UCHAR retcode;
	UCHAR SessionNum;
	char DestName[NCBNAMSZ];
	SessionNum=pncb->ncb_lsn;
	memset(DestName,'\0',NCBNAMSZ);
	strcpy(DestName, (char *)pncb->ncb_callname);
	hSThread=CreateThread(NULL,0,SessionReceiveThread,(PVOID)pncb,0,&id);//create a thread to receive the message
	dwSExitCode=0;
	retcode=0;
	printf("%s has accepted the connection to %s!\n",LocalName, DestName);
	Sleep(1000);
	memset(sbuffer,'\0',512);
	strcpy(sbuffer, "Welcome to Server!");
	SessionSend(Elana[0], SessionNum, sbuffer, sizeof(sbuffer));
	printf("When you send a message to the other party,\n");
	printf("the message \"exit\" means stopping to send!\n\n");
	for(;;)
	{
		printf("please input message to send to %s:",DestName);
		memset(sbuffer,'\0',512);
		gets(sbuffer);
		if(strcmp(sbuffer,"exit")==0)
		{
			TerminateThread(hSThread,dwSExitCode);
			CloseHandle(hSThread);
			HangupSession(SessionNum);
			break;
		}
		retcode=SessionSend(Elana[0], SessionNum, sbuffer, sizeof(sbuffer));
		if(retcode!=0) break;
	}
}

void ClientSession(PNCB pncb)
{
	HANDLE hCThread;
	DWORD dwCExitCode;
	DWORD id;
	char cbuffer[512];
	UCHAR retcode;
	UCHAR SessionNum;
	char DestName[NCBNAMSZ];
	SessionNum=pncb->ncb_lsn;
	memset(DestName,'\0',NCBNAMSZ);
	strcpy(DestName, (char *)pncb->ncb_callname);
	hCThread=CreateThread(NULL,0,SessionReceiveThread,(PVOID)pncb,0,&id);//create a thread to receive the message
	dwCExitCode=0;
	retcode=0;
	printf("%s has connected to the server %s!\n",LocalName, DestName);
	printf("When you send a message to the other party,\n");
	printf("the message \"exit\" means stopping to send!\n");
	Sleep(1000);
	for(;;)
	{
		memset(cbuffer,'\0',512);
		gets(cbuffer);
		if(strcmp(cbuffer,"exit")==0)
		{
			TerminateThread(hCThread,dwCExitCode);
			CloseHandle(hCThread);
			HangupSession(SessionNum);
			break;
		}
		retcode=SessionSend(Elana[0], SessionNum, cbuffer, sizeof(cbuffer));
		if(retcode!=0) break;
		printf("please input message to send to %s:",DestName);
	}
}
/*********** connection-less datagram service of the communicating parties ************/
DWORD WINAPI GDGRecvTread(LPVOID param)//receive the message from the group
{
	WORD len=DGLEN;
	char Gbuffer[DGLEN];
	PDataGram pdata=(PDataGram)Gbuffer;
	char GCallName[NCBNAMSZ];
	while(1)
	{
		memset(GCallName, '\0',NCBNAMSZ);
		if(SDGRecv(Elana[0],GroupNameNum,GCallName,Gbuffer,&len)==NRC_GOODRET)
		{
			printf("\nMulticasting sender's name: %s\n",GCallName);
			printf("the content of message: %s\n",pdata->message);
			printf("$");
		}
	}
	return 1;
}

DWORD WINAPI SDGRecvTread(LPVOID param)//receive the message from Single user
{//receive the information from the given name number,
	//if the name number is set to 0xff, then receive all the local data
	WORD len1=DGLEN;
	char buffer1[DGLEN];
	PDataGram pdata1=(PDataGram)buffer1;
	char CallName1[NCBNAMSZ];
	while(1)
	{
		memset(CallName1, '\0',NCBNAMSZ);
		if(SDGRecv(Elana[0],NameNum,CallName1,buffer1,&len1)==NRC_GOODRET)
		{
			if(pdata1->flag==Message)
			{
				printf("\nUnicasting sender's name: %s\n",CallName1);
				printf("the content of message: %s\n",pdata1->message);
				printf("$");
			}
			if(pdata1->flag==Online)
			{
				printf("\nThe user %s is on line and has the group name %s!\n$", CallName1, pdata1->message);
			}
		}
	}
	return 1;
}


void DGsendCMD(UCHAR lana, UCHAR NameNum, char * DestName)
{
	char buff[256];
	char buffer[DGLEN];
	WORD len=DGLEN;
	PDataGram pdata=(PDataGram)buffer;
	printf("please input message to send to the name %s:",DestName);
	memset(buff,'\0',256);
	gets(buff);
	pdata->flag=Message;
	strcpy(pdata->message,buff);
	SDGSend(lana,DestName, NameNum, (char *)pdata, len);
}
/******* connection-less datagram broadcast service of the communicating parties *******/
DWORD WINAPI DGRecvBCTread(LPVOID param)//receive the broadcasting message from single user
{//receive the information from the given name number,
	WORD len;
	char buffer[DGLEN];
	char buff[DGLEN];
	PDataGram pdata1=(PDataGram)buffer;
	PDataGram pdata2=(PDataGram)buff;
	char DestName[NCBNAMSZ];

	while(1)
	{
		len=DGLEN;
		memset(DestName, '\0',NCBNAMSZ);
		if(DatagramRecvBC(Elana[0], NameNum,buffer, &len, DestName)!=NRC_GOODRET) continue;
		switch(pdata1->flag)
		{
		case Message:// Ordinary message
			if(strcmp(DestName,LocalName)==0)
			{
				printf("\nBroadcasting message has sent out!\n<cpmmand>");
				break;
			}
			printf("\nThe user %s broadcasts the message:%s\n$", DestName, pdata1->message);
			break;
		case Online:// a user comes on line
			if(strcmp(DestName,LocalName)==0) break;
			printf("\nThe user %s goes on line and has the group name %s!\n$", DestName, pdata1->message);
			Sleep(1000);
			len=DGLEN;
			pdata2->flag=Online;
			memset(pdata2->message,'\0',256);
			strcpy(pdata2->message,GroupName);
			SDGSend(Elana[0], DestName, NameNum, (char *)pdata2, len);
			break;
		case Offline://a user gets off line
			if(strcmp(DestName,LocalName)==0) break;
			printf("\nThe user %s has been off line!\n$", DestName);
			break;
		case Listen://a user is waiting for a connection
			if(strcmp(DestName,LocalName)==0)
			{
				printf("Listening message has sent out!\n");
				break;
			}
			printf("\nThe server %s is waiting for a client to connect!\n$", DestName);
			break;
		}
	}
	return 1;
}

void DGsendBCCMD(UCHAR lana, UCHAR NameNum)
{
	char buff[256];
	char buffer[DGLEN];
	WORD len=DGLEN;
	PDataGram pdata=(PDataGram)buffer;
	printf("please input message to broadcast:");
	memset(buff,'\0',256);
	gets(buff);
	pdata->flag=Message;
	memset(pdata->message,'\0',256);
	strcpy(pdata->message,buff);
	DatagramSendBC(lana, NameNum,(char *)pdata, len);
}

int main(void)
{
	DWORD dwExitCode1,dwExitCode2,dwExitCode3;
	DWORD id;
	HANDLE hThread1,hThread2,hThread3;
	char command[64];
	/******************** the initialization of the network device *****************************/
	UCHAR uRetCode=0;
	int i;
	char sKey[64];
	dwExitCode1=0;
	dwExitCode2=0;
	dwExitCode3=0;
	//learn The information of the adapter resource!
	uRetCode=LanaEnum(&lenum);
	if (uRetCode!=NRC_GOODRET) return -1;
	printf("Adapter is normal!\n");
	for (i=0; i<lenum.length;i++)
	{//reset the each adapter with protocol.
		uRetCode=RestNCB(lenum.lana[i]);
		if (uRetCode==NRC_GOODRET)
		{
			Elana[Elength]=lenum.lana[i];
			Elength++;
		}
	};
	if (Elength==0)
	{
		printf("NCB reseting fails\n");
		getchar();
		return -1;
	}
	printf("The length of lenum is %d \n",Elength);
	printf("NCB reseting is OK\n");
	/*************** naming service of the communicating parties ****************/
	printf("Enter the local name:");
	scanf("%s",sKey);
	MakeNetbiosName(LocalName,sKey);
	fflush(stdin);
	printf("Enter the group name:");
	scanf("%s",sKey);
	MakeNetbiosName(GroupName,sKey);
	fflush(stdin);
	if (AddName(LocalName,Elana[0],&NameNum)!=NRC_GOODRET)
	{
		printf("Name adding is going wrong!\n");
		getchar();
		return -1;
	}
	printf("Name adding is OK!\n");
	if (AddGroupName(GroupName,Elana[0],&GroupNameNum)!=NRC_GOODRET)
	{
		printf("Group Name adding is going wrong!\n");
		getchar();
		return -1;
	}
	printf("Group Name adding is OK!\n");
	hThread1=CreateThread(NULL,0,DGRecvBCTread,NULL,0,&id);// create a thread to receive the broadcast messages
	hThread2=CreateThread(NULL,0,GDGRecvTread,NULL,0,&id);// create a thread to receive the multicast messages
	hThread3=CreateThread(NULL,0,SDGRecvTread,NULL,0,&id);// create a thread to receive the unicast messages
	Sleep(3000);
	system("cls");
	printf("The locall name: %s and the group name: %s\n", LocalName,GroupName);
	printf("*******************************************************\n");
	printf("\t\tcommand prompt:$\n lookup: learn the information about the adapter\n session: setup the session communication \
		   \nsendtoname: send a datagram to the given name\n sendtoall: send a datagram to all the users on LAN\n exit: quit\n");
	printf("*******************************************************\n");

	WORD len=DGLEN;
	DataGram data;
	PDataGram pdata=(PDataGram)&data;
	char CallName[NCBNAMSZ];
	NCB ncb;
	memset(&ncb,0,sizeof(NCB));
	PNCB pncb=&ncb;
	pdata->flag=Online;
	strcpy(pdata->message,GroupName);
	DatagramSendBC(Elana[0], NameNum,(char *)pdata, len);
	while(1)
	{
		printf("$");
		scanf("%s", command);
		fflush(stdin);
		switch(CommandTranslating(command))
		{
		case 1://learn the information about the adapter
			printf("If you type \"l\",it means localname otherwise the adapter\n" );
			printf("Please enter your choice:" );
			scanf("%s",sKey);
			fflush(stdin);
			if (strcmp(sKey,"l")==0) Adapterlist(Elana[0],LocalName);
			else for (i=0;i<Elength;i++) Adapterlist(Elana[i],"*");
			Sleep(999);
			break;
		case 2://setup the session communication
			printf("Enter the character * (to listen as a server) \n");
			printf("or the server's name( to connect as a client ):");
			scanf("%s",sKey);
			MakeNetbiosName(CallName, sKey);
			fflush(stdin);
			if(strcmp(CallName,"*")==0)
			{
				pdata->flag=Listen;
				memset(pdata->message,'\0',256);
				strcpy(pdata->message,"**");
				DatagramSendBC(Elana[0], NameNum,(char *)pdata, len);
				while(1)
				{
					NetbiosListen(Elana[0],pncb, LocalName);
					ServerSession(pncb);
					printf("If you need listen to the next, please type the letter \"y\":");
					scanf("%s",sKey);
					if(strcmp(sKey,"y")!=0) break;
				}
			}
			else
			{
				uRetCode=NetbiosCall(pncb,Elana[0],LocalName, CallName);
				if (uRetCode==0) ClientSession(pncb);
				else printf("The server %s isn't listenning!\n",CallName);
			}
			Sleep(3000);
			break;
		case 3://send a datagram to the given name
			printf("Enter the receiver's or group's name:");
			scanf("%s",sKey);
			MakeNetbiosName(CallName, sKey);
			fflush(stdin);
			DGsendCMD(Elana[0], NameNum, CallName);
			Sleep(999);
			break;
		case 4://send a datagram to all the users on LAN
			DGsendBCCMD(Elana[0], NameNum);
			Sleep(999);
			break;
		case 5://quit
			TerminateThread(hThread1,dwExitCode1);
			TerminateThread(hThread2,dwExitCode2);
			TerminateThread(hThread3,dwExitCode3);
			pdata->flag=Offline;
			strcpy(pdata->message, GroupName);
			DatagramSendBC(Elana[0], NameNum,(char *)pdata, len);
			CloseHandle(hThread1);
			CloseHandle(hThread2);
			CloseHandle(hThread3);
			DeleteName(GroupName, Elana[0]);
			DeleteName(LocalName, Elana[0]);
			return 1;
		default:
			printf("You type a wrong command!\n");
			Sleep(999);
			break;
		}
	}
	getchar();
	return 0;
} 