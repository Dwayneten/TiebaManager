// KeywordsPage.cpp : 实现文件
//

#include "stdafx.h"
#include "TiebaManager.h"
#include "KeywordsPage.h"
#include "afxdialogex.h"
#include "Global.h"
#include "SettingDlg.h"


// CKeywordsPage 对话框

IMPLEMENT_DYNAMIC(CKeywordsPage, CListPage)

CKeywordsPage::CKeywordsPage(CWnd* pParent /*=NULL*/)
	: CListPage(pParent)
{

}

CKeywordsPage::~CKeywordsPage()
{
}

void CKeywordsPage::DoDataExchange(CDataExchange* pDX)
{
	CListPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CKeywordsPage, CListPage)
END_MESSAGE_MAP()


// CKeywordsPage 消息处理程序


BOOL CKeywordsPage::OnInitDialog()
{
	CListPage::OnInitDialog();

	m_static.SetWindowText(_T("支持图片地址\\视频地址，当被测文本含有文本时匹配\r\n\
注意正则表达式 .* 匹配所有文本，请不要不加限制使用！！"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

BOOL CKeywordsPage::CheckMatchTooMuch(const CString& text, BOOL isRegex)
{
	return StringIncludes(MATCH_TOO_MUCH_CONTENT_TEST1, text, isRegex) 
		&& StringIncludes(MATCH_TOO_MUCH_CONTENT_TEST2, text, isRegex);
}

void CKeywordsPage::PostChangeList()
{
	((CSettingDlg*)GetParent()->GetParent())->m_clearScanCache = TRUE;
}

BOOL CKeywordsPage::TestMatch(const CString& test, const CString& text, BOOL isRegex)
{
	return StringIncludes(test, text, isRegex);
}
