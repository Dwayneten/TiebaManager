#pragma once
#include "ListPage.h"


// CWhiteContentPage �Ի���

class CWhiteContentPage : public CListPage
{
	DECLARE_DYNAMIC(CWhiteContentPage)

public:
	CWhiteContentPage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CWhiteContentPage();

// �Ի�������
	enum { IDD = IDD_LIST_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

protected:
	virtual BOOL CheckMatchTooMuch(const CString& text, BOOL isRegex);
	virtual BOOL TestMatch(const CString& test, const CString& text, BOOL isRegex);
};