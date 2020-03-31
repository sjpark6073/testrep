#pragma once

#include <vector>
#include <list>
#include <map>

using namespace std;

class CommonUtil
{
public:
    CommonUtil(void);
public:
    ~CommonUtil(void);

	static CString GetSystemPath();
	static CString GetSystemDrive();
	static CString GetUserTempPath();
    static CString GetWindowsPath();
	static CString GetLocalPath(CString* pLocalFileName = NULL, CString* pLocalFilePath = NULL);

    static BOOL Is64BitWindows();
    static BOOL GetRegValue(HKEY plKey, CString szSubekey, CString szValueName, DWORD szDefualt, DWORD& pszRetVal);    
    static BOOL GetRegValue32Or64(HKEY plKey, CString szSubekey, CString szValueName, DWORD szDefualt, DWORD& pszRetVal, int nFlag);
	static BOOL GetRegQValue(HKEY plKey, CString szSubekey, CString szValueName, QWORD szDefualt, QWORD& pszRetVal);
	static BOOL GetRegQValue32Or64(HKEY plKey, CString szSubekey, CString szValueName, QWORD szDefualt, QWORD& pszRetVal, int nFlag);
    static BOOL GetRegString(HKEY plKey, CString szSubekey, CString szValueName, CString szDefualt, CString& pszRetVal);
    static BOOL GetRegString32Or64(HKEY plKey, CString szSubekey, CString szValueName, CString szDefualt, CString& pszRetVal, int nFlag);
	static BOOL GetRegMultiString(HKEY plKey, CString szSubekey, CString szValueName, vector<CString>& pRetVal);
	static BOOL GetRegMultiString32Or64(HKEY plKey, CString szSubekey, CString szValueName, vector<CString>& pRetVal, int nFlag);
	static BOOL GetReg(HKEY plKey, CString szSubekey, CString szValueName, REGSAM samDesired, vector<BYTE>& pRetVal, DWORD& dwValueType);
	static BOOL GetReg(HKEY plKey, CString szSubekey, CString szValueName, REGSAM samDesired, vector<BYTE>& pRetVal, DWORD& dwValueType, CString* pstrCmdResult);
	static BOOL GetReg(HKEY plKey, CString szSubekey, CString szValueName, vector<BYTE>& pRetVal, DWORD& dwValueType, CString* pstrCmdResult = NULL);
	static BOOL GetReg32Or64(HKEY plKey, CString szSubekey, CString szValueName, vector<BYTE>& pRetVal, DWORD& dwValueType, int nFlag, CString* pstrCmdResult = NULL);
	static BOOL GetRegDWord(HKEY plKey, CString szSubekey, CString szValueName, DWORD szDefualt, DWORD& pszRetVal, CString* pstrCmdResult = NULL);
	static BOOL GetRegDWord32Or64(HKEY plKey, CString szSubekey, CString szValueName, DWORD szDefualt, DWORD& pszRetVal, int nFlag, CString* pstrCmdResult = NULL);
	static BOOL GetRegQWord(HKEY plKey, CString szSubekey, CString szValueName, QWORD szDefualt, QWORD& pszRetVal, CString* pstrCmdResult = NULL);
	static BOOL GetRegQWord32Or64(HKEY plKey, CString szSubekey, CString szValueName, QWORD szDefualt, QWORD& pszRetVal, int nFlag, CString* pstrCmdResult = NULL);
	static BOOL GetRegSZ(HKEY plKey, CString szSubekey, CString szValueName, CString szDefualt, CString& pszRetVal, CString* pstrCmdResult = NULL);
	static BOOL GetRegSZ32Or64(HKEY plKey, CString szSubekey, CString szValueName, CString szDefualt, CString& pszRetVal, int nFlag, CString* pstrCmdResult = NULL);
	static BOOL GetRegExpandSZ(HKEY plKey, CString szSubekey, CString szValueName, CString szDefualt, CString& pszRetVal, CString* pstrCmdResult = NULL);
	static BOOL GetRegExpandSZ32Or64(HKEY plKey, CString szSubekey, CString szValueName, CString szDefualt, CString& pszRetVal, int nFlag, CString* pstrCmdResult = NULL);
	static BOOL GetRegMultiSZ(HKEY plKey, CString szSubekey, CString szValueName, vector<CString>& pRetVal, CString* pstrCmdResult = NULL);
	static BOOL GetRegMultiSZ32Or64(HKEY plKey, CString szSubekey, CString szValueName, vector<CString>& pRetVal, int nFlag, CString* pstrCmdResult = NULL);
	static BOOL GetRegBinary(HKEY plKey, CString szSubekey, CString szValueName, vector<BYTE>& pRetVal, CString* pstrCmdResult = NULL);
	static BOOL GetRegBinary32Or64(HKEY plKey, CString szSubekey, CString szValueName, vector<BYTE>& pRetVal, int nFlag, CString* pstrCmdResult = NULL);

