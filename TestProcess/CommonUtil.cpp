#include "stdafx.h"
#include "CommonUtil.h"

#include <WtsApi32.h>
#include <UserEnv.h>
#include <tlhelp32.h>
#pragma comment(lib,"WtsApi32.lib")
#pragma comment(lib,"UserEnv.lib")

typedef BOOL (WINAPI *LPWOW64DISABLEWOW64FSREDIRECTION)(PVOID*);
typedef BOOL (WINAPI *LPWOW64REVERTWOW64FSREDIRECTION)(PVOID);

CommonUtil::CommonUtil(void)
{
}

CommonUtil::~CommonUtil(void)
{
}

CString CommonUtil::GetSystemPath()
{
	CString strPath;
	::GetSystemDirectory(strPath.GetBuffer(1024), 1024);
	strPath.ReleaseBuffer();
	return strPath;
}

CString CommonUtil::GetSystemDrive()
{
	CString strDrive;
	SplitPath(GetSystemPath(), &strDrive, NULL, NULL, NULL);
	return strDrive;
}

CString CommonUtil::GetUserTempPath()
{
	CString strTempPath;
	::GetTempPath(MAX_PATH, strTempPath.GetBuffer(1024));
	strTempPath.ReleaseBuffer();

	int nIndex = strTempPath.ReverseFind('\\');
	if(0 <= nIndex && nIndex == strTempPath.GetLength()-1)
	{
		return strTempPath.Left(nIndex);
	}

	return strTempPath;
}

CString CommonUtil::GetWindowsPath()
{
	CString strPath;
	::GetWindowsDirectory(strPath.GetBuffer(1024), 1024);
	strPath.ReleaseBuffer();
    return strPath;
}

CString CommonUtil::GetLocalPath(CString* pLocalFileName, CString* pLocalFilePath)
{
	CString strFilePath;
	::GetModuleFileName(NULL, strFilePath.GetBuffer(1024), 1024);
	strFilePath.ReleaseBuffer();

	CString strDrive, strDir, strFilename, strExt;
	SplitPath(strFilePath, &strDrive, &strDir, &strFilename, &strExt);

	if(NULL != pLocalFileName) *pLocalFileName = strFilename+strExt;
	if(NULL != pLocalFilePath) *pLocalFilePath = strFilePath;

	return strDrive+strDir;
}

//======================================================================//
//            레지스트리의 값 데이터를 읽어온다.
//======================================================================//
BOOL CommonUtil::GetRegValue(HKEY plKey, CString szSubekey, CString szValueName, DWORD szDefualt, DWORD& pszRetVal)
{
    HKEY key;
    DWORD valueType;
    DWORD lpRet;
    DWORD Size = sizeof(lpRet);
	pszRetVal = szDefualt;

	REGSAM samDesired = KEY_ALL_ACCESS | (Is64BitWindows() ? KEY_WOW64_64KEY : KEY_WOW64_32KEY);
	if(RegOpenKeyEx((HKEY)plKey, szSubekey, NULL, samDesired, &key) == ERROR_SUCCESS )
	{
		if(RegQueryValueEx(key, szValueName, NULL, &valueType, (LPBYTE)&lpRet, &Size) == ERROR_SUCCESS)
		{
			pszRetVal = lpRet;
			RegCloseKey(key);
			return TRUE;
		}

		RegCloseKey(key);
	}

    return FALSE;
}

BOOL CommonUtil::GetRegValue32Or64(HKEY plKey, CString szSubekey, CString szValueName, DWORD szDefualt, DWORD& pszRetVal, int nFlag)
{
    HKEY key;
    DWORD valueType;
    DWORD lpRet;
    DWORD Size = sizeof(lpRet);
	pszRetVal = szDefualt;

	REGSAM samDesired = KEY_ALL_ACCESS | (0 == nFlag ? KEY_WOW64_32KEY : KEY_WOW64_64KEY);
	if(RegOpenKeyEx((HKEY)plKey, szSubekey, NULL, samDesired, &key) == ERROR_SUCCESS)
	{
		if(RegQueryValueEx(key, szValueName, NULL, &valueType, (LPBYTE)&lpRet, &Size) == ERROR_SUCCESS)
		{
			pszRetVal = lpRet;
			RegCloseKey(key);
			return TRUE;
		}

		RegCloseKey(key);
	}

    return FALSE;
}

BOOL CommonUtil::GetRegQValue(HKEY plKey, CString szSubekey, CString szValueName, QWORD szDefualt, QWORD& pszRetVal)
{
	HKEY key;
	DWORD valueType;
	QWORD lpRet;
	DWORD Size = sizeof(lpRet);
	pszRetVal = szDefualt;

	REGSAM samDesired = KEY_ALL_ACCESS | (Is64BitWindows() ? KEY_WOW64_64KEY : KEY_WOW64_32KEY);
	if(RegOpenKeyEx((HKEY)plKey, szSubekey, NULL, samDesired, &key) == ERROR_SUCCESS )
	{
		if(RegQueryValueEx(key, szValueName, NULL, &valueType, (LPBYTE)&lpRet, &Size) == ERROR_SUCCESS)
		{
			pszRetVal = lpRet;
			RegCloseKey(key);
			return TRUE;
		}

		RegCloseKey(key);
	}

	return FALSE;
}

BOOL CommonUtil::GetRegQValue32Or64(HKEY plKey, CString szSubekey, CString szValueName, QWORD szDefualt, QWORD& pszRetVal, int nFlag)
{
	HKEY key;
	DWORD valueType;
	QWORD lpRet;
	DWORD Size = sizeof(lpRet);
	pszRetVal = szDefualt;

	REGSAM samDesired = KEY_ALL_ACCESS | (0 == nFlag ? KEY_WOW64_32KEY : KEY_WOW64_64KEY);
	if(RegOpenKeyEx((HKEY)plKey, szSubekey, NULL, samDesired, &key) == ERROR_SUCCESS)
	{
		if(RegQueryValueEx(key, szValueName, NULL, &valueType, (LPBYTE)&lpRet, &Size) == ERROR_SUCCESS)
		{
			pszRetVal = lpRet;
			RegCloseKey(key);
			return TRUE;
		}

		RegCloseKey(key);
	}

	return FALSE;
}

//======================================================================//
//            레지스트리의 문자열 데이터를 읽어온다.
//======================================================================//
BOOL CommonUtil::GetRegString(HKEY plKey, CString szSubekey, CString szValueName, CString szDefualt, CString& pszRetVal)
{
	HKEY key;
	DWORD Size;
	DWORD valueType;
	pszRetVal = szDefualt;

	REGSAM samDesired = KEY_ALL_ACCESS | (Is64BitWindows() ? KEY_WOW64_64KEY : KEY_WOW64_32KEY);
	if(RegOpenKeyEx((HKEY)plKey, szSubekey, NULL, samDesired, &key) == ERROR_SUCCESS)
	{
		if(RegQueryValueEx(key, szValueName, NULL, NULL, NULL, &Size) == ERROR_SUCCESS)
		{
			if(0 < Size)
			{
				vector<BYTE> vecRetVal(Size);
				if(RegQueryValueEx(key, szValueName, NULL, &valueType, (LPBYTE)&vecRetVal[0], &Size) == ERROR_SUCCESS)
				{
					pszRetVal = (TCHAR*)&vecRetVal[0];
					RegCloseKey(key);
					return TRUE;
				}
			}
			else
			{
				pszRetVal.Empty();
				RegCloseKey(key);
				return TRUE;
			}
		}
		RegCloseKey(key);    
	}

	return FALSE;
}

BOOL CommonUtil::GetRegString32Or64(HKEY plKey, CString szSubekey, CString szValueName, CString szDefualt, CString& pszRetVal, int nFlag)
{
	HKEY key;
	DWORD Size;
	DWORD valueType;
	pszRetVal = szDefualt;

	REGSAM samDesired = KEY_ALL_ACCESS | (0 == nFlag ? KEY_WOW64_32KEY : KEY_WOW64_64KEY);
	if(RegOpenKeyEx((HKEY)plKey, szSubekey, NULL, samDesired, &key) == ERROR_SUCCESS)
	{
		if(RegQueryValueEx(key, szValueName, NULL, NULL, NULL, &Size) == ERROR_SUCCESS)
		{
			if(0 < Size)
			{
				vector<BYTE> vecRetVal(Size);
				if(RegQueryValueEx(key, szValueName, NULL, &valueType, (LPBYTE)&vecRetVal[0], &Size) == ERROR_SUCCESS)
				{
					pszRetVal = (TCHAR*)&vecRetVal[0];
					RegCloseKey(key);
					return TRUE;
				}
			}
			else
			{
				pszRetVal.Empty();
				RegCloseKey(key);
				return TRUE;
			}
		}
		RegCloseKey(key);
	}

	return FALSE;
}

BOOL CommonUtil::GetRegMultiString(HKEY plKey, CString szSubekey, CString szValueName, vector<CString>& pRetVal)
{
	HKEY key;
	DWORD Size;
	DWORD valueType;

	REGSAM samDesired = KEY_ALL_ACCESS | (Is64BitWindows() ? KEY_WOW64_64KEY : KEY_WOW64_32KEY);
	if(RegOpenKeyEx((HKEY)plKey, szSubekey, NULL, samDesired, &key) == ERROR_SUCCESS)
	{
		if(RegQueryValueEx(key, szValueName, NULL, NULL, NULL, &Size) == ERROR_SUCCESS)
		{
			if(0 < Size)
			{
				vector<BYTE> vecRetVal(Size);
				if(RegQueryValueEx(key, szValueName, NULL, &valueType, (LPBYTE)&vecRetVal[0], &Size) == ERROR_SUCCESS)
				{
					pRetVal.clear();
					size_t nOffset = 0;
					while(nOffset < Size)
					{
						CString strRetVal = (TCHAR*)&vecRetVal[nOffset];
						pRetVal.push_back(strRetVal);

						nOffset += (strRetVal.GetLength()+1)*sizeof(TCHAR);
					}
					RegCloseKey(key);
					return TRUE;
				}
			}
			else
			{
				pRetVal.clear();
				RegCloseKey(key);
				return TRUE;
			}
		}
		RegCloseKey(key);    
	}

	return FALSE;
}

BOOL CommonUtil::GetRegMultiString32Or64(HKEY plKey, CString szSubekey, CString szValueName, vector<CString>& pRetVal, int nFlag)
{
	HKEY key;
	DWORD Size;
	DWORD valueType;

	REGSAM samDesired = KEY_ALL_ACCESS | (0 == nFlag ? KEY_WOW64_32KEY : KEY_WOW64_64KEY);
	if(RegOpenKeyEx((HKEY)plKey, szSubekey, NULL, samDesired, &key) == ERROR_SUCCESS)
	{
		if(RegQueryValueEx(key, szValueName, NULL, NULL, NULL, &Size) == ERROR_SUCCESS)
		{
			if(0 < Size)
			{
				vector<BYTE> vecRetVal(Size);
				if(RegQueryValueEx(key, szValueName, NULL, &valueType, (LPBYTE)&vecRetVal[0], &Size) == ERROR_SUCCESS)
				{
					pRetVal.clear();
					size_t nOffset = 0;
					while(nOffset < Size)
					{
						CString strRetVal = (TCHAR*)&vecRetVal[nOffset];
						pRetVal.push_back(strRetVal);

						nOffset += (strRetVal.GetLength()+1)*sizeof(TCHAR);
					}
					RegCloseKey(key);
					return TRUE;
				}
			}
			else
			{
				pRetVal.clear();
				RegCloseKey(key);
				return TRUE;
			}
		}
		RegCloseKey(key);   
	}

	return FALSE;
}

