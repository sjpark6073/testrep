#include "StdAfx.h"
#include "LogManager.h"
#include <locale.h>

CLogManager::CLogManager(void)
{
	m_bEnableFile = FALSE;
	m_bEnableConsole = FALSE;
	m_strPath = _T("log.txt");
	InitializeCriticalSectionAndSpinCount(&m_csLock,4000);

	_tsetlocale(LC_ALL, _T(""));
}


CLogManager::~CLogManager(void)
{
	DeleteCriticalSection(&m_csLock);
}

CLogManager* CLogManager::GetInstance()
{
	static CLogManager lf;
	return &lf;
}

void CLogManager::SetLogFilePath(CString strPath)
{
	m_strPath = strPath;
}

void CLogManager::Enable(BOOL bEnableFile, BOOL bEnableConsole)
{
	EnterCriticalSection(&m_csLock);

	m_bEnableFile = bEnableFile;
	m_bEnableConsole = bEnableConsole;

	if(m_bEnableConsole)
	{
		if(AllocConsole())
		{
			TCHAR szFileName[1024] = {0,};
			if(0 < GetModuleFileName(NULL, szFileName, _countof(szFileName)))
			{
				SetConsoleTitle(szFileName);
			}
		}
	}
	else
	{
		FreeConsole();
	}

	LeaveCriticalSection(&m_csLock);
}

void CLogManager::Log(LOG_TYPE type, LPCTSTR pszFormat, ...)
{
	va_list list;
	va_start(list, pszFormat);
	Log(type, pszFormat, list);
	va_end(list);
}

void CLogManager::Log_i(LPCTSTR pszFormat, ...)
{
	va_list list;
	va_start(list, pszFormat);
	Log(I, pszFormat, list);
	va_end(list);
}

void CLogManager::Log_e(LPCTSTR pszFormat, ...)
{
	va_list list;
	va_start(list, pszFormat);
	Log(E, pszFormat, list);
	va_end(list);
}

void CLogManager::Log_d(LPCTSTR pszFormat, ...)
{
	va_list list;
	va_start(list, pszFormat);
	Log(D, pszFormat, list);
	va_end(list);
}

void CLogManager::Log(LOG_TYPE type, LPCTSTR pszFormat, va_list list)
{
	CString strLogBuf;
	_vsntprintf_s(strLogBuf.GetBuffer(65536), 65536, _TRUNCATE, pszFormat, list);
	strLogBuf.ReleaseBuffer();

	CTime CurTime = CTime::GetCurrentTime();

	CString strLog;
	switch(type)
	{
	case I:
		strLog.Format(_T("[INF %s] %s"), CurTime.Format(_T("%Y-%m-%d %H:%M:%S")), strLogBuf);
		break;
	case E:
		strLog.Format(_T("[ERR %s] %s"), CurTime.Format(_T("%Y-%m-%d %H:%M:%S")), strLogBuf);
		break;
	case D:
		strLog.Format(_T("[DBG %s] %s"), CurTime.Format(_T("%Y-%m-%d %H:%M:%S")), strLogBuf);
		break;
	}

	EnterCriticalSection(&m_csLock);

	if(m_bEnableFile)
	{
		FILE* fp = NULL;
		if(0 == _tfopen_s(&fp, m_strPath, _T("ab")) && NULL != fp)
		{
			fprintf(fp, "%s\r\n", CommonUtil::StringToAnsi(strLog));
			fclose(fp);
		}
	}

	if(m_bEnableConsole)
	{
		FILE* fp = NULL;
		if(0 == _tfopen_s(&fp, _T("CONOUT$"), _T("w")) && NULL != fp)
		{
			_ftprintf(fp, _T("%s\n"), strLog);
			fclose(fp);
		}
	}

	LeaveCriticalSection(&m_csLock);
}