    static std::list<CString> GetEnumValueList(HKEY plKey, CString szSubekey);
    static std::list<CString> GetEnumValueList32Or64(HKEY plKey, CString szSubekey, int nFlag);
    static std::list<CString> GetEnumKeyList(HKEY plKey, CString szSubekey);
    static std::list<CString> GetEnumKeyList32Or64(HKEY plKey, CString szSubekey, int nFlag);

	static BOOL SetRegString(HKEY plKey, CString szSubekey, CString szValueName, CString szValue);
	static BOOL SetRegString32Or64(HKEY plKey, CString szSubekey, CString szValueName, CString szValue, int nFlag);
	static BOOL SetRegValue(HKEY plKey, CString szSubekey, CString szValueName, DWORD dwValue);
	static BOOL SetRegValue32Or64(HKEY plKey, CString szSubekey, CString szValueName, DWORD dwValue, int nFlag);

	static BOOL DeleteRegString(HKEY plKey, CString szSubekey, CString szValueName);
	static BOOL DeleteRegString32Or64(HKEY plKey, CString szSubekey, CString szValueName, int nFlag);

	static CString GetFileName(CString strFilePath);
	static CString GetParentDirPath(CString strFilePath);
	static void SplitPath(CString strFilePath, CString* pstrDrive, CString* pstrDir, CString* pstrFilename, CString* pstrExt);
    
	static BOOL GetOSVersion(OSVERSIONINFOEX* os);
	static BOOL GetOSVersion(DWORD* pMajorVersion, DWORD* pMinorVersion, BYTE* pProductType, CString* pOSName, CString* pOSString);
	static CString GetOSVersionString(DWORD dwMajorVersion, DWORD dwMinorVersion, DWORD dwBuildNumber, BYTE wProductType);
    static CString GetComName();            // 컴퓨터 이름
	static CString GetLoginUserName2();            // 계정 이름2

	//////////////////////////////////////////////////////////////////////////
	// String 관련 변환 함수
	static CString StringFromUTF8(CStringA strUTF8);
	static CString StringFromAnsi(CStringA strAnsi);
	static CString StringFromOEM(CStringA strOEM);
	static CString StringFromUnicode(CStringW strUnicode);

	static CStringA StringToUTF8(CString str);
	static CStringA StringToAnsi(CString str);
	static CStringW StringToUnicode(CString str);
	static CStringA StringToOEM(CString str);

	static CStringA UnicodeToAnsi(CStringW strUnicode);
	static CStringW AnsiToUnicode(CStringA strAnsi);
	static CStringA UnicodeToUTF8(CStringW strUnicode);
	static CStringW UTF8ToUnicode(CStringA strUTF8);
	static CStringA UnicodeToOEM(CStringW strUnicode);
	static CStringW OEMToUnicode(CStringA strOEM);
	static CStringA AnsiToUTF8(CStringA strAnsi);
	static CStringA UTF8ToAnsi(CStringA strUTF8);
	static CStringA OEMToUTF8(CStringA strAnsi);
	static CStringA UTF8ToOEM(CStringA strUTF8);
	static CStringA OEMToAnsi(CStringA strOEM);
	static CStringA AnsiToOEM(CStringA strAnsi);
	
