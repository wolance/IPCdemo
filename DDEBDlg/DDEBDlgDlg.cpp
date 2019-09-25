
// DDEBDlgDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DDEBDlg.h"
#include "DDEBDlgDlg.h"
#include "afxdialogex.h"
#include <Ddeml.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define NITEM 2 //定义ITEM的数量；
const char szApp[]="Server"; //server DDE服务名；
const char szTopic[]="Topic";//Server DDE目录名；
const char *pszItem[NITEM]={"Item1","Item2"};//SERVER ITEM名称字符串数组；
int count=0;//记数，在Static1中显示；
CString ServerData[NITEM];//存放服务器中的数据项内容；
HCONV hConv=0; //会话句柄；
DWORD idlnst=0; //DDEML实例句柄；
HWND hWnd; //窗口句柄；
HANDLE hlnst; //实例句柄；
HSZ hszApp=0; //SERVER服务字符串句柄；
HSZ hszTopic=0; //SERVER目录字符串句柄；
HSZ hszItem[NITEM]; //Server ITEM字符串句柄；
BOOL bConnect; // 建立连接标志；

//////////////////////////////////////DDE回调函数；
HDDEDATA CALLBACK DdeCallback(UINT wType,UINT wFmt,HCONV hConv,HSZ Topic,HSZ Item,
	HDDEDATA hData,DWORD lData1,DWORD lData2)
{
	int I;
	char tmp[255];
	switch(wType)
	{
	case XTYP_ADVSTART:
	case XTYP_CONNECT://请求连接；
		return ((HDDEDATA)TRUE);//允许；
	case XTYP_ADVDATA: //有数据到来；
		for(I=0;I<NITEM;I++)	
		{
			if(Item==hszItem[I])
			{
				DdeGetData(hData,(PBYTE)tmp,255,0);//取得数据；
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
		return ((HDDEDATA)DDE_FACK);//回执；
	case XTYP_ADVREQ:
	case XTYP_REQUEST://数据请求；
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

// CDDEBDlgDlg 对话框



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


// CDDEBDlgDlg 消息处理程序

BOOL CDDEBDlgDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	hWnd=m_hWnd;
	if (DdeInitialize(&idlnst,(PFNCALLBACK)DdeCallback,APPCMD_FILTERINITS|
		CBF_FAIL_EXECUTES|CBF_SKIP_CONNECT_CONFIRMS|CBF_FAIL_SELFCONNECTIONS|
		CBF_FAIL_POKES,0))
	{
		MessageBox("DDE SERVER初始化失败!");
		return FALSE;
	}
	hlnst=AfxGetApp()->m_hInstance;
	//创建DDE string
	hszApp=DdeCreateStringHandle(idlnst,szApp,0);
	hszTopic=DdeCreateStringHandle(idlnst,szTopic,0);
	for(int I=0;I<NITEM;I++)
		hszItem[I]=DdeCreateStringHandle(idlnst,pszItem[I],0);
	//注册服务；
	DdeNameService(idlnst,hszApp,0,DNS_REGISTER);
	bConnect=FALSE;
	SetTimer(1,1000,NULL);//开始定时；
	return TRUE; // return TRUE unless you set the focus to a control
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDDEBDlgDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDDEBDlgDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CDDEBDlgDlg::OnBnClickedOk()
{
	UpdateData();
	ServerData[0]=mStrEdit;
	DdePostAdvise(idlnst,hszTopic,hszItem[0]); //通知DDE更新该数据项目；
	if(!bConnect)//如果没有建立连接
	{
		hConv=DdeConnect(idlnst,hszApp,hszTopic,NULL);
		//连接服务器端；
		if(hConv) //如果建立成功
		{
			DWORD dwResult;
			bConnect=TRUE;
			for(int I=0;I<NITEM;I++)
				DdeClientTransaction(NULL,0,hConv,hszItem[I],CF_TEXT,XTYP_ADVSTART,
				TIMEOUT_ASYNC,&dwResult);
		}
	}
}
