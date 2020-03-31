#include "stdafx.h"
#include "Functions.h"

#include <TlHelp32.h>
#include <Sddl.h>
#include <winternl.h>

typedef struct _UNICODE_STRING64 {
	USHORT Length;
	USHORT MaximumLength;
	PVOID64 Buffer;
} UNICODE_STRING64;

typedef struct _RTL_USER_PROCESS_PARAMETERS64 {
	BYTE Reserved1[16];
	PVOID64 Reserved2[5];
	UNICODE_STRING64 CurrentDirectoryPath;
	PVOID64 CurrentDirectoryHandle;
	UNICODE_STRING64 DllPath;
	UNICODE_STRING64 ImagePathName;
	UNICODE_STRING64 CommandLine;
	PVOID64 env;
} RTL_USER_PROCESS_PARAMETERS64, *PRTL_USER_PROCESS_PARAMETERS64;

typedef struct _PROCESS_BASIC_INFORMATION64 {
	PVOID Reserved1[2];
	PVOID64 PebBaseAddress;
	PVOID Reserved2[4];
	ULONG_PTR UniqueProcessId[2];
	PVOID Reserved3[2];
} PROCESS_BASIC_INFORMATION64;

typedef struct _PEB64 {
	BYTE Reserved1[2];
	BYTE BeingDebugged;
	BYTE Reserved2[21];
	PVOID64 LoaderData;
	PVOID64 ProcessParameters;
} PEB64;


typedef NTSTATUS (NTAPI *_NtQueryInformationProcess)(
	IN HANDLE ProcessHandle,
	ULONG ProcessInformationClass,
	OUT PVOID ProcessInformation,
	IN ULONG ProcessInformationLength,
	OUT PULONG ReturnLength OPTIONAL
	);

typedef NTSTATUS (NTAPI *_NtWow64ReadVirtualMemory64)(
	IN HANDLE ProcessHandle,
	IN PVOID64 BaseAddress,
	OUT PVOID Buffer,
	IN ULONG64 Size,
	OUT PULONG64 NumberOfBytesRead
	);

void QueryProcessInfo(HANDLE hProcess, WMI_Win32_Process& Process, _NtQueryInformationProcess NtQueryInformationProcess)
{
	PROCESS_BASIC_INFORMATION pbi;
	ZeroMemory(&pbi, sizeof(PROCESS_BASIC_INFORMATION));
	NTSTATUS ret = NtQueryInformationProcess(hProcess, ProcessBasicInformation, &pbi, sizeof(PROCESS_BASIC_INFORMATION), NULL);
	if(0 != ret)
	{
		return;
	}

	PEB peb;
	ZeroMemory(&peb, sizeof(PEB));
	if(!ReadProcessMemory(hProcess, pbi.PebBaseAddress, &peb, sizeof(PEB), NULL))
	{
		return;
	}

	RTL_USER_PROCESS_PARAMETERS pp;
	ZeroMemory(&pp, sizeof(RTL_USER_PROCESS_PARAMETERS));
	if(!ReadProcessMemory(hProcess, peb.ProcessParameters, &pp, sizeof(RTL_USER_PROCESS_PARAMETERS), NULL))
	{
		return;
	}

	PWSTR pBuffer = (PWSTR)malloc(pp.CommandLine.MaximumLength);
	if(NULL != pBuffer)
	{
		if(ReadProcessMemory(hProcess, pp.CommandLine.Buffer, pBuffer, pp.CommandLine.MaximumLength, NULL))
		{
			Process.strCommandLine = pBuffer;
		}
		free(pBuffer);
	}

	pBuffer = (PWSTR)malloc(pp.ImagePathName.MaximumLength);
	if(NULL != pBuffer)
	{
		if(ReadProcessMemory(hProcess, pp.ImagePathName.Buffer, pBuffer, pp.ImagePathName.MaximumLength, NULL))
		{
			Process.strExecutablePath = pBuffer;
		}
		free(pBuffer);
	}
}

