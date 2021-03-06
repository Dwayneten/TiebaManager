/*
Copyright (C) 2015  xfgryujk
http://tieba.baidu.com/f?kw=%D2%BB%B8%F6%BC%AB%C6%E4%D2%FE%C3%D8%D6%BB%D3%D0xfgryujk%D6%AA%B5%C0%B5%C4%B5%D8%B7%BD

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "stdafx.h"
#include "TiebaManager.h"
#include "SettingDlg.h"
#include "afxdialogex.h"
#include "TiebaManagerDlg.h"
#include "Setting.h"
#include "Tieba.h"
#include <zlib.h>


// CSettingDlg 对话框

IMPLEMENT_DYNAMIC(CSettingDlg, CDialog)

// 构造函数
CSettingDlg::CSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingDlg::IDD, pParent)
{
	// 初始化m_pages
	int i = 0;
	m_pages[i++] = &m_prefPage;
	m_pages[i++] = &m_keywordsPage;
	m_pages[i++] = &m_blackListPage;
	m_pages[i++] = &m_whiteListPage;
	m_pages[i++] = &m_whiteContentPage;
	m_pages[i++] = &m_optionsPage;
	m_pages[i++] = &m_aboutPage;
}

#pragma region MFC
CSettingDlg::~CSettingDlg()
{
}

void CSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tab);
	DDX_Control(pDX, IDC_BUTTON1, m_logoutButton);
	DDX_Control(pDX, IDOK, m_okButton);
	DDX_Control(pDX, IDCANCEL, m_cancelButton);
}


BEGIN_MESSAGE_MAP(CSettingDlg, CDialog)
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CSettingDlg::OnTcnSelchangeTab1)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON1, &CSettingDlg::OnBnClickedButton1)
END_MESSAGE_MAP()
#pragma endregion

// CSettingDlg 消息处理程序

// 初始化
BOOL CSettingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(hIcon, TRUE);			// 设置大图标
	SetIcon(hIcon, FALSE);			// 设置小图标

	// 初始化m_tab
	int i = 0;
	m_tab.InsertItem(i++, _T("首选项"));
	m_tab.InsertItem(i++, _T("违规内容"));
	m_tab.InsertItem(i++, _T("屏蔽用户"));
	m_tab.InsertItem(i++, _T("信任用户"));
	m_tab.InsertItem(i++, _T("信任内容"));
	m_tab.InsertItem(i++, _T("方案"));
	m_tab.InsertItem(i++, _T("关于&&更新"));

	// 初始化各页
	m_prefPage.Create(IDD_PREF_PAGE, &m_tab);
	m_keywordsPage.Create(IDD_LIST_PAGE, &m_tab);
	m_blackListPage.Create(IDD_LIST_PAGE, &m_tab);
	m_whiteListPage.Create(IDD_LIST_PAGE, &m_tab);
	m_whiteContentPage.Create(IDD_LIST_PAGE, &m_tab);
	m_optionsPage.Create(IDD_OPTIONS_PAGE, &m_tab);
	m_aboutPage.Create(IDD_ABOUT_PAGE, &m_tab);

	CRect rect;
	m_tab.GetClientRect(&rect);
	rect.left += 1; rect.right -= 3; rect.top += 23; rect.bottom -= 2;
	m_prefPage.SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW);
	for (i = 1; i < _countof(m_pages); i++)
		m_pages[i]->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_HIDEWINDOW);

	// 显示配置
	ShowCurrentOptions();
	m_clearScanCache = FALSE; // 在m_prefPage.m_scanPageCountEdit.SetWindowText后初始化
	m_optionsPage.m_currentOptionStatic.SetWindowText(_T("当前方案：") + g_currentOption); // 当前方案
	m_aboutPage.m_autoCheckUpdateCheck.SetCheck(g_autoUpdate); // 自动更新
	// 方案
	CFileFind fileFind;
	BOOL flag = fileFind.FindFile(OPTIONS_PATH + _T("*.tb"));
	while (flag)
	{
		flag = fileFind.FindNextFile();
		m_optionsPage.m_list.AddString(fileFind.GetFileTitle());
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

#pragma region UI
// 窗口 /////////////////////////////////////////////////////////////////////////////////

// 取消
void CSettingDlg::OnCancel()
{
	DestroyWindow();
}

// 提示是否保存
void CSettingDlg::OnClose()
{
	int result = AfxMessageBox(_T("保存设置？"), MB_ICONQUESTION | MB_YESNOCANCEL);
	if (result == IDYES)
	{
		OnOK();
		return;
	}
	else if (result == IDCANCEL)
		return;

	CDialog::OnClose();
}

// 释放this
void CSettingDlg::PostNcDestroy()
{
	CDialog::PostNcDestroy();

	((CTiebaManagerDlg*)AfxGetApp()->m_pMainWnd)->m_settingDlg = NULL;
	delete this;
}

// 限制最小尺寸
void CSettingDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 489;
	lpMMI->ptMinTrackSize.y = 411;

	CDialog::OnGetMinMaxInfo(lpMMI);
}

// 改变尺寸
void CSettingDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (m_tab.m_hWnd == NULL)
		return;

	CRect rect;
	GetClientRect(&rect); // 默认473 * 373
	m_tab.SetWindowPos(NULL, 0, 0, rect.Width() - 21, rect.Height() - 58, SWP_NOMOVE | SWP_NOREDRAW);
	int y = rect.Height() - 35;
	m_logoutButton.SetWindowPos(NULL, rect.Width() - 326, y, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);
	m_okButton.SetWindowPos(NULL, rect.Width() - 200, y, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);
	m_cancelButton.SetWindowPos(NULL, rect.Width() - 105, y, 0, 0, SWP_NOSIZE | SWP_NOREDRAW);

	m_tab.GetClientRect(&rect);
	rect.left += 1; rect.right -= 3; rect.top += 23; rect.bottom -= 2;
	for (int i = 0; i < _countof(m_pages); i++)
		m_pages[i]->SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height(), SWP_NOMOVE | SWP_NOREDRAW);

	Invalidate();
}

// 切换标签
void CSettingDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	int index = m_tab.GetCurSel();
	for (int i = 0; i < _countof(m_pages); i++)
		m_pages[i]->ShowWindow(i == index ? SW_SHOW : SW_HIDE);
}
#pragma endregion

// 显示当前设置
void CSettingDlg::ShowCurrentOptions()
{
	CString tmp;
	tmp.Format(_T("%d"), g_scanInterval);
	m_prefPage.m_scanIntervalEdit.SetWindowText(tmp);		// 扫描间隔
	m_prefPage.m_banIDCheck.SetCheck(g_banID);				// 封ID
	m_prefPage.m_banDurationCombo.SetCurSel(g_banDuration == 1 ? 0 : (g_banDuration == 3 ? 1 : 2)); // 封禁时长
	m_prefPage.m_banIPCheck.SetCheck(g_banIP);				// 封IP
	m_prefPage.OnBnClickedCheck1();
	tmp.Format(_T("%d"), g_trigCount);
	m_prefPage.m_trigCountEdit.SetWindowText(tmp);			// 封禁违规次数
	m_prefPage.m_onlyScanTitleCheck.SetCheck(g_onlyScanTitle); // 只扫描标题
	tmp.Format(_T("%g"), g_deleteInterval);
	m_prefPage.m_deleteIntervalEdit.SetWindowText(tmp);		// 删帖间隔
	m_prefPage.m_confirmCheck.SetCheck(g_confirm);			// 操作前提示
	tmp.Format(_T("%d"), g_scanPageCount);
	m_prefPage.m_scanPageCountEdit.SetWindowText(tmp);		// 扫描最后页数
	m_prefPage.m_briefLogCheck.SetCheck(g_briefLog);		// 只输出删帖封号

	// 违规内容
	m_keywordsPage.m_list.ResetContent();
	for (const RegexText& i : g_keywords)
		m_keywordsPage.m_list.AddString((i.isRegex ? IS_REGEX_PREFIX : NOT_REGEX_PREFIX) + i.text);

	// 屏蔽用户
	m_blackListPage.m_list.ResetContent();
	for (const RegexText& i : g_blackList)
		m_blackListPage.m_list.AddString((i.isRegex ? IS_REGEX_PREFIX : NOT_REGEX_PREFIX) + i.text);

	// 信任用户
	m_whiteListPage.m_list.ResetContent();
	for (const CString& i : g_whiteList)
		m_whiteListPage.m_list.AddString(i);

	// 信任内容
	m_whiteContentPage.m_list.ResetContent();
	for (const RegexText& i : g_whiteContent)
		m_whiteContentPage.m_list.AddString((i.isRegex ? IS_REGEX_PREFIX : NOT_REGEX_PREFIX) + i.text);
}

static inline void ApplyRegexTexts(vector<RegexText>& vec, CListBox& list)
{
	int size = list.GetCount();
	vec.resize(size);
	CString tmp;
	for (int i = 0; i < size; i++)
	{
		list.GetText(i, tmp);
		vec[i].isRegex = tmp.Left(REGEX_PREFIX_LENGTH) == IS_REGEX_PREFIX;
		vec[i].text = tmp.Right(tmp.GetLength() - REGEX_PREFIX_LENGTH);
		vec[i].regexp = vec[i].isRegex ? vec[i].text : _T("");
	}
}

// 应用对话框中的设置
void CSettingDlg::ApplyOptionsInDlg()
{
	CString strBuf;
	int intBuf;
	m_prefPage.m_scanIntervalEdit.GetWindowText(strBuf);
	g_scanInterval = _ttoi(strBuf);								// 扫描间隔
	g_banID = m_prefPage.m_banIDCheck.GetCheck();				// 封ID
	intBuf = m_prefPage.m_banDurationCombo.GetCurSel();
	g_banDuration = intBuf == 0 ? 1 : (intBuf == 1 ? 3 : 10);	// 封禁时长
	g_banIP = m_prefPage.m_banIPCheck.GetCheck();				// 封IP
	m_prefPage.m_trigCountEdit.GetWindowText(strBuf);
	g_trigCount = _ttoi(strBuf);								// 封禁违规次数
	g_onlyScanTitle = m_prefPage.m_onlyScanTitleCheck.GetCheck(); // 只扫描标题
	m_prefPage.m_deleteIntervalEdit.GetWindowText(strBuf);
	g_deleteInterval = (float)_ttof(strBuf);					// 删帖间隔
	g_confirm = m_prefPage.m_confirmCheck.GetCheck();			// 操作前提示
	m_prefPage.m_scanPageCountEdit.GetWindowText(strBuf);
	g_scanPageCount = _ttoi(strBuf);							// 扫描最后页数
	g_briefLog = m_prefPage.m_briefLogCheck.GetCheck();			// 只输出删帖封号

	g_optionsLock.Lock();
	// 违规内容
	ApplyRegexTexts(g_keywords, m_keywordsPage.m_list);

	// 屏蔽用户
	ApplyRegexTexts(g_blackList, m_blackListPage.m_list);

	// 信任用户
	int size = m_whiteListPage.m_list.GetCount();
	g_whiteList.resize(size);
	for (int i = 0; i < size; i++)
	{
		m_whiteListPage.m_list.GetText(i, g_whiteList[i]);
	}

	// 信任内容
	ApplyRegexTexts(g_whiteContent, m_whiteContentPage.m_list);
	g_optionsLock.Unlock();

	if (m_clearScanCache)
	{
		if (!g_briefLog)
			((CTiebaManagerDlg*)AfxGetApp()->m_pMainWnd)->Log(_T("<font color=green>清除扫描记录</font>"));
		g_reply.clear();
	}
}

static inline void ReadRegexTexts(const gzFile& f, CListBox& list)
{
	int size;
	gzread(f, &size, sizeof(int)); // 长度
	list.ResetContent();
	CString strBuf;
	for (int i = 0; i < size; i++)
	{
		int isRegex;
		gzread(f, &isRegex, sizeof(int)); // 是正则
		int strLen;
		gzread(f, &strLen, sizeof(int)); // 字符串长度
		gzread(f, strBuf.GetBuffer(strLen), strLen * sizeof(TCHAR)); // 字符串
		strBuf.ReleaseBuffer();
		list.AddString((isRegex != 0 ? IS_REGEX_PREFIX : NOT_REGEX_PREFIX) + strBuf);
	}
}

// 显示文件中的设置
void CSettingDlg::ShowOptionsInFile(LPCTSTR path)
{
	CString strBuf;

	gzFile f = gzopen_w(path, "rb");
	if (f == NULL)
		goto UseDefaultOptions;

	// 头部
	char header[2];
	gzread(f, header, sizeof(header));
	if (header[0] != 'T' || header[1] != 'B')
	{
		gzclose(f);
		goto UseDefaultOptions;
	}

	// 违规内容
	ReadRegexTexts(f, m_keywordsPage.m_list);

	// 屏蔽用户
	ReadRegexTexts(f, m_blackListPage.m_list);

	// 信任用户
	int size;
	gzread(f, &size, sizeof(int)); // 长度
	m_whiteListPage.m_list.ResetContent();
	int intBuf;
	for (int i = 0; i < size; i++)
	{
		gzread(f, &intBuf, sizeof(int)); // 字符串长度
		gzread(f, strBuf.GetBuffer(intBuf), intBuf * sizeof(TCHAR)); // 字符串
		strBuf.ReleaseBuffer();
		m_whiteListPage.m_list.AddString(strBuf);
	}

	// 信任内容
	ReadRegexTexts(f, m_whiteContentPage.m_list);

	BOOL boolBuf;
	float floatBuf;
	gzread(f, &intBuf, sizeof(int));
	strBuf.Format(_T("%d"), intBuf);
	m_prefPage.m_scanIntervalEdit.SetWindowText(strBuf);	// 扫描间隔
	gzread(f, &boolBuf, sizeof(BOOL));
	m_prefPage.m_banIDCheck.SetCheck(boolBuf);				// 封ID
	gzread(f, &intBuf, sizeof(int));
	m_prefPage.m_banDurationCombo.SetCurSel(intBuf == 1 ? 0 : (intBuf == 3 ? 1 : 2)); // 封禁时长
	gzread(f, &boolBuf, sizeof(BOOL));
	m_prefPage.m_banIPCheck.SetCheck(boolBuf);				// 封IP
	gzread(f, &intBuf, sizeof(int));
	strBuf.Format(_T("%d"), intBuf);
	m_prefPage.m_trigCountEdit.SetWindowText(strBuf);		// 封禁违规次数
	gzread(f, &boolBuf, sizeof(BOOL));
	m_prefPage.m_onlyScanTitleCheck.SetCheck(boolBuf);		// 只扫描标题
	gzread(f, &floatBuf, sizeof(float));
	strBuf.Format(_T("%g"), floatBuf);
	m_prefPage.m_deleteIntervalEdit.SetWindowText(strBuf);	// 删帖间隔
	gzread(f, &boolBuf, sizeof(BOOL));
	m_prefPage.m_confirmCheck.SetCheck(boolBuf);			// 操作前提示
	gzread(f, &intBuf, sizeof(int));
	strBuf.Format(_T("%d"), intBuf);
	m_prefPage.m_scanPageCountEdit.SetWindowText(strBuf);	// 扫描最后页数
	gzread(f, &boolBuf, sizeof(BOOL));
	m_prefPage.m_briefLogCheck.SetCheck(boolBuf);			// 只输出删帖封号

	gzclose(f);
	return;

UseDefaultOptions:
	m_keywordsPage.m_list.ResetContent();					// 违规内容
	m_blackListPage.m_list.ResetContent();					// 屏蔽用户
	m_whiteListPage.m_list.ResetContent();					// 信任用户
	m_whiteContentPage.m_list.ResetContent();				// 信任内容
	m_prefPage.m_scanIntervalEdit.SetWindowText(_T("5"));	// 扫描间隔
	m_prefPage.m_banIDCheck.SetCheck(FALSE);				// 封ID
	m_prefPage.m_banDurationCombo.SetCurSel(0);				// 封禁时长
	m_prefPage.m_banIPCheck.SetCheck(FALSE);				// 封IP
	m_prefPage.m_trigCountEdit.SetWindowText(_T("1"));		// 封禁违规次数
	m_prefPage.m_onlyScanTitleCheck.SetCheck(FALSE);		// 只扫描标题
	m_prefPage.m_deleteIntervalEdit.SetWindowText(_T("2"));	// 删帖间隔
	m_prefPage.m_confirmCheck.SetCheck(TRUE);				// 操作前提示
	m_prefPage.m_scanPageCountEdit.SetWindowText(_T("1"));	// 扫描最后页数
	m_prefPage.m_briefLogCheck.SetCheck(FALSE);				// 只输出删帖封号
}

static inline void WriteRegexTexts(const gzFile& f, CListBox& list)
{
	int size;
	gzwrite(f, &(size = list.GetCount()), sizeof(int)); // 长度
	CString strBuf;
	for (int i = 0; i < size; i++)
	{
		list.GetText(i, strBuf);
		BOOL isRegex = strBuf.Left(REGEX_PREFIX_LENGTH) == IS_REGEX_PREFIX;
		int intBuf;
		gzwrite(f, &(intBuf = isRegex ? 1 : 0), sizeof(int)); // 是正则
		strBuf = strBuf.Right(strBuf.GetLength() - REGEX_PREFIX_LENGTH);
		int len = strBuf.GetLength();
		gzwrite(f, &len, sizeof(int)); // 字符串长度
		gzwrite(f, (LPCTSTR)strBuf, len * sizeof(TCHAR)); // 字符串
	}
}

// 把对话框中的设置写入文件
void CSettingDlg::SaveOptionsInDlg(LPCTSTR path)
{
	gzFile f = gzopen_w(path, "wb");
	if (f == NULL)
		return;

	// 头部
	gzwrite(f, "TB", 2);

	// 违规内容
	WriteRegexTexts(f, m_keywordsPage.m_list);

	// 屏蔽用户
	WriteRegexTexts(f, m_blackListPage.m_list);

	// 信任用户
	int size;
	gzwrite(f, &(size = m_whiteListPage.m_list.GetCount()), sizeof(int)); // 长度
	CString strBuf;
	for (int i = 0; i < size; i++)
	{
		m_whiteListPage.m_list.GetText(i, strBuf);
		int len = strBuf.GetLength();
		gzwrite(f, &len, sizeof(int)); // 字符串长度
		gzwrite(f, (LPCTSTR)strBuf, len * sizeof(TCHAR)); // 字符串
	}

	// 信任内容
	WriteRegexTexts(f, m_whiteContentPage.m_list);

	int intBuf;
	BOOL boolBuf;
	float floatBuf;
	m_prefPage.m_scanIntervalEdit.GetWindowText(strBuf);
	gzwrite(f, &(intBuf = _ttoi(strBuf)), sizeof(int));								// 扫描间隔
	gzwrite(f, &(boolBuf = m_prefPage.m_banIDCheck.GetCheck()), sizeof(BOOL));		// 封ID
	intBuf = m_prefPage.m_banDurationCombo.GetCurSel();
	intBuf = intBuf == 0 ? 1 : (intBuf == 1 ? 3 : 10);
	gzwrite(f, &intBuf, sizeof(int));												// 封禁时长
	gzwrite(f, &(boolBuf = m_prefPage.m_banIPCheck.GetCheck()), sizeof(BOOL));		// 封IP
	m_prefPage.m_trigCountEdit.GetWindowText(strBuf);
	gzwrite(f, &(intBuf = _ttoi(strBuf)), sizeof(int));								// 封禁违规次数
	gzwrite(f, &(boolBuf = m_prefPage.m_onlyScanTitleCheck.GetCheck()), sizeof(BOOL)); // 只扫描标题
	m_prefPage.m_deleteIntervalEdit.GetWindowText(strBuf);
	gzwrite(f, &(floatBuf = (float)_ttof(strBuf)), sizeof(float));					// 删帖间隔
	gzwrite(f, &(boolBuf = m_prefPage.m_confirmCheck.GetCheck()), sizeof(BOOL));	// 操作前提示
	m_prefPage.m_scanPageCountEdit.GetWindowText(strBuf);
	gzwrite(f, &(intBuf = _ttoi(strBuf)), sizeof(int));								// 扫描最后页数
	gzwrite(f, &(boolBuf = m_prefPage.m_briefLogCheck.GetCheck()), sizeof(BOOL));	// 只输出删帖封号

	gzclose(f);
}

// 确认
void CSettingDlg::OnOK()
{
	CString tmp;
	m_optionsPage.m_currentOptionStatic.GetWindowText(tmp);
	g_currentOption = tmp.Right(tmp.GetLength() - 5);
	if (!PathFileExists(OPTIONS_PATH))
		CreateDirectory(OPTIONS_PATH, NULL);
	SaveOptionsInDlg(OPTIONS_PATH + g_currentOption + _T(".tb"));
	ApplyOptionsInDlg();
	WritePrivateProfileString(_T("Routine"), _T("Option"), g_currentOption, PROFILE_PATH);
	WritePrivateProfileString(_T("Routine"), _T("AutoUpdate"), 
		m_aboutPage.m_autoCheckUpdateCheck.GetCheck() ? _T("1") : _T("0"), PROFILE_PATH);

	DestroyWindow();
}

// 退出登录
void CSettingDlg::OnBnClickedButton1()
{
	g_cookie = _T("");
	AfxGetApp()->m_pMainWnd->DestroyWindow();
}
