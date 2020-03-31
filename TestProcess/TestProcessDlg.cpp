
// TestProcessDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "TestProcess.h"
#include "TestProcessDlg.h"
#include "afxdialogex.h"
#include "Functions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTestProcessDlg 대화 상자




CTestProcessDlg::CTestProcessDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTestProcessDlg::IDD, pParent)
	, m_strInput(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDT_INPUT, m_strInput);
	DDX_Control(pDX, IDC_LST_OUTPUT, m_lstOutput);
}

void CTestProcessDlg::SetSelectedColumnAll(CListCtrl* pListCtrl)
{
	ASSERT(pListCtrl && ::IsWindow(pListCtrl->GetSafeHwnd()));

	for(int i=0 ; i<pListCtrl->GetItemCount() ; i++)
	{
		pListCtrl->SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
		pListCtrl->SetSelectionMark(i);
	}
}

BOOL CTestProcessDlg::CopyListToClipboard(CListCtrl* pListCtrl, LPCTSTR lpszSeparator /*= _T("\t")*/)
{
	ASSERT(pListCtrl && ::IsWindow(pListCtrl->GetSafeHwnd()));

	CString sResult;
	POSITION pos = pListCtrl->GetFirstSelectedItemPosition();
	if (!pos)
		return TRUE;

	CWaitCursor wait;
	int nItem, nCount = 0;
	int nColumn = 1;

	//컬럼 이름 관련 구조체
	TCHAR szName[128]; // This is the buffer where the name will be stored
	LVCOLUMN lvColInfo;
	lvColInfo.mask = LVCF_TEXT;
	lvColInfo.pszText = szName;
	lvColInfo.cchTextMax = _countof(szName);

	//헤더와 컬럼 갯수 구하기
	if ((pListCtrl->GetStyle() & LVS_TYPEMASK) == LVS_REPORT &&
		pListCtrl->GetExtendedStyle() & LVS_EX_FULLROWSELECT)
	{
		CHeaderCtrl* pHeader = pListCtrl->GetHeaderCtrl();
		nColumn = pHeader ? pHeader->GetItemCount() : 1;
	}

	//컬럼 이름 삽입
	for (int i = 0; i < nColumn; ++i)
	{
		pListCtrl->GetColumn(i, &lvColInfo);

		sResult += lvColInfo.pszText;
		if (i != nColumn - 1)
			sResult += lpszSeparator;
	}
	sResult += _T("\r\n");

	//데이터 내용 삽입
	while (pos)
	{
		nItem = pListCtrl->GetNextSelectedItem(pos);
		if (0 != nCount)
			sResult += _T("\r\n");

		for (int i = 0; i < nColumn; ++i)
		{
			sResult += pListCtrl->GetItemText(nItem, i);
			if (i != nColumn - 1)
				sResult += lpszSeparator;
		}
		++nCount;
	}
	sResult += _T("\r\n");

	//클립보드 저장
	if (pListCtrl->OpenClipboard())
	{
		EmptyClipboard();

		int nLen = (sResult.GetLength()+1) * sizeof(WCHAR);
		HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, nLen);
		LPBYTE pGlobalData = (LPBYTE)GlobalLock(hGlobal);
		ZeroMemory(pGlobalData, nLen);
		CopyMemory(pGlobalData, CommonUtil::StringToUnicode(sResult).GetBuffer(0), nLen);
		SetClipboardData(CF_UNICODETEXT, hGlobal);

		GlobalUnlock(hGlobal);
		GlobalFree(hGlobal);

		CloseClipboard();
		return TRUE;
	}
	return FALSE;
}

BEGIN_MESSAGE_MAP(CTestProcessDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_QUERY, &CTestProcessDlg::OnBnClickedBtnQuery)
END_MESSAGE_MAP()


// CTestProcessDlg 메시지 처리기

BOOL CTestProcessDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	LOG_PATH(CommonUtil::GetLocalPath()+_T("\\processlog.txt"));
	LOG_ENABLE(TRUE, TRUE);

	CRect rect;
	m_lstOutput.GetClientRect(&rect);

	int nWidth = 150, nTotalWidth = 0;
	m_lstOutput.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_lstOutput.InsertColumn(0, _T("이름"), 0, 150); nTotalWidth += 150;
	m_lstOutput.InsertColumn(1, _T("PID"), 0, 70); nTotalWidth += 70;
	m_lstOutput.InsertColumn(2, _T("사용자 이름"), 0, 150); nTotalWidth += 150;

	if(rect.Width()-20 > nTotalWidth) nWidth = (rect.Width()-nTotalWidth-20)/2;
	m_lstOutput.InsertColumn(3, _T("이미지 경로 이름"), 0, nWidth); nTotalWidth += nWidth;
	m_lstOutput.InsertColumn(4, _T("명령줄"), 0, nWidth); nTotalWidth += nWidth;

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CTestProcessDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CTestProcessDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CTestProcessDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



BOOL CTestProcessDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->hwnd == m_lstOutput.GetSafeHwnd() && GetFocus()->GetSafeHwnd() == m_lstOutput.GetSafeHwnd())
	{
		if(GetKeyState(VK_CONTROL) < 0 && pMsg->message == WM_KEYDOWN)
		{
			switch(pMsg->wParam)
			{
			case 'A':
				SetSelectedColumnAll(&m_lstOutput);
				break;
			case 'C':
				CopyListToClipboard(&m_lstOutput, _T(" | "));
				break;
			default:
				break;
			}
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CTestProcessDlg::OnBnClickedBtnQuery()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWaitCursor wc;

	UpdateData();
	m_lstOutput.DeleteAllItems();

	vector<WMI_Win32_Process> vecProcesses;
	GetWin32Process(m_strInput, vecProcesses);

	for(size_t i=0 ; i<vecProcesses.size() ; i++)
	{
		CString strProcessId;
		strProcessId.Format(_T("%lu"), vecProcesses[i].dwProcessId);
		m_lstOutput.InsertItem(i, vecProcesses[i].strName);
		m_lstOutput.SetItemText(i, 1, strProcessId);
		m_lstOutput.SetItemText(i, 2, vecProcesses[i].strUser);
		m_lstOutput.SetItemText(i, 3, vecProcesses[i].strExecutablePath);
		m_lstOutput.SetItemText(i, 4, vecProcesses[i].strCommandLine);

		CString strCommandLine;
		vector<CString> vecArgs = CommonUtil::CommandLineParse(vecProcesses[i].strCommandLine);
		for(size_t i2=0 ; i2<vecArgs.size() ; i2++)
		{
			strCommandLine += _T("\r\n")+vecArgs[i2];
		}
		LOG_D(_T("%s | %lu | %s | %s | %s | CommandLine :%s"),
			vecProcesses[i].strName,
			vecProcesses[i].dwProcessId,
			vecProcesses[i].strUser,
			vecProcesses[i].strDomain,
			vecProcesses[i].strExecutablePath,
			strCommandLine);
	}

	UpdateData(FALSE);
}
