
// startBrowserDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "startBrowser.h"
#include "startBrowserDlg.h"
#include "afxdialogex.h"
#include <iostream>
#include <iomanip>//不要忘记包含此头文件
#include <vector>
#include"tlhelp32.h"
using namespace std;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
BOOL  LookUpProcessAndKill(const  vector<CString>  &vecProcessName)
{
	BOOL bRet = FALSE;
	HANDLE SnapShot, ProcessHandle;
	SHFILEINFO shSmall;
	PROCESSENTRY32 ProcessInfo;

	CString strSearchName;
	CString strRunProcessName;
	SnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (SnapShot != NULL)
	{
		ProcessInfo.dwSize = sizeof(ProcessInfo);  // 设置ProcessInfo的大小  
		BOOL Status = Process32First(SnapShot, &ProcessInfo);

		//提升升级工具进程权限为SE_DEBUG_NAME，否则XP下无法杀掉进程
		HANDLE hToken;
		if (OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken))
		{
			LUID luid;
			if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid))
			{
				TOKEN_PRIVILEGES TokenPrivileges;
				TokenPrivileges.PrivilegeCount = 1;
				TokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
				TokenPrivileges.Privileges[0].Luid = luid;
				AdjustTokenPrivileges(hToken, FALSE, &TokenPrivileges, 0, NULL, NULL);
			}
			CloseHandle(hToken);
		}

		while (Status)
		{
			// 获取进程文件信息  
			SHGetFileInfo(ProcessInfo.szExeFile, 0, &shSmall, sizeof(shSmall), SHGFI_ICON | SHGFI_SMALLICON);

			// 检测进程是否需要关闭
			strRunProcessName = ProcessInfo.szExeFile;
			strRunProcessName.MakeLower();
			for (vector<CString>::size_type it = 0; it != vecProcessName.size(); ++it)
			{
				strSearchName = vecProcessName[it];
				strSearchName.MakeLower();
				if (strSearchName == strRunProcessName)
				{
					// 获取进程句柄，强行关闭  
					ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessInfo.th32ProcessID);
					if (ProcessHandle != NULL)
					{
						bRet = TerminateProcess(ProcessHandle, 1);
						CloseHandle(ProcessHandle);
					}

					break;
				}

			}

			// 获取下一个进程的信息  
			Status = Process32Next(SnapShot, &ProcessInfo);
		}
	}

	return  bRet;
}

DWORD GetProcessidFromName(LPCTSTR name)
{
	PROCESSENTRY32 pe;
	DWORD id = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hSnapshot, &pe))
		return 0;
	while (1)
	{
		pe.dwSize = sizeof(PROCESSENTRY32);
		if (Process32Next(hSnapshot, &pe) == FALSE)
			break;
		if (wcscmp(pe.szExeFile, name) == 0)
		{
			id = pe.th32ProcessID;
			break;
		}


	}
	CloseHandle(hSnapshot);
	return id;
}
CString GetFileTitleFromFileName(CString fileName, BOOL ext=TRUE) {
	int Where = -1;
	Where = fileName.ReverseFind('\\');
	if (Where == -1) {
		Where = fileName.ReverseFind('/');
	}
	CString FileTitle = fileName.Right(fileName.GetLength() - 1 - Where); 
	if (!ext){
		int Which = FileTitle.ReverseFind('.'); 
		if (Which != -1) {
			FileTitle = FileTitle.Left(Which);
		}
	}
	return FileTitle;
}

CString * SplitString(CString str, char split, int& iSubStrs)
{
	int iPos = 0; //分割符位置  
	int iNums = 0; //分割符的总数  
	CString strTemp = str;
	CString strRight;
	//先计算子字符串的数量  
	while (iPos != -1)
	{
		iPos = strTemp.Find(split);
		if (iPos == -1)
		{
			break;
		}
		strRight = strTemp.Mid(iPos + 1, str.GetLength());
		strTemp = strRight;
		iNums++;
	}
	if (iNums == 0) //没有找到分割符  
	{
		//子字符串数就是字符串本身  
		iSubStrs = 1;
		return NULL;
	}
	//子字符串数组  
	iSubStrs = iNums + 1; //子串的数量 = 分割符数量 + 1  
	CString* pStrSplit;
	pStrSplit = new CString[iSubStrs];
	strTemp = str;
	CString strLeft;
	for (int i = 0; i < iNums; i++)
	{
		iPos = strTemp.Find(split);
		//左子串  
		strLeft = strTemp.Left(iPos);
		//右子串  
		strRight = strTemp.Mid(iPos + 1, strTemp.GetLength());
		strTemp = strRight;
		pStrSplit[i] = strLeft;
	}
	pStrSplit[iNums] = strTemp;
	return pStrSplit;
}

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CstartBrowserDlg 对话框