	// String 관련 변환 함수 끝
	//////////////////////////////////////////////////////////////////////////
	// cmd 관련 함수
	static CString ExecuteCmd(CString strCommand, DWORD dwTimeout = 60000);
	static CString ExecuteCmd2(CString strCommand, BOOL bChangeCP = TRUE, DWORD dwTimeout = 60000, int nRetry = 3);
	static CString ExecuteCmd3(CString strCommand, CString* pstrError = NULL, DWORD dwTimeout = 20000);
	static std::vector<CString> ParseResultToList(CString strResult, CString strDelimi, BOOL bEmptyDelete = FALSE);
	static std::map<CString, CString> ParseResultToKeyValue(CString strResult, CString strMainDelimi, CString strSubDelimi, BOOL IsKeyMakeUpper);
	static std::vector<std::map<CString, CString>> ParseResultToTable(CString strResult, CString strMainDelimi, CString strSubDelimi, BOOL IsHeadMakeUpper);
	// cmd 관련 함수 끝
	//////////////////////////////////////////////////////////////////////////
	// 숫자, 문자 변환 관련 함수
	static CString CommonUtil::IntToStr(int nNum);
	static CString CommonUtil::UIntToStr(unsigned int nNum);
	static CString CommonUtil::Int64ToStr(__int64 nNum);
	static CString CommonUtil::UInt64ToStr(unsigned __int64 nNum);
	// 숫자, 문자 변환 관련 함수 끝
	//////////////////////////////////////////////////////////////////////////
	// 프로세스 및 디렉토리 관련 함수
	static BOOL CreateProcessOnLoginSession(CString strProcess, WORD wShowWindow, LPPROCESS_INFORMATION lpProcessInformation);
	static BOOL CreateProcessOnLoginSession(CString strProcess, WORD wShowWindow, BOOL bWait = FALSE);

	static BOOL CreateProcessOnLoginSessionUACElevation(CString strProcess, WORD wShowWindow, LPPROCESS_INFORMATION lpProcessInformation);
	static BOOL CreateProcessOnLoginSessionUACElevation(CString strProcess, WORD wShowWindow, BOOL bWait = FALSE);

	static BOOL CreateProcessOnCurrentSession(CString strProcess, WORD wShowWindow, LPPROCESS_INFORMATION lpProcessInformation);
	static BOOL CreateProcessOnCurrentSession(CString strProcess, WORD wShowWindow, BOOL bWait = FALSE);

	static BOOL CreateDirectoryEx(CString strFolderPath);
	static BOOL DeleteDirectory(CString strDirPath);

	static BOOL SetPrivilegesToCurrentThread();
	static BOOL TerminateProcessEx(DWORD dwProcessId, UINT uExitCode);
	// 프로세스 및 디렉토리 관련 함수 끝
	//////////////////////////////////////////////////////////////////////////
	// Wow64 Redirection
	static BOOL Wow64Disable(PVOID* ppOldValue);
	static BOOL Wow64Revert(PVOID pOldValue);

	//////////////////////////////////////////////////////////////////////////
	// Mutex 관련 함수
	static HANDLE HaveMutex(CString strMutexName, BOOL bExistFail = TRUE);
	static BOOL CheckMutex(CString strMutexName);
	// Mutex 관련 함수 끝
	//////////////////////////////////////////////////////////////////////////
	// bat 및 cmd 파일에서 set에 의한 변수명 가져오기
	static BOOL GetVarOfSetFromFile(CString strFilePath, CString strVarName, CString& strValue);
	static BOOL GetVarOfKeyFromFile(CString strFilePath, CString strKeyName, CString& strValue);
	//////////////////////////////////////////////////////////////////////////
	// Command Line Parser
	static vector<CString> CommandLineParse(CString strCommandLine);
	//////////////////////////////////////////////////////////////////////////
	// XML File 관련
	static BOOL MakeXMLFile(CString strFilePath, CString strXML);
	static CString GetXMLFromFile(CString strFilePath);
};
