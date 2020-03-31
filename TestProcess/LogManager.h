#pragma once

#define LOG_MANAGER						CLogManager::GetInstance()

#define LOG_TYPE_I						CLogManager::I
#define LOG_TYPE_E						CLogManager::E
#define LOG_TYPE_D						CLogManager::D

#define LOG(type, format, ...)			LOG_MANAGER->Log(type, format, ##__VA_ARGS__)
#define LOG_I(format, ...)				LOG_MANAGER->Log_i(format, ##__VA_ARGS__)
#define LOG_E(format, ...)				LOG_MANAGER->Log_e(format, ##__VA_ARGS__)
#define LOG_D(format, ...)				LOG_MANAGER->Log_d(format, ##__VA_ARGS__)

#define LOG_ENABLE(file, console)		LOG_MANAGER->Enable(file, console)
#define LOG_PATH(path)					LOG_MANAGER->SetLogFilePath(path)

class CLogManager
{
public:
	enum LOG_TYPE
	{
		I,
		E,
		D
	};

public:
	CLogManager(void);
	~CLogManager(void);

	static CLogManager* GetInstance();
	void SetLogFilePath(CString strPath);
	void Enable(BOOL bEnableFile, BOOL bEnableConsole);
	void Log(LOG_TYPE type, LPCTSTR pszFormat, ...);
	void Log_i(LPCTSTR pszFormat, ...);
	void Log_e(LPCTSTR pszFormat, ...);
	void Log_d(LPCTSTR pszFormat, ...);

private:
	void Log(LOG_TYPE type, LPCTSTR pszFormat, va_list list);

private:
	CString m_strPath;
	BOOL m_bEnableFile;
	BOOL m_bEnableConsole;

	CRITICAL_SECTION m_csLock;
};