BOOL CommonUtil::GetReg(HKEY plKey, CString szSubekey, CString szValueName, REGSAM samDesired, vector<BYTE>& pRetVal, DWORD& dwValueType)
{
	BOOL bRet = FALSE;
	HKEY key = NULL;
	DWORD Size = 0;
	pRetVal.clear();

	if(ERROR_SUCCESS == RegOpenKeyEx((HKEY)plKey, szSubekey, NULL, samDesired, &key))
	{
		bRet = (ERROR_SUCCESS == RegQueryValueEx(key, szValueName, NULL, &dwValueType, NULL, &Size));
		if(bRet && 0 < Size)
		{
			pRetVal.resize(Size, 0);
			bRet = (ERROR_SUCCESS == RegQueryValueEx(key, szValueName, NULL, &dwValueType, (LPBYTE)&pRetVal[0], &Size));
		}
		RegCloseKey(key);    
	}

	return bRet;
}

BOOL CommonUtil::GetReg(HKEY plKey, CString szSubekey, CString szValueName, REGSAM samDesired, vector<BYTE>& pRetVal, DWORD& dwValueType, CString* pstrCmdResult)
{
	BOOL bRet = GetReg(plKey, szSubekey, szValueName, samDesired, pRetVal, dwValueType);

	if(NULL != pstrCmdResult)
	{
		if(bRet)
		{
			switch(dwValueType)
			{
			case REG_SZ:
				pstrCmdResult->Format(_T("%s\tREG_SZ\t%s"), szValueName, (0 < pRetVal.size() ? (TCHAR*)&pRetVal[0] : _T("")));
				break;
			case REG_EXPAND_SZ:
				pstrCmdResult->Format(_T("%s\tREG_EXPAND_SZ\t%s"), szValueName, (0 < pRetVal.size() ? (TCHAR*)&pRetVal[0] : _T("")));
				break;
			case REG_BINARY:
				pstrCmdResult->Format(_T("%s\tREG_BINARY\t"), szValueName);
				for(size_t i1=0 ; i1<pRetVal.size() ; i1++)
				{
					CString strHex;
					strHex.Format(_T("%02X"), pRetVal[i1]);
					*pstrCmdResult += strHex;
				}
				break;
			case REG_DWORD:
				pstrCmdResult->Format(_T("%s\tREG_DWORD\t0x%x"), szValueName, (sizeof(DWORD) <= pRetVal.size() ? *(DWORD*)&pRetVal[0] : 0));
				break;
			case REG_MULTI_SZ:
				{
					pstrCmdResult->Format(_T("%s\tREG_MULTI_SZ\t"), szValueName);
					size_t nOffset = 0;
					while(nOffset < pRetVal.size())
					{
						CString strRetVal = (TCHAR*)&pRetVal[nOffset];
						if(!strRetVal.IsEmpty())
						{
							if(0 != nOffset) *pstrCmdResult += _T("\\0");
							*pstrCmdResult += strRetVal;
						}
						nOffset += (strRetVal.GetLength()+1)*sizeof(TCHAR);
					}
				}
				break;
			case REG_QWORD:
				{
					DWORD dwHigh = 0, dwLow = 0;
					if(sizeof(QWORD) <= pRetVal.size())
					{
						dwHigh = *(DWORD*)&pRetVal[sizeof(QWORD)/2];
						dwLow =  *(DWORD*)&pRetVal[0];
					}
					pstrCmdResult->Format(_T("%s\tREG_QWORD\t0x%x%x"), szValueName, dwHigh, dwLow);
				}
				break;
			default:
				pstrCmdResult->Format(_T("%s\tREG_UNKNOWN"), szValueName);
				break;
			}
		}
		else *pstrCmdResult = _T("ERROR: The system was unable to find the specified registry key or value.");
	}

	return bRet;
}

BOOL CommonUtil::GetReg(HKEY plKey, CString szSubekey, CString szValueName, vector<BYTE>& pRetVal, DWORD& dwValueType, CString* pstrCmdResult)
{
	REGSAM samDesired = KEY_ALL_ACCESS | (Is64BitWindows() ? KEY_WOW64_64KEY : KEY_WOW64_32KEY);
	return GetReg(plKey, szSubekey, szValueName, samDesired, pRetVal, dwValueType, pstrCmdResult);
}

BOOL CommonUtil::GetReg32Or64(HKEY plKey, CString szSubekey, CString szValueName, vector<BYTE>& pRetVal, DWORD& dwValueType, int nFlag, CString* pstrCmdResult)
{
	REGSAM samDesired = KEY_ALL_ACCESS | (0 == nFlag ? KEY_WOW64_32KEY : KEY_WOW64_64KEY);
	return GetReg(plKey, szSubekey, szValueName, samDesired, pRetVal, dwValueType, pstrCmdResult);
}

BOOL CommonUtil::GetRegDWord(HKEY plKey, CString szSubekey, CString szValueName, DWORD szDefualt, DWORD& pszRetVal, CString* pstrCmdResult)
{
	pszRetVal = szDefualt;

	vector<BYTE> vecValues;
	DWORD dwValueType = REG_NONE;
	if(GetReg(plKey, szSubekey, szValueName, vecValues, dwValueType, pstrCmdResult) && sizeof(DWORD) <= vecValues.size())
	{
		pszRetVal = *(DWORD*)&vecValues[0];
		return TRUE;
	}

	return FALSE;
}

BOOL CommonUtil::GetRegDWord32Or64(HKEY plKey, CString szSubekey, CString szValueName, DWORD szDefualt, DWORD& pszRetVal, int nFlag, CString* pstrCmdResult)
{
	pszRetVal = szDefualt;

	vector<BYTE> vecValues;
	DWORD dwValueType = REG_NONE;
	if(GetReg32Or64(plKey, szSubekey, szValueName, vecValues, dwValueType, nFlag, pstrCmdResult) && sizeof(DWORD) <= vecValues.size())
	{
		pszRetVal = *(DWORD*)&vecValues[0];
		return TRUE;
	}

	return FALSE;
}

BOOL CommonUtil::GetRegQWord(HKEY plKey, CString szSubekey, CString szValueName, QWORD szDefualt, QWORD& pszRetVal, CString* pstrCmdResult)
{
	pszRetVal = szDefualt;

	vector<BYTE> vecValues;
	DWORD dwValueType = REG_NONE;
	if(GetReg(plKey, szSubekey, szValueName, vecValues, dwValueType, pstrCmdResult) && sizeof(QWORD) <= vecValues.size())
	{
		pszRetVal = *(QWORD*)&vecValues[0];
		return TRUE;
	}

	return FALSE;
}

BOOL CommonUtil::GetRegQWord32Or64(HKEY plKey, CString szSubekey, CString szValueName, QWORD szDefualt, QWORD& pszRetVal, int nFlag, CString* pstrCmdResult)
{
	pszRetVal = szDefualt;

	vector<BYTE> vecValues;
	DWORD dwValueType = REG_NONE;
	if(GetReg32Or64(plKey, szSubekey, szValueName, vecValues, dwValueType, nFlag, pstrCmdResult) && sizeof(QWORD) <= vecValues.size())
	{
		pszRetVal = *(QWORD*)&vecValues[0];
		return TRUE;
	}

	return FALSE;
}

BOOL CommonUtil::GetRegSZ(HKEY plKey, CString szSubekey, CString szValueName, CString szDefualt, CString& pszRetVal, CString* pstrCmdResult)
{
	pszRetVal = szDefualt;

	vector<BYTE> vecValues;
	DWORD dwValueType = REG_NONE;
	if(GetReg(plKey, szSubekey, szValueName, vecValues, dwValueType, pstrCmdResult))
	{
		if(0 < vecValues.size())
		{
			pszRetVal = (TCHAR*)&vecValues[0];
		}
		else
		{
			pszRetVal.Empty();
		}
		return TRUE;
	}

	return FALSE;
}

BOOL CommonUtil::GetRegSZ32Or64(HKEY plKey, CString szSubekey, CString szValueName, CString szDefualt, CString& pszRetVal, int nFlag, CString* pstrCmdResult)
{
	pszRetVal = szDefualt;

	vector<BYTE> vecValues;
	DWORD dwValueType = REG_NONE;
	if(GetReg32Or64(plKey, szSubekey, szValueName, vecValues, dwValueType, nFlag, pstrCmdResult))
	{
		if(0 < vecValues.size())
		{
			pszRetVal = (TCHAR*)&vecValues[0];
		}
		else
		{
			pszRetVal.Empty();
		}
		return TRUE;
	}

	return FALSE;
}

BOOL CommonUtil::GetRegExpandSZ(HKEY plKey, CString szSubekey, CString szValueName, CString szDefualt, CString& pszRetVal, CString* pstrCmdResult)
{
	return GetRegSZ(plKey, szSubekey, szValueName, szDefualt, pszRetVal, pstrCmdResult);
}

BOOL CommonUtil::GetRegExpandSZ32Or64(HKEY plKey, CString szSubekey, CString szValueName, CString szDefualt, CString& pszRetVal, int nFlag, CString* pstrCmdResult)
{
	return GetRegSZ32Or64(plKey, szSubekey, szValueName, szDefualt, pszRetVal, nFlag, pstrCmdResult);
}

BOOL CommonUtil::GetRegMultiSZ(HKEY plKey, CString szSubekey, CString szValueName, vector<CString>& pRetVal, CString* pstrCmdResult)
{
	vector<BYTE> vecValues;
	DWORD dwValueType = REG_NONE;
	if(GetReg(plKey, szSubekey, szValueName, vecValues, dwValueType, pstrCmdResult))
	{
		pRetVal.clear();
		size_t nOffset = 0;
		while(nOffset < vecValues.size())
		{
			CString strRetVal = (TCHAR*)&vecValues[nOffset];
			if(!strRetVal.IsEmpty()) pRetVal.push_back(strRetVal);
			nOffset += (strRetVal.GetLength()+1)*sizeof(TCHAR);
		}
		return TRUE;
	}

	return FALSE;
}

BOOL CommonUtil::GetRegMultiSZ32Or64(HKEY plKey, CString szSubekey, CString szValueName, vector<CString>& pRetVal, int nFlag, CString* pstrCmdResult)
{
	vector<BYTE> vecValues;
	DWORD dwValueType = REG_NONE;
	if(GetReg32Or64(plKey, szSubekey, szValueName, vecValues, dwValueType, nFlag, pstrCmdResult))
	{
		pRetVal.clear();
		size_t nOffset = 0;
		while(nOffset < vecValues.size())
		{
			CString strRetVal = (TCHAR*)&vecValues[nOffset];
			if(!strRetVal.IsEmpty()) pRetVal.push_back(strRetVal);
			nOffset += (strRetVal.GetLength()+1)*sizeof(TCHAR);
		}
		return TRUE;
	}

	return FALSE;
}