void QueryProcessInfo64(HANDLE hProcess, WMI_Win32_Process& Process, _NtQueryInformationProcess NtQueryInformationProcess, _NtWow64ReadVirtualMemory64 NtWow64ReadVirtualMemory64)
{
	PROCESS_BASIC_INFORMATION64 pbi;
	ZeroMemory(&pbi, sizeof(PROCESS_BASIC_INFORMATION64));
	NTSTATUS ret = NtQueryInformationProcess(hProcess, ProcessBasicInformation, &pbi, sizeof(PROCESS_BASIC_INFORMATION64), NULL);
	if(0 != ret)
	{
		return;
	}

	PEB64 peb;
	ZeroMemory(&peb, sizeof(PEB64));
	ret = NtWow64ReadVirtualMemory64(hProcess, pbi.PebBaseAddress, &peb, sizeof(PEB64), NULL);
	if(0 != ret)
	{
		return;
	}

	RTL_USER_PROCESS_PARAMETERS64 pp;
	ZeroMemory(&pp, sizeof(RTL_USER_PROCESS_PARAMETERS64));
	ret = NtWow64ReadVirtualMemory64(hProcess, peb.ProcessParameters, &pp, sizeof(RTL_USER_PROCESS_PARAMETERS64), NULL);
	if(0 != ret)
	{
		return;
	}

	PWSTR pBuffer = (PWSTR)malloc(pp.CommandLine.MaximumLength);
	if(NULL != pBuffer)
	{
		ret = NtWow64ReadVirtualMemory64(hProcess, pp.CommandLine.Buffer, pBuffer, pp.CommandLine.MaximumLength, NULL);
		if(0 == ret)
		{
			Process.strCommandLine = pBuffer;
		}
		else
		{
		}
		free(pBuffer);
	}

	pBuffer = (PWSTR)malloc(pp.ImagePathName.MaximumLength);
	if(NULL != pBuffer)
	{
		ret = NtWow64ReadVirtualMemory64(hProcess, pp.ImagePathName.Buffer, pBuffer, pp.ImagePathName.MaximumLength, NULL);
		if(0 == ret)
		{
			Process.strExecutablePath = pBuffer;
		}
		else
		{
		}
		free(pBuffer);
	}
}

