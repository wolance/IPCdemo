
// DDEBDlgDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CDDEBDlgDlg �Ի���
class CDDEBDlgDlg : public CDialogEx
{
// ����
public:
	CDDEBDlgDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_DDEBDLG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString mStrEdit;
	CListBox mCtrlListBox;
	afx_msg void OnBnClickedOk();
};
