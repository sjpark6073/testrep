#pragma once

#include <vector>
using namespace std;

typedef struct _WMI_Win32_Process_
{
	_WMI_Win32_Process_()
	{
		dwProcessId = 0;
	};

	CString strName;
	CString strCommandLine;
	CString strExecutablePath;
	DWORD dwProcessId;
	CString strUser;
	CString strDomain;

	DWORD   cntThreads;
	DWORD   th32ParentProcessID;    // this process's parent process
	LONG    pcPriClassBase;         // Base priority of process's threads

}WMI_Win32_Process, *PWMI_Win32_Process;

BOOL GetWin32Process(CString strProcessName, vector<WMI_Win32_Process>& vecReturn);