BOOL GetWin32Process(CString strProcessName, vector<WMI_Win32_Process>& vecReturn)
{
	HANDLE hToken = NULL;
	if(!::OpenThreadToken(GetCurrentThread(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, FALSE, &hToken))
	{
		if(ERROR_NO_TOKEN == ::GetLastError())
		{
			if(!ImpersonateSelf(SecurityImpersonation))
			{
				return FALSE;
			}

			if(!::OpenThreadToken(GetCurrentThread(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, FALSE, &hToken))
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}

	TOKEN_PRIVILEGES tp;
	LUID luid;
	TOKEN_PRIVILEGES tpPrevious;
	DWORD cbPrevious=sizeof(TOKEN_PRIVILEGES);

	if(!::LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid ))
	{
		::CloseHandle(hToken);
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = 0;

	::AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), &tpPrevious, &cbPrevious);
	if(ERROR_SUCCESS != ::GetLastError())
	{
		::CloseHandle(hToken);
		return FALSE;
	}

	tpPrevious.PrivilegeCount = 1;
	tpPrevious.Privileges[0].Luid = luid;
	tpPrevious.Privileges[0].Attributes |= (SE_PRIVILEGE_ENABLED);

	::AdjustTokenPrivileges(hToken, FALSE, &tpPrevious, cbPrevious, NULL, NULL);
	if(ERROR_SUCCESS != ::GetLastError())
	{
		::CloseHandle(hToken);
		return FALSE;
	}

	::CloseHandle(hToken);

	BOOL bWow64Env = FALSE;
	if(!::IsWow64Process(GetCurrentProcess(), &bWow64Env))
	{
		return FALSE;
	}

	HMODULE hModule = GetModuleHandleA("ntdll.dll");
	if(NULL == hModule)
	{
		return FALSE;
	}

	_NtQueryInformationProcess NtQueryInformationProcess = NULL;
	_NtQueryInformationProcess NtWow64QueryInformationProcess64 = NULL;
	_NtWow64ReadVirtualMemory64 NtWow64ReadVirtualMemory64 = NULL;

	NtQueryInformationProcess = (_NtQueryInformationProcess)GetProcAddress(hModule, "NtQueryInformationProcess");
	if(NULL == NtQueryInformationProcess)
	{
		FreeLibrary(hModule);
		return FALSE;
	}

	if(bWow64Env)
	{
		NtWow64QueryInformationProcess64 = (_NtQueryInformationProcess)GetProcAddress(hModule, "NtWow64QueryInformationProcess64");
		NtWow64ReadVirtualMemory64 = (_NtWow64ReadVirtualMemory64)GetProcAddress(hModule, "NtWow64ReadVirtualMemory64");
		if(NULL == NtWow64QueryInformationProcess64 || NULL == NtWow64ReadVirtualMemory64)
		{
			FreeLibrary(hModule);
			return FALSE;
		}
	}

	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(INVALID_HANDLE_VALUE != hProcessSnap && NULL != hProcessSnap)
	{
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);
		if(::Process32First(hProcessSnap, &pe32))
		{
			CString strProcessNameUpper = strProcessName; strProcessNameUpper.MakeUpper();
			do 
			{
				if(strProcessName.IsEmpty() || 0 <= CString(pe32.szExeFile).MakeUpper().Find(strProcessNameUpper))
				{
					WMI_Win32_Process Process;
					Process.strName = pe32.szExeFile;
					Process.dwProcessId = pe32.th32ProcessID;

					Process.cntThreads = pe32.cntThreads;
					Process.th32ParentProcessID = pe32.th32ParentProcessID;
					Process.pcPriClassBase = pe32.pcPriClassBase;

					HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
					if(NULL != hProcess)
					{
						HANDLE hToken = NULL;
						if(::OpenProcessToken(hProcess, TOKEN_QUERY, &hToken))
						{
							DWORD dwSize = 0;
							GetTokenInformation(hToken, TokenUser, NULL, 0, &dwSize);

							if(0 < dwSize)
							{
								TOKEN_OWNER* pOwner = (TOKEN_OWNER*)malloc(dwSize);
								if(NULL != pOwner)
								{
									if(GetTokenInformation(hToken, TokenUser, pOwner, dwSize, &dwSize))
									{
										SID_NAME_USE SidNameUse;
										CString strName, strDomain;
										DWORD dwNameLen = 1024, dwDomainLen = 1024;
										BOOL bLookup = LookupAccountSid(NULL, pOwner->Owner, strName.GetBuffer(1024), &dwNameLen, strDomain.GetBuffer(1024), &dwDomainLen, &SidNameUse);
										strName.ReleaseBuffer(); strDomain.ReleaseBuffer();

										LPTSTR pstrName = NULL;
										BOOL bConvert = ConvertSidToStringSid(pOwner->Owner, &pstrName);

										if(bLookup || bConvert)
										{
											if(!bLookup) strName = pstrName;

											Process.strDomain = strDomain;
											Process.strUser = strName;

											if(NULL != pstrName) LocalFree(pstrName);
										}
									}
									free(pOwner);
								}
							}

							::CloseHandle(hToken);
						}

						if(!bWow64Env)	// x86 시스템에서 x86 프로세스로 구동되거나 x64 시스템에서 x64 프로세스로 구동되는 경우
						{
							QueryProcessInfo(hProcess, Process, NtQueryInformationProcess);
						}
						else	// x64 시스템에서 x86 프로세스로 구동되는 경우
						{
							BOOL bWow64Proc = FALSE;
							if(IsWow64Process(hProcess, &bWow64Proc))
							{
								if(!bWow64Proc)	// x64 프로세스인 경우
								{
									QueryProcessInfo64(hProcess, Process, NtWow64QueryInformationProcess64, NtWow64ReadVirtualMemory64);
								}
								else	// x86 프로세스인 경우
								{
									QueryProcessInfo(hProcess, Process, NtQueryInformationProcess);
								}
							}
						}

						::CloseHandle(hProcess);
					}

					vecReturn.push_back(Process);
				}

				Sleep(1);
			} while (::Process32Next(hProcessSnap, &pe32));
		}

		CloseHandle(hProcessSnap);
	}

	FreeLibrary(hModule);
	return TRUE;
}