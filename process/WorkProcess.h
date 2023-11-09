#pragma once

#include <windows.h>
#include <tchar.h>

class CWorkProcess
{
public:
    explicit CWorkProcess(const char *szApp, const char *arg = NULL);
    ~CWorkProcess();

    bool LaunchProcess(bool bShowWind = false);

    bool CheckIfProcessIsActive();

    bool StopProcess();

    bool WaitForEnded(int nTimeout);

    static bool RunSubProcess(_TCHAR *szCmd, int nTimeout);

private:
    PROCESS_INFORMATION m_pi;
    char *m_pszCmd;
};