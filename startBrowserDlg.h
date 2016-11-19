
// startBrowserDlg.h : 头文件
//

#pragma once


// CstartBrowserDlg 对话框
class CstartBrowserDlg : public CDialogEx
{
// 构造
public:
	CstartBrowserDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_STARTBROWSER_DIALOG };
#endif
	
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