BOOL CommonUtil::GetRegBinary(HKEY plKey, CString szSubekey, CString szValueName, vector<BYTE>& pRetVal, CString* pstrCmdResult)
{
	DWORD dwValueType = REG_NONE;
	return GetReg(plKey, szSubekey, szValueName, pRetVal, dwValueType, pstrCmdResult);
}

BOOL CommonUtil::GetRegBinary32Or64(HKEY plKey, CString szSubekey, CString szValueName, vector<BYTE>& pRetVal, int nFlag, CString* pstrCmdResult)
{
	DWORD dwValueType = REG_NONE;
	return GetReg32Or64(plKey, szSubekey, szValueName, pRetVal, dwValueType, nFlag, pstrCmdResult);
}

std::list<CString> CommonUtil::GetEnumValueList(HKEY plKey, CString szSubekey)
{
    HKEY key;
    DWORD dwIndex = 0;
    CString strValue;
    TCHAR  lpValue[MAX_PATH];
	DWORD Size = _countof(lpValue);
	std::list<CString> EnumList;

	REGSAM samDesired = KEY_ALL_ACCESS | (Is64BitWindows() ? KEY_WOW64_64KEY : KEY_WOW64_32KEY);
	if(RegOpenKeyEx((HKEY)plKey, szSubekey, NULL, samDesired, &key) == ERROR_SUCCESS)
	{
		while(RegEnumValue(key, dwIndex, lpValue, &Size, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
		{
			strValue = lpValue;
			EnumList.push_back(strValue);
			Size = _countof(lpValue);
			dwIndex++;
		}
		RegCloseKey(key); 
	}
    
    return EnumList;
}

std::list<CString> CommonUtil::GetEnumValueList32Or64(HKEY plKey, CString szSubekey, int nFlag)
{
	HKEY key;
	DWORD dwIndex = 0;
	CString strValue;
	TCHAR  lpValue[MAX_PATH];
	DWORD Size = _countof(lpValue);
	std::list<CString> EnumList;

	REGSAM samDesired = KEY_ALL_ACCESS | (0 == nFlag ? KEY_WOW64_32KEY : KEY_WOW64_64KEY);
	if(RegOpenKeyEx((HKEY)plKey, szSubekey, NULL, samDesired, &key) == ERROR_SUCCESS)
	{
		while(RegEnumValue(key, dwIndex, lpValue, &Size, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
		{
			strValue = lpValue;
			EnumList.push_back(strValue);
			Size = _countof(lpValue);
			dwIndex++;
		}
		RegCloseKey(key); 
	}

	return EnumList;
}

std::list<CString> CommonUtil::GetEnumKeyList(HKEY plKey, CString szSubekey)
{
    HKEY key;
    DWORD dwIndex = 0;
    CString strValue;
    TCHAR  lpValue[MAX_PATH];
	std::list<CString> EnumList;

	REGSAM samDesired = KEY_ALL_ACCESS | (Is64BitWindows() ? KEY_WOW64_64KEY : KEY_WOW64_32KEY);
	if(RegOpenKeyEx((HKEY)plKey, szSubekey, NULL, samDesired, &key) == ERROR_SUCCESS)
	{
		while(RegEnumKey(key, dwIndex, lpValue, _countof(lpValue)) == ERROR_SUCCESS)
		{
			strValue = lpValue;
			EnumList.push_back(strValue);
			dwIndex++;
		}
		RegCloseKey(key); 
	}
    
    return EnumList;
}

std::list<CString> CommonUtil::GetEnumKeyList32Or64(HKEY plKey, CString szSubekey, int nFlag)
{
	HKEY key;
	DWORD dwIndex = 0;
	CString strValue;
	TCHAR  lpValue[MAX_PATH];
	std::list<CString> EnumList;

	REGSAM samDesired = KEY_ALL_ACCESS | (0 == nFlag ? KEY_WOW64_32KEY : KEY_WOW64_64KEY);
	if(RegOpenKeyEx((HKEY)plKey, szSubekey, NULL, samDesired, &key) == ERROR_SUCCESS)
	{
		while(RegEnumKey(key, dwIndex, lpValue, _countof(lpValue)) == ERROR_SUCCESS)
		{
			strValue = lpValue;
			EnumList.push_back(strValue);
			dwIndex++;
		}
		RegCloseKey(key); 
	}

	return EnumList;
}

BOOL CommonUtil::SetRegString(HKEY plKey, CString szSubekey, CString szValueName, CString szValue)
{
	HKEY key;

	REGSAM samDesired = KEY_ALL_ACCESS | (Is64BitWindows() ? KEY_WOW64_64KEY : KEY_WOW64_32KEY);
	if(RegCreateKeyEx((HKEY)plKey, szSubekey, 0, NULL, 0, samDesired, NULL, &key, NULL) == ERROR_SUCCESS)
	{
		if(RegSetValueEx(key, szValueName, 0, REG_SZ, (BYTE*)(LPTSTR)(LPCTSTR)szValue, (szValue.GetLength()+1)*sizeof(TCHAR)) == ERROR_SUCCESS)
		{
			RegCloseKey(key);
			return TRUE;
		}

		RegCloseKey(key);
	}

	return FALSE;
}

BOOL CommonUtil::SetRegString32Or64(HKEY plKey, CString szSubekey, CString szValueName, CString szValue, int nFlag)
{
	HKEY key;

	REGSAM samDesired = KEY_ALL_ACCESS | (0 == nFlag ? KEY_WOW64_32KEY : KEY_WOW64_64KEY);
	if(RegCreateKeyEx((HKEY)plKey, szSubekey, 0, NULL, 0, samDesired, NULL, &key, NULL) == ERROR_SUCCESS)
	{
		if(RegSetValueEx(key, szValueName, 0, REG_SZ, (BYTE*)(LPTSTR)(LPCTSTR)szValue, (szValue.GetLength()+1)*sizeof(TCHAR)) == ERROR_SUCCESS)
		{
			RegCloseKey(key);
			return TRUE;
		}

		RegCloseKey(key);
	}

	return FALSE;
}

BOOL CommonUtil::SetRegValue(HKEY plKey, CString szSubekey, CString szValueName, DWORD dwValue)
{
	HKEY key;

	REGSAM samDesired = KEY_ALL_ACCESS | (Is64BitWindows() ? KEY_WOW64_64KEY : KEY_WOW64_32KEY);
	if(RegCreateKeyEx((HKEY)plKey, szSubekey, 0, NULL, 0, samDesired, NULL, &key, NULL) == ERROR_SUCCESS)
	{
		if(RegSetValueEx(key, szValueName, 0, REG_DWORD, (BYTE*)&dwValue, sizeof(DWORD)) == ERROR_SUCCESS)
		{
			RegCloseKey(key);
			return TRUE;
		}

		RegCloseKey(key);
	}

	return FALSE;
}

BOOL CommonUtil::SetRegValue32Or64(HKEY plKey, CString szSubekey, CString szValueName, DWORD dwValue, int nFlag)
{
	HKEY key;

	REGSAM samDesired = KEY_ALL_ACCESS | (0 == nFlag ? KEY_WOW64_32KEY : KEY_WOW64_64KEY);
	if(RegCreateKeyEx((HKEY)plKey, szSubekey, 0, NULL, 0, samDesired, NULL, &key, NULL) == ERROR_SUCCESS)
	{
		if(RegSetValueEx(key, szValueName, 0, REG_DWORD, (BYTE*)&dwValue, sizeof(DWORD)) == ERROR_SUCCESS)
		{
			RegCloseKey(key);
			return TRUE;
		}

		RegCloseKey(key);
	}

	return FALSE;
}

BOOL CommonUtil::DeleteRegString(HKEY plKey, CString szSubekey, CString szValueName)
{
	HKEY key;

	REGSAM samDesired = KEY_ALL_ACCESS | (Is64BitWindows() ? KEY_WOW64_64KEY : KEY_WOW64_32KEY);
	LONG lRet = RegOpenKeyEx((HKEY)plKey, szSubekey, NULL, samDesired, &key);
	if(ERROR_SUCCESS == lRet)
	{
		lRet = RegDeleteValue(key, szValueName);
		if(ERROR_SUCCESS == lRet || ERROR_FILE_NOT_FOUND == lRet)
		{
			RegCloseKey(key);
			return TRUE;
		}

		RegCloseKey(key);
	}
	else if(ERROR_FILE_NOT_FOUND == lRet)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CommonUtil::DeleteRegString32Or64(HKEY plKey, CString szSubekey, CString szValueName, int nFlag)
{
	HKEY key;

	REGSAM samDesired = KEY_ALL_ACCESS | (0 == nFlag ? KEY_WOW64_32KEY : KEY_WOW64_64KEY);
	LONG lRet = RegOpenKeyEx((HKEY)plKey, szSubekey, NULL, samDesired, &key);
	if(ERROR_SUCCESS == lRet)
	{
		lRet = RegDeleteValue(key, szValueName);
		if(ERROR_SUCCESS == lRet || ERROR_FILE_NOT_FOUND == lRet)
		{
			RegCloseKey(key);
			return TRUE;
		}

		RegCloseKey(key);
	}
	else if(ERROR_FILE_NOT_FOUND == lRet)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CommonUtil::Is64BitWindows()
{
#if defined(_WIN64)
    return TRUE;  // 64-bit programs run only on Win64
#elif defined(_WIN32)
    // 32-bit programs run on both 32-bit and 64-bit Windows
    // so must sniff
    BOOL f64 = FALSE;
    return IsWow64Process(GetCurrentProcess(), &f64) && f64;
#else
    return FALSE; // Win64 does not support Win16
#endif
}

BOOL CommonUtil::GetOSVersion(OSVERSIONINFOEX* os)
{
#ifdef UNICODE
	OSVERSIONINFOEXW* osw = os;
#else
	OSVERSIONINFOEXW o;
	OSVERSIONINFOEXW* osw = &o;
#endif

	HMODULE hMod = LoadLibrary(TEXT("ntdll.dll"));
	if(NULL == hMod)
	{
		return FALSE;
	}

	typedef void (WINAPI* RtlGetVersion) (OSVERSIONINFOEXW*);
	RtlGetVersion func = (RtlGetVersion)GetProcAddress(hMod, "RtlGetVersion");
	if(NULL == func)
	{
		FreeLibrary(hMod);
		return FALSE;
	}

	ZeroMemory(osw, sizeof(*osw));
	osw->dwOSVersionInfoSize = sizeof(*osw);
	func(osw);
#ifndef UNICODE
	os->dwBuildNumber = osw->dwBuildNumber;
	os->dwMajorVersion = osw->dwMajorVersion;
	os->dwMinorVersion = osw->dwMinorVersion;
	os->dwPlatformId = osw->dwPlatformId;
	os->dwOSVersionInfoSize = sizeof (*os);
	DWORD sz = sizeof (os->szCSDVersion);
	WCHAR* src = osw->szCSDVersion;
	unsigned char* dtc = (unsigned char *)os->szCSDVersion;
	while(*src)
		* Dtc++ = (unsigned char)* src++;
	*Dtc = '\ 0';
#endif

	FreeLibrary(hMod);
	return TRUE;
}

BOOL CommonUtil::GetOSVersion(DWORD* pMajorVersion, DWORD* pMinorVersion, BYTE* pProductType, CString* pOSName, CString* pOSString)
{
	OSVERSIONINFOEX osvi;
	if(!GetOSVersion(&osvi))
	{
		return FALSE;
	}

	if(NULL != pMajorVersion) *pMajorVersion = osvi.dwMajorVersion;
	if(NULL != pMinorVersion) *pMinorVersion = osvi.dwMinorVersion;
	if(NULL != pProductType) *pProductType = osvi.wProductType;
	if(NULL != pOSName || NULL != pOSString)
	{
		CString strOSName = GetOSVersionString(osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber, osvi.wProductType);
		if(NULL != pOSName) *pOSName = strOSName;
		if(NULL != pOSString)
		{
			if(0 == strOSName.CompareNoCase(_T("Windows 10"))) *pOSString = _T("WINDOWS_PC_10");
			if(0 == strOSName.CompareNoCase(_T("Windows Server 2019"))) *pOSString = _T("WINDOWS_SERVER_2019");
			if(0 == strOSName.CompareNoCase(_T("Windows Server 2016"))) *pOSString = _T("WINDOWS_SERVER_2016");
			if(0 == strOSName.CompareNoCase(_T("Windows 8.1"))) *pOSString = _T("WINDOWS_PC_8.1");
			if(0 == strOSName.CompareNoCase(_T("Windows Server 2012 R2"))) *pOSString = _T("WINDOWS_SERVER_2012R2");
			if(0 == strOSName.CompareNoCase(_T("Windows 8"))) *pOSString = _T("WINDOWS_PC_8");
			if(0 == strOSName.CompareNoCase(_T("Windows Server 2012"))) *pOSString = _T("WINDOWS_SERVER_2012");
			if(0 == strOSName.CompareNoCase(_T("Windows 7"))) *pOSString = _T("WINDOWS_PC_7");
			if(0 == strOSName.CompareNoCase(_T("Windows Server 2008 R2"))) *pOSString = _T("WINDOWS_SERVER_2008R2");
			if(0 == strOSName.CompareNoCase(_T("Windows Vista"))) *pOSString = _T("WINDOWS_PC_VISTA");
			if(0 == strOSName.CompareNoCase(_T("Windows Server 2008"))) *pOSString = _T("WINDOWS_SERVER_2008");
			if(0 == strOSName.CompareNoCase(_T("Windows XP"))) *pOSString = _T("WINDOWS_PC_XP");
			if(0 == strOSName.CompareNoCase(_T("Windows Server 2003"))) *pOSString = _T("WINDOWS_SERVER_2003");
			if(0 == strOSName.CompareNoCase(_T("Windows 2000"))) *pOSString = _T("WINDOWS_SERVER_2000");
		}
	}

	return TRUE;
}

CString CommonUtil::GetOSVersionString(DWORD dwMajorVersion, DWORD dwMinorVersion, DWORD dwBuildNumber, BYTE wProductType)
{
	switch(dwMajorVersion)
	{
	case 10:
		switch(dwMinorVersion)
		{
		case 0:
			switch(wProductType)
			{
			case VER_NT_WORKSTATION:
				return _T("Windows 10");
			case VER_NT_SERVER:
			case VER_NT_DOMAIN_CONTROLLER:
				if(17763 > dwBuildNumber) return _T("Windows Server 2016");
				else return _T("Windows Server 2019");
			}
			break;
		}
		break;
	case 6:
		switch(dwMinorVersion)
		{
		case 3:
			switch(wProductType)
			{
			case VER_NT_WORKSTATION:
				return _T("Windows 8.1");
			case VER_NT_SERVER:
			case VER_NT_DOMAIN_CONTROLLER:
				return _T("Windows Server 2012 R2");
			}
			break;
		case 2:
			switch(wProductType)
			{
			case VER_NT_WORKSTATION:
				return _T("Windows 8");
			case VER_NT_SERVER:
			case VER_NT_DOMAIN_CONTROLLER:
				return _T("Windows Server 2012");
			}
			break;
		case 1:
			switch(wProductType)
			{
			case VER_NT_WORKSTATION:
				return _T("Windows 7");
			case VER_NT_SERVER:
			case VER_NT_DOMAIN_CONTROLLER:
				return _T("Windows Server 2008 R2");
			}
			break;
		case 0:
			switch(wProductType)
			{
			case VER_NT_WORKSTATION:
				return _T("Windows Vista");
			case VER_NT_SERVER:
			case VER_NT_DOMAIN_CONTROLLER:
				return _T("Windows Server 2008");
			}
			break;
		}
		break;
	case 5:
		switch(dwMinorVersion)
		{
		case 2:
			switch(wProductType)
			{
			case VER_NT_WORKSTATION:
				return _T("Windows XP");
			case VER_NT_SERVER:
			case VER_NT_DOMAIN_CONTROLLER:
				return _T("Windows Server 2003");
			}
			break;
		case 1:
			return _T("Windows XP");
		case 0:
			return _T("Windows 2000");
		}
		break;
	case 4:
		return _T("Under Windows NT");
	}
	
	return _T("");
}

//======================================================================//
// 컴퓨터이름을 가져옵니다.
//======================================================================//
CString CommonUtil::GetComName()
{
	TCHAR szComName[255] = {0,};
    DWORD len=255;
    CString strComName;
    if(GetComputerName(szComName, &len))
	{
		strComName.Format(_T("%s"), szComName);
	}

    return strComName;
}

//======================================================================//
// 계정 이름을 가져옵니다.
//======================================================================//
CString CommonUtil::GetLoginUserName2()
{
	CString strUserName;

	DWORD dwSessionId = WTSGetActiveConsoleSessionId();

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(INVALID_HANDLE_VALUE == hSnap)
	{
		return strUserName;
	}

	PROCESSENTRY32 procEntry;
	procEntry.dwSize = sizeof(PROCESSENTRY32);
	if(!Process32First(hSnap, &procEntry))
	{
		CloseHandle(hSnap);
		return strUserName;
	}

	DWORD dwProcessID = 0;
	do
	{
		if(0 == _tcsicmp(procEntry.szExeFile, _T("explorer.exe")))
		{
			DWORD winlogonSessId = 0;
			if(ProcessIdToSessionId(procEntry.th32ProcessID, &winlogonSessId))
			{
				if(0 == dwProcessID)
				{
					dwProcessID = procEntry.th32ProcessID;
				}

				if(winlogonSessId == dwSessionId)
				{
					dwProcessID = procEntry.th32ProcessID;
					break;
				}
			}
		}
	}while(Process32Next(hSnap, &procEntry));

	if(0 == dwProcessID)
	{
		return strUserName;
	}

	HANDLE hProcess = OpenProcess(MAXIMUM_ALLOWED, FALSE, dwProcessID);
	if(NULL != hProcess)
	{
		HANDLE hToken = NULL;
		if(OpenProcessToken(hProcess,
			TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY
			|TOKEN_DUPLICATE|TOKEN_ASSIGN_PRIMARY|TOKEN_ADJUST_SESSIONID
			|TOKEN_READ|TOKEN_WRITE,
			&hToken))
		{
			DWORD dwReturnLength = 0;
			if(!GetTokenInformation(hToken, TokenUser, NULL, 0, &dwReturnLength))
			{
				if(ERROR_INSUFFICIENT_BUFFER == GetLastError() && 0 < dwReturnLength)
				{
					BYTE* pBuffer = new BYTE[dwReturnLength];
					if(NULL != pBuffer)
					{
						if(GetTokenInformation(hToken, TokenUser, pBuffer, dwReturnLength, &dwReturnLength))
						{
							DWORD dwNameLength = 0;
							DWORD dwDomainNameLength = 0;
							SID_NAME_USE snu;
							if(!LookupAccountSid(NULL, ((TOKEN_USER*)pBuffer)->User.Sid, NULL, &dwNameLength, NULL, &dwDomainNameLength, &snu))
							{
								if(0 < dwNameLength || 0 < dwDomainNameLength)
								{
									BOOL bMemOK = TRUE;

									TCHAR* pszName = NULL;
									TCHAR* pszDomainName = NULL;
									if(0 < dwNameLength)
									{
										pszName = new TCHAR[dwNameLength];
										if(NULL == pszName)
										{
											bMemOK = FALSE;
										}
									}
									if(0 < dwDomainNameLength)
									{
										pszDomainName = new TCHAR[dwDomainNameLength];
										if(NULL == pszDomainName)
										{
											bMemOK = FALSE;
										}
									}

									if(bMemOK)
									{
										if(LookupAccountSid(NULL, ((TOKEN_USER*)pBuffer)->User.Sid, pszName, &dwNameLength, pszDomainName, &dwDomainNameLength, &snu))
										{
											strUserName = pszName;
										}
									}

									if(NULL != pszName)
									{
										delete pszName;
									}
									if(NULL != pszDomainName)
									{
										delete pszDomainName;
									}
								}
							}
						}

						delete pBuffer;
					}
				}
			}
			CloseHandle(hToken);
		}
		CloseHandle(hProcess);
	}

	return strUserName;
}

CString CommonUtil::GetFileName(CString strFilePath)
{
	CString strFilename, strExt;
	SplitPath(strFilePath, NULL, NULL, &strFilename, &strExt);
	return strFilename+strExt;
}

CString CommonUtil::GetParentDirPath(CString strFilePath)
{
	CString strDrive, strDir;
	SplitPath(strFilePath, &strDrive, &strDir, NULL, NULL);

	CString strRet = strDrive+strDir;

	int nLen = strRet.GetLength();
	if(0 < nLen && _T('\\') == strRet.GetAt(nLen-1)) strRet = strRet.Left(nLen-1);
	return strRet;
}

void CommonUtil::SplitPath(CString strFilePath, CString* pstrDrive, CString* pstrDir, CString* pstrFilename, CString* pstrExt)
{
	TCHAR* pDrive = (NULL != pstrDrive ? pstrDrive->GetBuffer(1024) : NULL);
	TCHAR* pDir = (NULL != pstrDir ? pstrDir->GetBuffer(1024) : NULL);
	TCHAR* pFilename = (NULL != pstrFilename ? pstrFilename->GetBuffer(1024) : NULL);
	TCHAR* pExt = (NULL != pstrExt ? pstrExt->GetBuffer(1024) : NULL);

	_tsplitpath_s(strFilePath, pDrive, (NULL != pDrive ? 1024 : 0), pDir, (NULL != pDir ? 1024 : 0), pFilename, (NULL != pFilename ? 1024 : 0), pExt, (NULL != pExt ? 1024 : 0));

	if(NULL != pDrive) pstrDrive->ReleaseBuffer();
	if(NULL != pDir) pstrDir->ReleaseBuffer();
	if(NULL != pFilename) pstrFilename->ReleaseBuffer();
	if(NULL != pExt) pstrExt->ReleaseBuffer();
}

CString CommonUtil::StringFromUTF8(CStringA strUTF8)
{
#ifdef UNICODE
	return UTF8ToUnicode(strUTF8);
#else // ANSI
	return UTF8ToAnsi(strUTF8);
#endif
}

CString CommonUtil::StringFromAnsi(CStringA strAnsi)
{
#ifdef UNICODE
	return AnsiToUnicode(strAnsi);
#else // ANSI
	return strAnsi;
#endif
}

CString CommonUtil::StringFromOEM(CStringA strOEM)
{
#ifdef UNICODE
	return OEMToUnicode(strOEM);
#else // ANSI
	return OEMToAnsi(strOEM);
#endif
}

CString CommonUtil::StringFromUnicode(CStringW strUnicode)
{
#ifdef UNICODE
	return strUnicode;
#else // ANSI
	return UnicodeToAnsi(strUnicode);
#endif
}

CStringA CommonUtil::StringToUTF8(CString str)
{
#ifdef UNICODE
	return UnicodeToUTF8(str);
#else // ANSI
	return AnsiToUTF8(str);
#endif
}

CStringA CommonUtil::StringToAnsi(CString str)
{
#ifdef UNICODE
	return UnicodeToAnsi(str);
#else // ANSI
	return str;
#endif
}

CStringW CommonUtil::StringToUnicode(CString str)
{
#ifdef UNICODE
	return str;
#else // ANSI
	return AnsiToUnicode(str);
#endif
}

CStringA CommonUtil::StringToOEM(CString str)
{
#ifdef UNICODE
	return UnicodeToOEM(str);
#else // ANSI
	return AnsiToOEM(str);
#endif
}

CStringA CommonUtil::UnicodeToAnsi(CStringW strUnicode)
{
	// 길이를 구하고
	int nLength = ::WideCharToMultiByte(CP_ACP, 0, strUnicode, -1, NULL, 0, NULL, NULL);

	// 버퍼를 할당한 뒤
	int nAllocateLength = sizeof(CHAR) * nLength;
	LPSTR pBuffer = (LPSTR)malloc(nAllocateLength);

	ASSERT(pBuffer != NULL);
	memset(pBuffer, 0, nAllocateLength);

	// 변환한다
	::WideCharToMultiByte(CP_ACP, 0, strUnicode, -1, pBuffer, nLength, NULL, NULL);

	CStringA strResult = pBuffer;

	free(pBuffer);

	return strResult;
}

CStringW CommonUtil::AnsiToUnicode(CStringA strAnsi)
{
	// 길이를 구하고
	int nLength = ::MultiByteToWideChar(CP_ACP, 0, strAnsi, -1, NULL, 0);

	// 버퍼를 할당한 뒤
	int nBufferAllocatedSize = sizeof(WCHAR) * nLength;
	LPWSTR pBuffer = (LPWSTR)malloc(nBufferAllocatedSize);

	ASSERT(pBuffer != NULL);
	memset(pBuffer, 0, nBufferAllocatedSize);

	// 변환한다
	::MultiByteToWideChar(CP_ACP, 0, strAnsi, -1, pBuffer, nLength);

	CStringW strResult = pBuffer;

	free(pBuffer);

	return strResult;
}

CStringA CommonUtil::UnicodeToUTF8(CStringW strUnicode)
{
	// 길이를 구하고
	int nLength = ::WideCharToMultiByte(CP_UTF8, 0, strUnicode, -1, NULL, 0, NULL, NULL);

	// 버퍼를 할당한 뒤
	int nAllocateLength = sizeof(CHAR) * nLength;
	LPSTR pBuffer = (LPSTR)malloc(nAllocateLength);

	ASSERT(pBuffer != NULL);
	memset(pBuffer, 0, nAllocateLength);

	// 변환한다
	::WideCharToMultiByte(CP_UTF8, 0, strUnicode, -1, pBuffer, nLength, NULL, NULL);

	CStringA strResult = pBuffer;

	free(pBuffer);

	return strResult;
}

CStringW CommonUtil::UTF8ToUnicode(CStringA strUTF8)
{
	// 길이를 구하고
	int nLength = ::MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, NULL, 0);

	// 버퍼를 할당한 뒤
	int nBufferAllocatedSize = sizeof(WCHAR) * nLength;
	LPWSTR pBuffer = (LPWSTR)malloc(nBufferAllocatedSize);

	ASSERT(pBuffer != NULL);
	memset(pBuffer, 0, nBufferAllocatedSize);

	// 변환한다
	::MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, pBuffer, nLength);

	CStringW strResult = pBuffer;

	free(pBuffer);

	return strResult;
}

CStringA CommonUtil::UnicodeToOEM(CStringW strUnicode)
{
	// 길이를 구하고
	int nLength = ::WideCharToMultiByte(CP_OEMCP, 0, strUnicode, -1, NULL, 0, NULL, NULL);

	// 버퍼를 할당한 뒤
	int nAllocateLength = sizeof(CHAR) * nLength;
	LPSTR pBuffer = (LPSTR)malloc(nAllocateLength);

	ASSERT(pBuffer != NULL);
	memset(pBuffer, 0, nAllocateLength);

	// 변환한다
	::WideCharToMultiByte(CP_OEMCP, 0, strUnicode, -1, pBuffer, nLength, NULL, NULL);

	CStringA strResult = pBuffer;

	free(pBuffer);

	return strResult;
}

CStringW CommonUtil::OEMToUnicode(CStringA strOEM)
{
	// 길이를 구하고
	int nLength = ::MultiByteToWideChar(CP_OEMCP, 0, strOEM, -1, NULL, 0);

	// 버퍼를 할당한 뒤
	int nBufferAllocatedSize = sizeof(WCHAR) * nLength;
	LPWSTR pBuffer = (LPWSTR)malloc(nBufferAllocatedSize);

	ASSERT(pBuffer != NULL);
	memset(pBuffer, 0, nBufferAllocatedSize);

	// 변환한다
	::MultiByteToWideChar(CP_OEMCP, 0, strOEM, -1, pBuffer, nLength);

	CStringW strResult = pBuffer;

	free(pBuffer);

	return strResult;
}

CStringA CommonUtil::AnsiToUTF8(CStringA strAnsi)
{
	return UnicodeToUTF8(AnsiToUnicode(strAnsi));
}

CStringA CommonUtil::UTF8ToAnsi(CStringA strUTF8)
{
	return UnicodeToAnsi(UTF8ToUnicode(strUTF8));
}

CStringA CommonUtil::OEMToUTF8(CStringA strOEM)
{
	return UnicodeToUTF8(OEMToUnicode(strOEM));
}

CStringA CommonUtil::UTF8ToOEM(CStringA strUTF8)
{
	return UnicodeToOEM(UTF8ToUnicode(strUTF8));
}

CStringA CommonUtil::OEMToAnsi(CStringA strOEM)
{
	return UnicodeToAnsi(OEMToUnicode(strOEM));
}

CStringA CommonUtil::AnsiToOEM(CStringA strAnsi)
{
	return UnicodeToOEM(AnsiToUnicode(strAnsi));
}

//함수명 그대로 cmd실행 함수, 64비트 상태면 64비트 상태에 맞게 접근하게 설정도 함, 프로그램이 32bit 체제기 때문에
CString CommonUtil::ExecuteCmd(CString strCommand, DWORD dwTimeout)
{
	CString strPipeData;

	HANDLE hPipeRead = NULL;
	HANDLE hPipeWrite = NULL;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	SECURITY_ATTRIBUTES sa;

	ZeroMemory(&si, sizeof(STARTUPINFO));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	ZeroMemory(&sa, sizeof(SECURITY_ATTRIBUTES));

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;

	DWORD dwPipeSize = 1048576;	//1024*1024 = 1M
	if(!CreatePipe(&hPipeRead, &hPipeWrite, &sa, dwPipeSize))
	{
		return strPipeData;
	}

	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;
	si.hStdOutput = hPipeWrite;
	si.hStdError = hPipeWrite;

	PVOID pOldValue = NULL;
	Wow64Disable(&pOldValue);

	if(!CreateProcess(NULL, (LPTSTR)(LPCTSTR)strCommand, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
	{
		CloseHandle(hPipeRead);
		CloseHandle(hPipeWrite);
		Wow64Revert(pOldValue);
		return strPipeData;
	}

	char* pPipeData = NULL;
	DWORD dwTotalPipeRead = 0;
	BOOL bIsOK = TRUE;

	DWORD dwElapsedTime = 0;
	DWORD dwCurrentTime = GetTickCount();

	while(bIsOK) 
	{
		DWORD dwWait = WaitForSingleObject(pi.hProcess, 1000);

		DWORD dwTotalBytesAvail = 0;
		while(bIsOK = PeekNamedPipe(hPipeRead, NULL, 0, NULL, &dwTotalBytesAvail, NULL))
		{
			if(0 >= dwTotalBytesAvail) break;

			char* pReallocBuffer = (char*)realloc(pPipeData, dwTotalPipeRead+dwTotalBytesAvail+1);
			if(NULL == pReallocBuffer)
			{
				bIsOK = FALSE;
				break;
			}
			pPipeData = pReallocBuffer;

			char* pReadBuffer = pPipeData + dwTotalPipeRead;
			ZeroMemory(pReadBuffer, dwTotalBytesAvail+1);

			DWORD dwNumberOfBytesRead = 0;
			if(!ReadFile(hPipeRead, pReadBuffer, dwTotalBytesAvail, &dwNumberOfBytesRead, NULL) || dwTotalBytesAvail != dwNumberOfBytesRead)
			{
				bIsOK = FALSE;
				break;
			}

			dwTotalPipeRead += dwTotalBytesAvail;
			dwTotalBytesAvail = 0;
		}

		if(WAIT_TIMEOUT != dwWait) break;

		if(INFINITE != dwTimeout)
		{
			DWORD dwTickCount = GetTickCount();
			if(dwCurrentTime > dwTickCount)
			{
				dwElapsedTime += DWORD(-1) - dwCurrentTime;
				dwElapsedTime += dwTickCount;
			}
			else
			{
				dwElapsedTime += (dwTickCount - dwCurrentTime);
			}
			dwCurrentTime = dwTickCount;

			if(dwTimeout <= dwElapsedTime)
			{
				bIsOK = FALSE;
			}
		}
	}

	if(WAIT_TIMEOUT == WaitForSingleObject(pi.hProcess, 100))
		TerminateProcess(pi.hProcess, 0);

	if(bIsOK && NULL != pPipeData)
	{
		strPipeData = StringFromAnsi(pPipeData);
		strPipeData.Trim();
	}

	if(NULL != pPipeData) free(pPipeData);
	CloseHandle(hPipeRead);
	CloseHandle(hPipeWrite);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	Wow64Revert(pOldValue);

	return strPipeData;
}

CString CommonUtil::ExecuteCmd2(CString strCommand, BOOL bChangeCP, DWORD dwTimeout, int nRetry)
{
	strCommand.Trim();
	if(strCommand.IsEmpty())
	{
		return _T("");
	}

	if(0 >= nRetry) nRetry = 1;

	PVOID pOldValue = NULL;
	Wow64Disable(&pOldValue);

	CString strResult;

	for(int i=0 ; i<nRetry ; i++)
	{
		CString strResultFilePath;
		strResultFilePath.Format(_T("%s\\Result%lu_%lu.txt"), GetLocalPath(), GetCurrentProcessId(), _time32(NULL));

		CString strParam;
		if(bChangeCP)
			strParam.Format(_T("/c \"chcp 437 && %s 1> \"%s\" 2>&1\""), strCommand, strResultFilePath);
		else
			strParam.Format(_T("/c \"%s 1> \"%s\" 2>&1\""), strCommand, strResultFilePath);

		SHELLEXECUTEINFO sei;
		::ZeroMemory( &sei, sizeof(SHELLEXECUTEINFO) );

		sei.cbSize = sizeof(SHELLEXECUTEINFO);
		sei.lpFile = _T("cmd.exe");
		sei.lpParameters = (LPCTSTR)strParam;
		sei.nShow = SW_HIDE;
		sei.fMask = SEE_MASK_NOCLOSEPROCESS;
		sei.lpVerb = _T("open");

		if(ShellExecuteEx(&sei) && NULL != sei.hProcess)
		{
			// 프로세스 실행이 완료될때까지 대기
			if(WAIT_OBJECT_0 != WaitForSingleObject(sei.hProcess, dwTimeout))
			{
				::TerminateProcess(sei.hProcess, 0);
			}
			CloseHandle(sei.hProcess);
		}

		FILE* fp = _tfopen((LPCTSTR)strResultFilePath, _T("rb"));
		if(NULL != fp)
		{
			fseek(fp, 0, SEEK_END);
			long lFileLen = ftell(fp);
			fseek(fp, 0, SEEK_SET);

			if(0 < lFileLen)
			{
				BYTE* pBuffer = new BYTE[lFileLen+2];
				if(NULL != pBuffer)
				{
					ZeroMemory(pBuffer, lFileLen+2);
					size_t nReadSize = fread(pBuffer, lFileLen, 1, fp);
					if(1 != nReadSize)
					{
						delete[] pBuffer;
						return FALSE;
					}

					int nFileType = 0;
					if(3 <= lFileLen)
					{
						BYTE byUnicode[2] = {0xFF, 0xFE};
						BYTE byUtf8[3] = {0xEF, 0xBB, 0xBF};

						if(0 == memcmp(pBuffer, byUnicode, 2))
						{
							nFileType = 1;
						}
						else if(0 == memcmp(pBuffer, byUtf8, 3))
						{
							nFileType = 2;
						}
					}
					else if(2 <= lFileLen)
					{
						BYTE byUnicode[2] = {0xFF, 0xFE};

						if(0 == memcmp(pBuffer, byUnicode, 2))
						{
							nFileType = 1;
						}
					}

					switch(nFileType)
					{
					case 0:
						strResult = StringFromAnsi((char*)pBuffer);
						break;

					case 1:
						strResult = StringFromUnicode((wchar_t*)(pBuffer+2));
						break;

					case 2:
						strResult = StringFromUTF8((char*)(pBuffer+3));
						break;

					default:
						strResult = StringFromAnsi((char*)pBuffer);
						break;
					}

					strResult.Trim();

					delete[] pBuffer;
				}
			}
			fclose(fp);
		}

		DeleteFile(strResultFilePath);

		if(!strResult.IsEmpty()) break;

		Sleep(1000);
	}

	//////////////////////////////////////////////////////////////////////////
	// 64비트에서 실행시 SysWOW64로 리다이렉트 복구
	//////////////////////////////////////////////////////////////////////////
	Wow64Revert(pOldValue);

	return strResult;
}

CString CommonUtil::ExecuteCmd3(CString strCommand, CString* pstrError, DWORD dwTimeout)
{
	strCommand.Trim();
	if(strCommand.IsEmpty())
	{
		return _T("");
	}

	CString strResultFilePath;
	strResultFilePath.Format(_T("%s\\Result%lu_%lu.txt"), GetLocalPath(), GetCurrentProcessId(), _time32(NULL));

	CString strCmdCommand;
	strCmdCommand.Format(_T("cmd.exe /c \"%s > \"%s\"\""), strCommand, strResultFilePath);

	CString strCmdResult = ExecuteCmd(strCmdCommand, dwTimeout);
	if(NULL != pstrError) *pstrError = strCmdResult;

	CString strResult;

	PVOID pOldValue = NULL;
	Wow64Disable(&pOldValue);

	FILE* fp = _tfopen((LPCTSTR)strResultFilePath, _T("rb"));
	if(NULL != fp)
	{
		fseek(fp, 0, SEEK_END);
		long lFileLen = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		if(0 < lFileLen)
		{
			BYTE* pBuffer = new BYTE[lFileLen+2];
			if(NULL != pBuffer)
			{
				ZeroMemory(pBuffer, lFileLen+2);
				size_t nReadSize = fread(pBuffer, lFileLen, 1, fp);
				if(1 != nReadSize)
				{
					delete[] pBuffer;
					return FALSE;
				}

				int nFileType = 0;
				if(3 <= lFileLen)
				{
					BYTE byUnicode[2] = {0xFF, 0xFE};
					BYTE byUtf8[3] = {0xEF, 0xBB, 0xBF};

					if(0 == memcmp(pBuffer, byUnicode, 2))
					{
						nFileType = 1;
					}
					else if(0 == memcmp(pBuffer, byUtf8, 3))
					{
						nFileType = 2;
					}
				}
				else if(2 <= lFileLen)
				{
					BYTE byUnicode[2] = {0xFF, 0xFE};

					if(0 == memcmp(pBuffer, byUnicode, 2))
					{
						nFileType = 1;
					}
				}

				switch(nFileType)
				{
				case 0:
					strResult = StringFromAnsi((char*)pBuffer);
					break;

				case 1:
					strResult = StringFromUnicode((wchar_t*)(pBuffer+2));
					break;

				case 2:
					strResult = StringFromUTF8((char*)(pBuffer+3));
					break;

				default:
					strResult = StringFromAnsi((char*)pBuffer);
					break;
				}

				strResult.Trim();

				delete[] pBuffer;
			}
		}
		fclose(fp);
	}

	DeleteFile(strResultFilePath);

	Wow64Revert(pOldValue);

	return strResult;
}

std::vector<CString> CommonUtil::ParseResultToList(CString strResult, CString strDelimi, BOOL bEmptyDelete)
{
	strResult.Trim();

	std::vector<CString> vecCmdResult;

	// 점검 결과 라인별로 저장
	int nCurPos = 0;
	CString strToken = _T("");
	do
	{
		strToken = strResult.Tokenize(strDelimi, nCurPos);
		strToken.Trim();

		if(!strToken.IsEmpty() || !bEmptyDelete) vecCmdResult.push_back(strToken);
	} while(nCurPos > -1);

	return vecCmdResult;
}

std::map<CString, CString> CommonUtil::ParseResultToKeyValue(CString strResult, CString strMainDelimi, CString strSubDelimi, BOOL IsKeyMakeUpper)
{
	strResult.Trim();

	std::map<CString, CString>        mapCmdResult;

	if(strMainDelimi.IsEmpty() || strSubDelimi.IsEmpty())
	{
		return mapCmdResult;
	}

	// 점검 결과 라인별로 저장
	int nCurPos = 0;
	int nTotalLength = strResult.GetLength();

	CString strToken = _T("");
	do
	{
		strToken = strResult.Tokenize(strMainDelimi, nCurPos);
		strToken.Trim();

		int nValuePos = 0;
		CString strKey = strToken.Tokenize(strSubDelimi, nValuePos);
		if(-1 < nValuePos && nValuePos <= strToken.GetLength())
		{
			CString strValue = strToken.Mid(nValuePos, strToken.GetLength() - nValuePos);

			strKey.Trim();
			strValue.Trim();

			if(IsKeyMakeUpper)
			{
				strKey.MakeUpper();
			}
			mapCmdResult.insert(std::make_pair(strKey, strValue));
		}
	} while(nCurPos > -1 && nCurPos <= nTotalLength);

	return mapCmdResult;

}

std::vector<std::map<CString, CString>> CommonUtil::ParseResultToTable(CString strResult, CString strMainDelimi, CString strSubDelimi, BOOL IsHeadMakeUpper)
{
	strResult.Trim();

	std::vector<std::map<CString, CString>> vecCmdResult;

	if(strMainDelimi.IsEmpty())
	{
		return vecCmdResult;
	}

	// 점검 결과 라인별로 저장
	int nCurPos = 0;
	int nTotalLength = strResult.GetLength();

	CString strToken = _T("");

	std::vector<CString> vecHeader;
	BOOL bFirst = TRUE;

	do
	{
		strToken = strResult.Tokenize(strMainDelimi, nCurPos);
		strToken.Trim();

		if(bFirst)
		{
			if(strSubDelimi.IsEmpty())
			{
				vecHeader.push_back(strToken);
			}
			else
			{
				int nNextPos = 0;
				int nIndex = 0;
				int nLength = strToken.GetLength();
				do 
				{
					CString strData = strToken.Tokenize(strSubDelimi, nNextPos);
					strData.Trim();
					if(IsHeadMakeUpper)
					{
						strData.MakeUpper();
					}
					vecHeader.push_back(strData);
				} while (-1 < nNextPos && nNextPos <= nLength);
			}

			bFirst = FALSE;
		}
		else
		{
			std::map<CString, CString> mapItem;

			if(strSubDelimi.IsEmpty())
			{
				mapItem.insert(std::make_pair(vecHeader[0], strToken));
			}
			else
			{
				int nPrevPos = 0;
				int nNextPos = 0;
				int nIndex = 0;
				int nLength = strToken.GetLength();
				do 
				{
					if(nIndex >= (int)vecHeader.size())
					{
						break;
					}

					CString strData = strToken.Tokenize(strSubDelimi, nNextPos);

					int nSkipCount = nNextPos - (nPrevPos + strData.GetLength() + 1);
					for(int i=0 ; i<nSkipCount ; i++)
					{
						mapItem.insert(std::make_pair(vecHeader[nIndex++], _T("")));
					}
					nPrevPos = nNextPos;

					strData.Trim();
					mapItem.insert(std::make_pair(vecHeader[nIndex++], strData));
				} while (-1 < nNextPos && nNextPos <= nLength);
			}

			vecCmdResult.push_back(mapItem);
		}
		
	} while(nCurPos > -1 && nCurPos <= nTotalLength);

	return vecCmdResult;
}

CString CommonUtil::IntToStr(int nNum)
{
	CString str;
	str.Format(_T("%d"), nNum);
	return str;
}

CString CommonUtil::UIntToStr(unsigned int nNum)
{
	CString str;
	str.Format(_T("%u"), nNum);
	return str;
}

CString CommonUtil::Int64ToStr(__int64 nNum)
{
	CString str;
	str.Format(_T("%lld"), nNum);
	return str;
}

CString CommonUtil::UInt64ToStr(unsigned __int64 nNum)
{
	CString str;
	str.Format(_T("%llu"), nNum);
	return str;
}

BOOL CommonUtil::CreateProcessOnLoginSession(CString strProcess, WORD wShowWindow, LPPROCESS_INFORMATION lpProcessInformation)
{
	DWORD dwSessionId;
	DWORD winlogonPid;
	DWORD dwCreationFlags;
	HANDLE hUserToken;
	HANDLE hUserTokenDup;
	HANDLE hPToken;
	HANDLE hProcess;

	dwSessionId = WTSGetActiveConsoleSessionId();

	PROCESSENTRY32 procEntry;

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	procEntry.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hSnap, &procEntry))
	{
		CloseHandle(hSnap);
		return FALSE;
	}

	do
	{
		if (_tcsicmp(procEntry.szExeFile, _T("explorer.exe")) == 0)
		{
			DWORD winlogonSessId = 0;
			if (ProcessIdToSessionId(procEntry.th32ProcessID, &winlogonSessId) && winlogonSessId == dwSessionId)
			{
				winlogonPid = procEntry.th32ProcessID;
				break;
			}
		}
	}while (Process32Next(hSnap, &procEntry));

	CloseHandle(hSnap);

	if(!WTSQueryUserToken(dwSessionId, &hUserToken))
	{
		return FALSE;
	}

	dwCreationFlags = NORMAL_PRIORITY_CLASS|CREATE_NEW_CONSOLE;

	TOKEN_PRIVILEGES tp;
	LUID luid;
	hProcess = OpenProcess(MAXIMUM_ALLOWED, FALSE, winlogonPid);
	if(NULL == hProcess)
	{
		CloseHandle(hUserToken);
		return FALSE;
	}

	if(!OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY
		|TOKEN_DUPLICATE|TOKEN_ASSIGN_PRIMARY|TOKEN_ADJUST_SESSIONID
		|TOKEN_READ|TOKEN_WRITE, &hPToken))
	{
		CloseHandle(hUserToken);
		CloseHandle(hProcess);
		return FALSE;
	}

	if(!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid))
	{
		CloseHandle(hUserToken);
		CloseHandle(hProcess);
		CloseHandle(hPToken);
		return FALSE;
	}

	tp.PrivilegeCount =1;
	tp.Privileges[0].Luid =luid;
	tp.Privileges[0].Attributes =SE_PRIVILEGE_ENABLED;

	if(!DuplicateTokenEx(hPToken, MAXIMUM_ALLOWED, NULL, SecurityIdentification, TokenPrimary, &hUserTokenDup))
	{
		CloseHandle(hUserToken);
		CloseHandle(hProcess);
		CloseHandle(hPToken);
		return FALSE;
	}

	if(!SetTokenInformation(hUserTokenDup, TokenSessionId, (LPVOID)&dwSessionId, sizeof(DWORD)))
	{
		CloseHandle(hUserToken);
		CloseHandle(hProcess);
		CloseHandle(hPToken);
		CloseHandle(hUserTokenDup);
		return FALSE;
	}

	if(!AdjustTokenPrivileges(hUserTokenDup, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, NULL))
	{
		CloseHandle(hUserToken);
		CloseHandle(hProcess);
		CloseHandle(hPToken);
		CloseHandle(hUserTokenDup);
		return FALSE;
	}

	LPVOID pEnv =NULL;

	if(CreateEnvironmentBlock(&pEnv, hUserTokenDup, TRUE))
	{
		dwCreationFlags|=CREATE_UNICODE_ENVIRONMENT;
	}
	else
	{
		pEnv=NULL;
	}

	STARTUPINFO StartupInfo;
	ZeroMemory(&StartupInfo, sizeof(STARTUPINFO));
	StartupInfo.cb= sizeof(STARTUPINFO);
	StartupInfo.lpDesktop = _T("winsta0\\default");
	StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
	StartupInfo.wShowWindow = wShowWindow;

	if(!CreateProcessAsUser(hUserTokenDup, NULL, (LPTSTR)(LPCTSTR)strProcess, NULL, NULL, FALSE, dwCreationFlags, pEnv, NULL, &StartupInfo, lpProcessInformation))
	{
		CloseHandle(hUserToken);
		CloseHandle(hProcess);
		CloseHandle(hPToken);
		CloseHandle(hUserTokenDup);
		return FALSE;
	}

	CloseHandle(hProcess);
	CloseHandle(hUserToken);
	CloseHandle(hUserTokenDup);
	CloseHandle(hPToken);

	return TRUE;
}

BOOL CommonUtil::CreateProcessOnLoginSession(CString strProcess, WORD wShowWindow, BOOL bWait)
{
	PROCESS_INFORMATION ProcessInfo;
	ZeroMemory(&ProcessInfo, sizeof(PROCESS_INFORMATION));

	if(!CreateProcessOnLoginSession(strProcess, wShowWindow, &ProcessInfo))
	{
		return FALSE;
	}

	if(bWait)
	{
		WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
	}

	CloseHandle(ProcessInfo.hProcess);
	CloseHandle(ProcessInfo.hThread);

	return TRUE;
}

BOOL CommonUtil::CreateProcessOnLoginSessionUACElevation(CString strProcess, WORD wShowWindow, LPPROCESS_INFORMATION lpProcessInformation)
{
	DWORD dwSessionId;
	DWORD winlogonPid;
	DWORD dwCreationFlags;
	HANDLE hUserToken;
	HANDLE hUserTokenDup;
	HANDLE hPrimaryToken;
	HANDLE hPToken;
	HANDLE hProcess;

	dwSessionId = WTSGetActiveConsoleSessionId();

	PROCESSENTRY32 procEntry;

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	procEntry.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hSnap, &procEntry))
	{
		CloseHandle(hSnap);
		return FALSE;
	}

	do
	{
		if (_tcsicmp(procEntry.szExeFile, _T("explorer.exe")) == 0)
		{
			DWORD winlogonSessId = 0;
			if (ProcessIdToSessionId(procEntry.th32ProcessID, &winlogonSessId) && winlogonSessId == dwSessionId)
			{
				winlogonPid = procEntry.th32ProcessID;
				break;
			}
		}
	}while (Process32Next(hSnap, &procEntry));

	CloseHandle(hSnap);

	if(!WTSQueryUserToken(dwSessionId, &hUserToken))
	{
		return FALSE;
	}

	dwCreationFlags = NORMAL_PRIORITY_CLASS|CREATE_NEW_CONSOLE;

	TOKEN_PRIVILEGES tp;
	LUID luid;
	hProcess = OpenProcess(MAXIMUM_ALLOWED, FALSE, winlogonPid);
	if(NULL == hProcess)
	{
		CloseHandle(hUserToken);
		return FALSE;
	}

	if(!OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY
		|TOKEN_DUPLICATE|TOKEN_ASSIGN_PRIMARY|TOKEN_ADJUST_SESSIONID
		|TOKEN_READ|TOKEN_WRITE, &hPToken))
	{
		CloseHandle(hUserToken);
		CloseHandle(hProcess);
		return FALSE;
	}

	if(!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid))
	{
		CloseHandle(hUserToken);
		CloseHandle(hProcess);
		CloseHandle(hPToken);
		return FALSE;
	}

	tp.PrivilegeCount =1;
	tp.Privileges[0].Luid =luid;
	tp.Privileges[0].Attributes =SE_PRIVILEGE_ENABLED;

	if(!DuplicateTokenEx(hPToken, MAXIMUM_ALLOWED, NULL, SecurityIdentification, TokenPrimary, &hUserTokenDup))
	{
		CloseHandle(hUserToken);
		CloseHandle(hProcess);
		CloseHandle(hPToken);
		return FALSE;
	}

	TOKEN_LINKED_TOKEN linkedToken = {0};
	DWORD dwSize = sizeof(linkedToken);
	if(GetTokenInformation(hPToken, TokenLinkedToken, &linkedToken, dwSize, &dwSize))
	{
		if(!DuplicateTokenEx(linkedToken.LinkedToken, MAXIMUM_ALLOWED, NULL, SecurityImpersonation, TokenPrimary, &hPrimaryToken))
		{
			if(NULL != linkedToken.LinkedToken) CloseHandle(linkedToken.LinkedToken);
			CloseHandle(hUserToken);
			CloseHandle(hProcess);
			CloseHandle(hPToken);
			CloseHandle(hUserTokenDup);
			return FALSE;
		}
	}
	else
	{
		DWORD dwErr = ::GetLastError();
		if(!DuplicateTokenEx(hPToken, MAXIMUM_ALLOWED, NULL, SecurityImpersonation, TokenPrimary, &hPrimaryToken))
		{
			CloseHandle(hUserToken);
			CloseHandle(hProcess);
			CloseHandle(hPToken);
			CloseHandle(hUserTokenDup);
			return FALSE;
		}
	}

	if(!SetTokenInformation(hUserTokenDup, TokenSessionId, (LPVOID)&dwSessionId, sizeof(DWORD)))
	{
		if(NULL != linkedToken.LinkedToken) CloseHandle(linkedToken.LinkedToken);
		CloseHandle(hUserToken);
		CloseHandle(hProcess);
		CloseHandle(hPToken);
		CloseHandle(hUserTokenDup);
		CloseHandle(hPrimaryToken);
		return FALSE;
	}

	if(!AdjustTokenPrivileges(hUserTokenDup, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, NULL))
	{
		if(NULL != linkedToken.LinkedToken) CloseHandle(linkedToken.LinkedToken);
		CloseHandle(hUserToken);
		CloseHandle(hProcess);
		CloseHandle(hPToken);
		CloseHandle(hUserTokenDup);
		CloseHandle(hPrimaryToken);
		return FALSE;
	}

	LPVOID pEnv =NULL;

	if(CreateEnvironmentBlock(&pEnv, hUserTokenDup, TRUE))
	{
		dwCreationFlags|=CREATE_UNICODE_ENVIRONMENT;
	}
	else
	{
		pEnv=NULL;
	}

	STARTUPINFO StartupInfo;
	ZeroMemory(&StartupInfo, sizeof(STARTUPINFO));
	StartupInfo.cb= sizeof(STARTUPINFO);
	StartupInfo.lpDesktop = _T("winsta0\\default");
	StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
	StartupInfo.wShowWindow = wShowWindow;

	if(!CreateProcessAsUser(hPrimaryToken, NULL, (LPTSTR)(LPCTSTR)strProcess, NULL, NULL, FALSE, dwCreationFlags, pEnv, NULL, &StartupInfo, lpProcessInformation))
	{
		DWORD dwErr = ::GetLastError();

		if(NULL != linkedToken.LinkedToken) CloseHandle(linkedToken.LinkedToken);
		CloseHandle(hUserToken);
		CloseHandle(hProcess);
		CloseHandle(hPToken);
		CloseHandle(hUserTokenDup);
		CloseHandle(hPrimaryToken);
		return FALSE;
	}

	if(NULL != linkedToken.LinkedToken) CloseHandle(linkedToken.LinkedToken);
	CloseHandle(hProcess);
	CloseHandle(hUserToken);
	CloseHandle(hUserTokenDup);
	CloseHandle(hPrimaryToken);
	CloseHandle(hPToken);

	return TRUE;
}

