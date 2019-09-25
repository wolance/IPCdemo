
// DDEBDlgDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DDEBDlg.h"
#include "DDEBDlgDlg.h"
#include "afxdialogex.h"
#include <Ddeml.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define NITEM 2 //����ITEM��������
const char szApp[]="Server"; //server DDE��������
const char szTopic[]="Topic";//Server DDEĿ¼����
const char *pszItem[NITEM]={"Item1","Item2"};//SERVER ITEM�����ַ������飻
int count=0;//��������Static1����ʾ��
CString ServerData[NITEM];//��ŷ������е����������ݣ�
HCONV hConv=0; //�Ự�����
DWORD idlnst=0; //DDEMLʵ�������
HWND hWnd; //���ھ����
HANDLE hlnst; //ʵ�������
HSZ hszApp=0; //SERVER�����ַ��������
HSZ hszTopic=0; //SERVERĿ¼�ַ��������
HSZ hszItem[NITEM]; //Server ITEM�ַ��������
BOOL bConnect; // �������ӱ�־��

//////////////////////////////////////DDE�ص�������
HDDEDATA CALLBACK DdeCallback(UINT wType,UINT wFmt,HCONV hConv,HSZ Topic,HSZ Item,
	HDDEDATA hData,DWORD lData1,DWORD lData2)
{
	int I;
	char tmp[255];
	switch(wType)
	{
	case XTYP_ADVSTART:
	case XTYP_CONNECT://�������ӣ�
		return ((HDDEDATA)TRUE);//����
	case XTYP_ADVDATA: //�����ݵ�����
		for(I=0;I<NITEM;I++)	
		{
			if(Item==hszItem[I])
			{
				DdeGetData(hData,(PBYTE)tmp,255,0);//ȡ�����ݣ�
				switch(I)
				{
				case 0:
					CListBox *pListBox = (CListBox*)CWnd::FromHandle( GetDlgItem(hWnd,IDC_LIST1));
					CString str = "DDEA: " + CString(tmp);
					pListBox->AddString(str);
					break;
				}
			}
		}
		return ((HDDEDATA)DDE_FACK);//��ִ��
	case XTYP_ADVREQ:
	case XTYP_REQUEST://��������
		for(I=0;I<NITEM;I++)
		{
			if(Item==hszItem[I])
			{
				CListBox *pListBox = (CListBox*)CWnd::FromHandle( GetDlgItem(hWnd,IDC_LIST1));
				CString str = "DDEB: " + ServerData[I];
				pListBox->AddString(str);
				return DdeCreateDataHandle(idlnst,(PBYTE)(LPCTSTR)ServerData[I],
					ServerData[I].GetLength()+1,0,Item,wFmt,0);
			}
		}
	}
	return(0);
}

// CDDEBDlgDlg �Ի���



CDDEBDlgDlg::CDDEBDlgDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDDEBDlgDlg::IDD, pParent)
	, mStrEdit(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDDEBDlgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, mStrEdit);
	DDX_Control(pDX, IDC_LIST1, mCtrlListBox);
}

BEGIN_MESSAGE_MAP(CDDEBDlgDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CDDEBDlgDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CDDEBDlgDlg ��Ϣ�������

BOOL CDDEBDlgDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	hWnd=m_hWnd;
	if (DdeInitialize(&idlnst,(PFNCALLBACK)DdeCallback,APPCMD_FILTERINITS|
		CBF_FAIL_EXECUTES|CBF_SKIP_CONNECT_CONFIRMS|CBF_FAIL_SELFCONNECTIONS|
		CBF_FAIL_POKES,0))
	{
		MessageBox("DDE SERVER��ʼ��ʧ��!");
		return FALSE;
	}
	hlnst=AfxGetApp()->m_hInstance;
	//����DDE string
	hszApp=DdeCreateStringHandle(idlnst,szApp,0);
	hszTopic=DdeCreateStringHandle(idlnst,szTopic,0);
	for(int I=0;I<NITEM;I++)
		hszItem[I]=DdeCreateStringHandle(idlnst,pszItem[I],0);
	//ע�����
	DdeNameService(idlnst,hszApp,0,DNS_REGISTER);
	bConnect=FALSE;
	SetTimer(1,1000,NULL);//��ʼ��ʱ��
	return TRUE; // return TRUE unless you set the focus to a control
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CDDEBDlgDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CDDEBDlgDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CDDEBDlgDlg::OnBnClickedOk()
{
	UpdateData();
	ServerData[0]=mStrEdit;
	DdePostAdvise(idlnst,hszTopic,hszItem[0]); //֪ͨDDE���¸�������Ŀ��
	if(!bConnect)//���û�н�������
	{
		hConv=DdeConnect(idlnst,hszApp,hszTopic,NULL);
		//���ӷ������ˣ�
		if(hConv) //��������ɹ�
		{
			DWORD dwResult;
			bConnect=TRUE;
			for(int I=0;I<NITEM;I++)
				DdeClientTransaction(NULL,0,hConv,hszItem[I],CF_TEXT,XTYP_ADVSTART,
				TIMEOUT_ASYNC,&dwResult);
		}
	}
}
