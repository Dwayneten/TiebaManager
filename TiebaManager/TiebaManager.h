
// TiebaManager.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTiebaManagerApp: 
// �йش����ʵ�֣������ TiebaManager.cpp
//

class CTiebaManagerApp : public CWinApp
{
public:
	CTiebaManagerApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTiebaManagerApp theApp;