BOOL CommonUtil::CreateProcessOnLoginSessionUACElevation(CString strProcess, WORD wShowWindow, BOOL bWait)
{
	PROCESS_INFORMATION ProcessInfo;
	ZeroMemory(&ProcessInfo, sizeof(PROCESS_INFORMATION));

	if(!CreateProcessOnLoginSessionUACElevation(strProcess, wShowWindow, &ProcessInfo))
	{
		return FALSE;
	}

	if(bWait)
	{
		WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
	}

	CloseHandle(ProcessInfo.hProcess);
	CloseHandle(ProcessInfo.hThread);

	return TRUE;
}

BOOL CommonUtil::CreateProcessOnCurrentSession(CString strProcess, WORD wShowWindow, LPPROCESS_INFORMATION lpProcessInformation)
{
	STARTUPINFO StartupInfo;
	ZeroMemory(&StartupInfo, sizeof(STARTUPINFO));
	StartupInfo.cb = sizeof(STARTUPINFO);
	StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
	StartupInfo.wShowWindow = wShowWindow;

	if(!CreateProcess(NULL, (LPTSTR)(LPCTSTR)strProcess, NULL, NULL, FALSE, 0, NULL, NULL, &StartupInfo, lpProcessInformation))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CommonUtil::CreateProcessOnCurrentSession(CString strProcess, WORD wShowWindow, BOOL bWait)
{
	PROCESS_INFORMATION ProcessInfo;
	ZeroMemory(&ProcessInfo, sizeof(PROCESS_INFORMATION));

	if(!CreateProcessOnCurrentSession(strProcess, wShowWindow, &ProcessInfo))
	{
		return FALSE;
	}

	if(bWait)
	{
		WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
	}

	CloseHandle(ProcessInfo.hProcess);
	CloseHandle(ProcessInfo.hThread);

	return TRUE;
}

BOOL CommonUtil::CreateDirectoryEx(CString strFolderPath)
{
	HANDLE hFind;
	WIN32_FIND_DATA findData;
	CString strTemp = strFolderPath;
	int nEnd = strFolderPath.GetLength();
	int nStart = nEnd;

	BOOL bRootDir = FALSE;
	hFind = ::FindFirstFile(strTemp, &findData);
	while(INVALID_HANDLE_VALUE == hFind)
	{
		int nTemp = strTemp.ReverseFind(_T('\\'));

		if(-1 == nTemp)
		{
			TCHAR szBuffer[1024] = {0};
			if(0 == GetLogicalDriveStrings(sizeof(szBuffer), szBuffer))
			{
				return FALSE;
			}

			int nOffSet = 0;
			while(NULL != szBuffer[nOffSet])
			{
				CString strDriveName = &szBuffer[nOffSet];
				if(0 == strTemp.CompareNoCase(strDriveName.Left(strDriveName.ReverseFind(_T('\\')))))
				{
					bRootDir = TRUE;
					break;
				}
				nOffSet = nOffSet + strDriveName.GetLength() + 1;
			}

			if(bRootDir)
			{
				break;
			}
			else
			{
				return FALSE;
			}
		}

		nStart = nTemp;
		strTemp = strTemp.Left(nStart);

		hFind = ::FindFirstFile(strTemp, &findData);
	}
	::FindClose(hFind);

	if(!bRootDir && FILE_ATTRIBUTE_DIRECTORY != (FILE_ATTRIBUTE_DIRECTORY & findData.dwFileAttributes))
	{
		return FALSE;
	}

	while(nStart < nEnd)
	{
		nStart = strFolderPath.Find(_T('\\'), nStart+1);
		if(-1 == nStart)
		{
			nStart = nEnd;
		}

		strTemp = strFolderPath.Left(nStart);
		if(!::CreateDirectory(strTemp, NULL))
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CommonUtil::DeleteDirectory(CString strDirPath)
{
	CFileFind finder;
	BOOL bWorking;
	BOOL bResult = TRUE;
	CString strAllFile;

	strAllFile = strDirPath;
	strAllFile += _T("\\*.*");

	bWorking = finder.FindFile(strAllFile);
	while(bWorking)
	{
		bWorking = finder.FindNextFile();
		CString strFileName = finder.GetFileName();
		CString strFilePath = finder.GetFilePath();
		if(!finder.IsDots())
		{
			if(finder.IsDirectory())
			{
				if(!DeleteDirectory(strFilePath))
					bResult = FALSE;
			}
			else
			{
				if(!::DeleteFile(strFilePath))
					bResult = FALSE;
			}
		}
	}

	finder.Close();

	if(!::RemoveDirectory(strDirPath))
		bResult = FALSE;

	return bResult;
}

BOOL CommonUtil::SetPrivilegesToCurrentThread()
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
	return TRUE;
}

BOOL CommonUtil::TerminateProcessEx(DWORD dwProcessId, UINT uExitCode)
{
	SetPrivilegesToCurrentThread();

	BOOL bRet = FALSE;
	HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
	if(NULL != hProcess)
	{
		bRet = ::TerminateProcess(hProcess, uExitCode);
		::CloseHandle(hProcess);
	}

	return bRet;
}

BOOL CommonUtil::Wow64Disable(PVOID* ppOldValue)
{
#if !defined(_WIN64)
	if(CommonUtil::Is64BitWindows())
	{
		LPWOW64DISABLEWOW64FSREDIRECTION lpfWow64DisableWow64FsRedirection = NULL;

		HMODULE hMod = GetModuleHandle(_T("Kernel32.dll"));
		if(NULL == hMod)
		{
			return FALSE;
		}

		lpfWow64DisableWow64FsRedirection = (LPWOW64DISABLEWOW64FSREDIRECTION)GetProcAddress(hMod, "Wow64DisableWow64FsRedirection");
		if(NULL == lpfWow64DisableWow64FsRedirection)
		{
			return FALSE;
		}

		return (*lpfWow64DisableWow64FsRedirection)(ppOldValue);
	}
#endif

	return TRUE;
}

BOOL CommonUtil::Wow64Revert(PVOID pOldValue)
{
#if !defined(_WIN64)
	if(CommonUtil::Is64BitWindows())
	{
		LPWOW64REVERTWOW64FSREDIRECTION lpfWow64RevertWow64FsRedirection = NULL;

		HMODULE hMod = GetModuleHandle(_T("Kernel32.dll"));
		if(NULL == hMod)
		{
			return FALSE;
		}

		lpfWow64RevertWow64FsRedirection = (LPWOW64REVERTWOW64FSREDIRECTION)GetProcAddress(hMod, "Wow64RevertWow64FsRedirection");
		if(NULL == lpfWow64RevertWow64FsRedirection)
		{
			return FALSE;
		}

		return (*lpfWow64RevertWow64FsRedirection)(pOldValue);
	}
#endif

	return TRUE;
}

HANDLE CommonUtil::HaveMutex(CString strMutexName, BOOL bExistFail)
{
	HANDLE hMutex = ::CreateMutex(NULL, FALSE, strMutexName);
	if(NULL == hMutex)
	{
		return NULL;
	}

	if(bExistFail && ERROR_ALREADY_EXISTS == ::GetLastError())
	{
		CloseHandle(hMutex);
		return NULL;
	}

	return hMutex;
}

BOOL CommonUtil::CheckMutex(CString strMutexName)
{
	HANDLE hMutex = ::OpenMutex(MUTEX_ALL_ACCESS, FALSE, strMutexName);  
	if(NULL == hMutex)
	{
		if(ERROR_ACCESS_DENIED == ::GetLastError())
		{
			return TRUE;
		}

		return FALSE;
	}

	CloseHandle(hMutex);

	return TRUE;
}

BOOL CommonUtil::GetVarOfSetFromFile(CString strFilePath, CString strVarName, CString& strValue)
{
	strVarName.Trim();
	if(strVarName.IsEmpty())
	{
		return FALSE;
	}

	FILE* fp = NULL;
	if(0 != _tfopen_s(&fp, strFilePath, _T("r")) || NULL == fp)
	{
		return FALSE;
	}

	BOOL bRet = FALSE;

	CString strBuffer;
	while(NULL != _fgetts(strBuffer.GetBuffer(1024), 1024, fp))
	{
		strBuffer.ReleaseBuffer();
		CString strTmpBuffer = strBuffer.Trim();
		strTmpBuffer.MakeUpper();
		if(0 == strTmpBuffer.Find(_T("SET")))
		{
			CString strSetData = strBuffer.Right(strBuffer.GetLength()-_tcslen(_T("SET")));
			strSetData.Trim();
			map<CString, CString> mapSet = CommonUtil::ParseResultToKeyValue(strSetData, _T("\n"), _T("="), FALSE);
			map<CString, CString>::iterator it = mapSet.find(strVarName);
			if(mapSet.end() != it)
			{
				strValue = it->second;
				bRet = TRUE;
				break;
			}
		}
		strBuffer.Empty();
	}

	fclose(fp);
	return bRet;
}

BOOL CommonUtil::GetVarOfKeyFromFile(CString strFilePath, CString strKeyName, CString& strValue)
{
	strKeyName.Trim();
	if(strKeyName.IsEmpty())
	{
		return FALSE;
	}

	FILE* fp = NULL;
	if(0 != _tfopen_s(&fp, strFilePath, _T("r")) || NULL == fp)
	{
		return FALSE;
	}

	BOOL bRet = FALSE;

	CString strBuffer;
	while(NULL != _fgetts(strBuffer.GetBuffer(1024), 1024, fp))
	{
		strBuffer.ReleaseBuffer();
		strBuffer.Trim();
		if(0 == CString(strBuffer).MakeUpper().Find(CString(strKeyName).MakeUpper()))
		{
			map<CString, CString> mapSet = CommonUtil::ParseResultToKeyValue(strBuffer, _T("\n"), _T("="), TRUE);
			map<CString, CString>::iterator it = mapSet.find(CString(strKeyName).MakeUpper());
			if(mapSet.end() != it)
			{
				strValue = it->second;
				bRet = TRUE;
				break;
			}
		}
		strBuffer.Empty();
	}

	fclose(fp);
	return bRet;
}

vector<CString> CommonUtil::CommandLineParse(CString strCommandLine)
{
	vector<CString> vecRet;

	if(!strCommandLine.IsEmpty())
	{
		int nArgc = 0;
		LPWSTR* pArgv = ::CommandLineToArgvW(CommonUtil::StringToUnicode(strCommandLine), &nArgc);
		if(NULL != pArgv)
		{
			for(int i=0 ; i<nArgc ; i++)
			{
				vecRet.push_back(CommonUtil::StringFromUnicode(pArgv[i]));
			}

			LocalFree(pArgv);
		}
	}

	return vecRet;
}

BOOL CommonUtil::MakeXMLFile(CString strFilePath, CString strXML)
{
	PVOID pOldValue = NULL;
	Wow64Disable(&pOldValue);

	FILE* fp = _tfopen((LPCTSTR)strFilePath, _T("wb"));
	if(NULL == fp)
	{
		Wow64Revert(pOldValue);
		return FALSE;
	}

	if(1 != fwrite((LPCTSTR)strXML, strXML.GetLength()*sizeof(TCHAR), 1, fp))
	{
		fclose(fp);
		::DeleteFile(strFilePath);
		Wow64Revert(pOldValue);
		return FALSE;
	}

	fflush(fp);
	fclose(fp);
	Wow64Revert(pOldValue);
	return TRUE;
}

CString CommonUtil::GetXMLFromFile(CString strFilePath)
{
	CString strXML;

	PVOID pOldValue = NULL;
	Wow64Disable(&pOldValue);

	FILE* fp = _tfopen((LPCTSTR)strFilePath, _T("rb"));
	if(NULL == fp)
	{
		Wow64Revert(pOldValue);
		return strXML;
	}

	fseek(fp, 0, SEEK_END);
	long lFileLen = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if(0 < lFileLen)
	{
		size_t nReadSize = fread(strXML.GetBuffer(lFileLen/sizeof(TCHAR)+1), lFileLen, 1, fp);
		strXML.ReleaseBuffer();
		if(1 != nReadSize)
		{
			strXML.Empty();
		}
	}

	fclose(fp);
	Wow64Revert(pOldValue);
	return strXML;
}