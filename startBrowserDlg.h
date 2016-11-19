
// startBrowserDlg.h : ͷ�ļ�
//

#pragma once


// CstartBrowserDlg �Ի���
class CstartBrowserDlg : public CDialogEx
{
// ����
public:
	CstartBrowserDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_STARTBROWSER_DIALOG };
#endif
	
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedOk();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
private:
	enum { TIMER_REFESHID = 1 };
	int timer_refresh;
	int mPeriod;	
	int mTimes;
	CString* mUrls;
	int mUrlCunt;
	CString mExePath;
	CString mExeName;
};
