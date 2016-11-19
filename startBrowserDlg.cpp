
// startBrowserDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "startBrowser.h"
#include "startBrowserDlg.h"
#include "afxdialogex.h"
#include <iostream>
#include <iomanip>//��Ҫ���ǰ�����ͷ�ļ�
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
		ProcessInfo.dwSize = sizeof(ProcessInfo);  // ����ProcessInfo�Ĵ�С  
		BOOL Status = Process32First(SnapShot, &ProcessInfo);

		//�����������߽���Ȩ��ΪSE_DEBUG_NAME������XP���޷�ɱ������
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
			// ��ȡ�����ļ���Ϣ  
			SHGetFileInfo(ProcessInfo.szExeFile, 0, &shSmall, sizeof(shSmall), SHGFI_ICON | SHGFI_SMALLICON);

			// �������Ƿ���Ҫ�ر�
			strRunProcessName = ProcessInfo.szExeFile;
			strRunProcessName.MakeLower();
			for (vector<CString>::size_type it = 0; it != vecProcessName.size(); ++it)
			{
				strSearchName = vecProcessName[it];
				strSearchName.MakeLower();
				if (strSearchName == strRunProcessName)
				{
					// ��ȡ���̾����ǿ�йر�  
					ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessInfo.th32ProcessID);
					if (ProcessHandle != NULL)
					{
						bRet = TerminateProcess(ProcessHandle, 1);
						CloseHandle(ProcessHandle);
					}

					break;
				}

			}

			// ��ȡ��һ�����̵���Ϣ  
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
	int iPos = 0; //�ָ��λ��  
	int iNums = 0; //�ָ��������  
	CString strTemp = str;
	CString strRight;
	//�ȼ������ַ���������  
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
	if (iNums == 0) //û���ҵ��ָ��  
	{
		//���ַ����������ַ�������  
		iSubStrs = 1;
		return NULL;
	}
	//���ַ�������  
	iSubStrs = iNums + 1; //�Ӵ������� = �ָ������ + 1  
	CString* pStrSplit;
	pStrSplit = new CString[iSubStrs];
	strTemp = str;
	CString strLeft;
	for (int i = 0; i < iNums; i++)
	{
		iPos = strTemp.Find(split);
		//���Ӵ�  
		strLeft = strTemp.Left(iPos);
		//���Ӵ�  
		strRight = strTemp.Mid(iPos + 1, strTemp.GetLength());
		strTemp = strRight;
		pStrSplit[i] = strLeft;
	}
	pStrSplit[iNums] = strTemp;
	return pStrSplit;
}

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CstartBrowserDlg �Ի���



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


// CstartBrowserDlg ��Ϣ�������

BOOL CstartBrowserDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	GetDlgItem(IDC_PATH)->SetWindowText(_T("C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe"));
	GetDlgItem(IDC_URL)->SetWindowText(_T("http://v.qq.com/x/cover/4mz2zy4nxjjqqbp/k0020goctd2.html?ptag=bl.zy.bs_dfzc_41.161026&volume=0\r\nhttp://v.qq.com/x/cover/4mz2zy4nxjjqqbp/p002077glsm.html?ptag=bl.zy.bs_dfzc_42.161026&volume=0\r\nhttps://v.qq.com/x/cover/4mz2zy4nxjjqqbp/e00203bcxtq.html?ptag=bl.zy.bs_dfzc_44.161026&volume=0"));
	GetDlgItem(IDC_TIMES)->SetWindowText(_T("10000"));
	GetDlgItem(IDC_PERIOD)->SetWindowText(_T("90"));
	
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CstartBrowserDlg::OnPaint()
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
HCURSOR CstartBrowserDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void CstartBrowserDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
		//���ַ����е�\r\nת��Ϊ;
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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
		// ���m_nData1�Ѿ��ﵽ10��������IDΪ1�Ķ�ʱ��   		
		title.Format(_T("��%d�Σ�����%d�Σ�ִ����..."), timer_refresh+1,mTimes);
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
