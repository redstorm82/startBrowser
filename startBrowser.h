
// startBrowser.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CstartBrowserApp: 
// �йش����ʵ�֣������ startBrowser.cpp
//

class CstartBrowserApp : public CWinApp
{
public:
	CstartBrowserApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CstartBrowserApp theApp;