CstartBrowserDlg::CstartBrowserDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_STARTBROWSER_DIALOG, pParent)
	, timer_refresh(0)
	, mPeriod(0)
	, mUrls(NULL)
	, mTimes(0)
	, mUrlCunt(0)
	, mExePath(_T("C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe"))
	, mExeName(_T("chrome.exe"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CstartBrowserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CstartBrowserDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()	
	ON_BN_CLICKED(IDOK, &CstartBrowserDlg::OnBnClickedOk)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CstartBrowserDlg 消息处理程序

BOOL CstartBrowserDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	GetDlgItem(IDC_PATH)->SetWindowText(_T("C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe"));
	GetDlgItem(IDC_URL)->SetWindowText(_T("http://v.qq.com/x/cover/4mz2zy4nxjjqqbp/k0020goctd2.html?ptag=bl.zy.bs_dfzc_41.161026&volume=0\r\nhttp://v.qq.com/x/cover/4mz2zy4nxjjqqbp/p002077glsm.html?ptag=bl.zy.bs_dfzc_42.161026&volume=0\r\nhttps://v.qq.com/x/cover/4mz2zy4nxjjqqbp/e00203bcxtq.html?ptag=bl.zy.bs_dfzc_44.161026&volume=0"));
	GetDlgItem(IDC_TIMES)->SetWindowText(_T("10000"));
	GetDlgItem(IDC_PERIOD)->SetWindowText(_T("90"));
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CstartBrowserDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CstartBrowserDlg::OnPaint()
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
HCURSOR CstartBrowserDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void CstartBrowserDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	int period = 90; //s	
	CString csPeriod;
	GetDlgItem(IDC_PERIOD)->GetWindowText(csPeriod);
	if (!csPeriod.IsEmpty()) {
		//MessageBox(csPeriod);
		period = _tstoi(csPeriod);
	}

	CString csURL;
	CString *url = NULL;
	CString csTimes;
	CString csExePath;

	int urlCount = 0;
	int times = 10000; //time

	GetDlgItem(IDC_URL)->GetWindowText(csURL);
	GetDlgItem(IDC_TIMES)->GetWindowText(csTimes);
	if (!csTimes.IsEmpty()) {
		times = _tstoi(csTimes);
		cout << "times " << times;
		//MessageBox(csTimes);
		mTimes = times;
	}
	if (!csURL.IsEmpty()) {
		//MessageBox(csURL);
		//将字符串中的\r\n转换为;
		char spit = ';';
		csURL.Replace(_T("\r\n"), CString(spit));
		mUrls = SplitString(csURL, spit, mUrlCunt);
		if (mUrls == NULL) {
			mUrls = new CString[1];
			mUrls[0] = csURL;
		}
	}
	if (!csTimes.IsEmpty()) {
		times = _tstoi(csTimes);
		cout << "times " << times;
		//MessageBox(csTimes);
		mTimes = times;
	}
	GetDlgItem(IDC_PATH)->GetWindowText(csExePath);
	if (!csExePath.IsEmpty()) {
		mExePath = csExePath;
	}
	mExeName= GetFileTitleFromFileName(mExePath);
	/*
	for (int ii = 0; ii < mUrlCunt && NULL != mUrls; ii++) {
	}*/
	//ShellExecute(NULL, _T("open"), _T("C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe"), _T("--incognito https://v.qq.com/x/cover/4mz2zy4nxjjqqbp/e00203bcxtq.html?ptag=bl.zy.bs_dfzc_44.161026&volume=0"), NULL, SW_SHOW);
	KillTimer(TIMER_REFESHID);
	SetTimer(TIMER_REFESHID, period*1000, NULL);
	/*
	http://v.qq.com/x/cover/4mz2zy4nxjjqqbp/k0020goctd2.html?ptag=bl.zy.bs_dfzc_41.161026&volume=0;http://v.qq.com/x/cover/4mz2zy4nxjjqqbp/p002077glsm.html?ptag=bl.zy.bs_dfzc_42.161026&volume=0;https://v.qq.com/x/cover/4mz2zy4nxjjqqbp/e00203bcxtq.html?ptag=bl.zy.bs_dfzc_44.161026&volume=0
	*/
	
	/*
	ShellExecute(NULL, _T("open"), _T("C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe"), _T("https://v.qq.com/x/cover/4mz2zy4nxjjqqbp/e00203bcxtq.html?ptag=bl.zy.bs_dfzc_44.161026&volume=0"), NULL, SW_SHOW);
	ShellExecute(NULL, _T("open"), _T("C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe"), _T("https://v.qq.com/x/cover/4mz2zy4nxjjqqbp/e00203bcxtq.html?ptag=bl.zy.bs_dfzc_44.161026&volume=0"), NULL, SW_SHOW);
	ShellExecute(NULL, _T("open"), _T("C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe"), _T("https://v.qq.com/x/cover/4mz2zy4nxjjqqbp/e00203bcxtq.html?ptag=bl.zy.bs_dfzc_44.161026&volume=0"), NULL, SW_SHOW);
	ShellExecute(NULL, _T("open"), _T("C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe"), _T("https://v.qq.com/x/cover/4mz2zy4nxjjqqbp/e00203bcxtq.html?ptag=bl.zy.bs_dfzc_44.161026&volume=0"), NULL, SW_SHOW);
	*/
	//CDialogEx::OnOK();
}


void CstartBrowserDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	vector<CString> procs;
	procs.push_back(mExeName);
	CString title;
	
	/*
	C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe
	C:\Program Files\Internet Explorer\iexplore.exe
	C:\Program Files (x86)\Tencent\QQBrowser\QQBrowser.exe
	*/
	switch (nIDEvent)
	{
	case TIMER_REFESHID:
		// 如果m_nData1已经达到10，则销毁ID为1的定时器   		
		title.Format(_T("第%d次，共计%d次，执行中..."), timer_refresh+1,mTimes);
		SetWindowText(title);
		LookUpProcessAndKill(procs);		
		if (mTimes < timer_refresh)
		{
			KillTimer(1);
			break;
		}			
		timer_refresh++;

		for (int ii = 0; ii < mUrlCunt && NULL != mUrls; ii++) {
			ShellExecute(NULL, _T("open"), mExePath, mUrls[ii], NULL, SW_SHOW);
		}

		break;	
	default:
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}
