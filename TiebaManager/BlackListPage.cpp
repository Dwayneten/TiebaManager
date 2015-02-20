// BlackListPage.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TiebaManager.h"
#include "BlackListPage.h"
#include "afxdialogex.h"
#include "Global.h"
#include "SettingDlg.h"


// CBlackListPage �Ի���

IMPLEMENT_DYNAMIC(CBlackListPage, CListPage)

CBlackListPage::CBlackListPage(CWnd* pParent /*=NULL*/)
	: CListPage(pParent)
{

}

CBlackListPage::~CBlackListPage()
{
}

void CBlackListPage::DoDataExchange(CDataExchange* pDX)
{
	CListPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBlackListPage, CListPage)
END_MESSAGE_MAP()


// CBlackListPage ��Ϣ�������


BOOL CBlackListPage::OnInitDialog()
{
	CListPage::OnInitDialog();

	m_static.SetWindowText(_T("�������ı������ı�ʱƥ��\r\n\
ע��������ʽ.*ƥ�������ı����벻Ҫ��������ʹ�ã���"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

BOOL CBlackListPage::CheckMatchTooMuch(const CString& text, BOOL isRegex)
{
	return StringMatchs(MATCH_TOO_MUCH_USERNAME_TEST1, text, isRegex) 
		&& StringMatchs(MATCH_TOO_MUCH_USERNAME_TEST2, text, isRegex);
}

void CBlackListPage::PostChangeList()
{
	((CSettingDlg*)GetParent()->GetParent())->m_clearScanCache = TRUE;
}

BOOL CBlackListPage::TestMatch(const CString& test, const CString& text, BOOL isRegex)
{
	return StringMatchs(test, text, isRegex);
}
