#pragma once
#include "afxwin.h"


// COptionsPage �Ի���

class COptionsPage : public CDialog
{
	DECLARE_DYNAMIC(COptionsPage)

public:
	COptionsPage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~COptionsPage();

// �Ի�������
	enum { IDD = IDD_OPTIONS_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	virtual void OnCancel();
	virtual void OnOK();
public:
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLbnDblclkList1();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton4();


public:
	CListBox m_list;
	CEdit m_edit;
	CButton m_newOptionsButton;
	CButton m_deleteOptionsButton;
	CButton m_renameOptionsButton;
	CButton m_loadOptionsButton;
	CButton m_saveOptionsButton;
	CStatic m_currentOptionStatic;
};
