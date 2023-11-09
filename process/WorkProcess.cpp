#include "WorkProcess.h"

#include <new>
#include <stdio.h>

CWorkProcess::CWorkProcess(const char *szApp, const char *szArg) : m_pszCmd(NULL)
{
    ZeroMemory(&m_pi, sizeof(m_pi));
    m_pi.hProcess = NULL;
    m_pi.hThread = NULL;

    unsigned usArgLen = (szArg == NULL ? 0 : strlen(szArg));

    if (szApp != NULL)
    {
        // 命令行参数
        unsigned usCmdLen = strlen(szApp) + sizeof(' ') + usArgLen;

        // 以'\n'结束
        unsigned usBufferSize = usCmdLen + 1;

        m_pszCmd = new (std::nothrow) char[usBufferSize];
        memset(m_pszCmd, 0, usBufferSize);

        // 组装命令行 0 1 2 3
        strcpy(m_pszCmd, szApp);

        if (szArg != NULL)
        {
            m_pszCmd[strlen(szApp)] = ' ';
            strcat(m_pszCmd, szArg);
        }
    }
}

CWorkProcess::~CWorkProcess()
{
    if (m_pi.hProcess != NULL)
        CloseHandle(m_pi.hProcess);
    if (m_pi.hThread != NULL)
        CloseHandle(m_pi.hThread);

    if (m_pszCmd != NULL)
    {
        delete[] m_pszCmd;
        m_pszCmd = NULL;
    }
}

bool CWorkProcess::LaunchProcess(bool bShowWind)
{
    if (m_pszCmd == NULL || strlen(m_pszCmd) == 0)
        return false;

    // Prepare handles.
    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = bShowWind ? SW_SHOW : SW_HIDE;

    // Start the child process.
    if (!CreateProcessA(
            NULL,     // app path
            m_pszCmd, // Command line (needs to include app path as first argument. args seperated by whitepace)
            NULL,     // Process handle not inheritable
            NULL,     // Thread handle not inheritable
            FALSE,    // Set handle inheritance to FALSE
            0,        // No creation flags
            NULL,     // Use parent's environment block
            NULL,     // Use parent's starting directory
            &si,      // Pointer to STARTUPINFO structure
            &m_pi)    // Pointer to PROCESS_INFORMATION structure
    )
    {
        // std::cout << "CreateProcess failed " << GetLastError() << std::endl;
        return false;
    }

    // std::cout << "Successfully launched child process" << std::endl;
    return true;
}

bool CWorkProcess::CheckIfProcessIsActive()
{
    // Check if handle is closed
    if (m_pi.hProcess == NULL)
    {
        printf("Process handle is closed or invalid (%d).\n");
        return false;
    }

    // If handle open, check if process is active
    DWORD lpExitCode = 0;
    if (GetExitCodeProcess(m_pi.hProcess, &lpExitCode) == 0)
    {
        printf("Cannot return exit code (%d).\n", GetLastError());
        return false;
    }

    if (lpExitCode == STILL_ACTIVE)
        return true;

    return false;
}

bool CWorkProcess::StopProcess()
{
    // Check if handle is invalid or has allready been closed
    if (m_pi.hProcess == NULL)
    {
        printf("Process handle invalid. Possibly allready been closed (%d).\n");
        return false;
    }

    // Terminate Process
    // DLLs are not notified when a process is terminated by TerminateProcess.
    if (!TerminateProcess(m_pi.hProcess, 1))
    {
        printf("ExitProcess failed (%d).\n", GetLastError());
        return false;
    }

    if (m_pi.hProcess != NULL)
    {
        CloseHandle(m_pi.hProcess);
        m_pi.hProcess = NULL;
    }

    if (m_pi.hThread != NULL)
    {
        CloseHandle(m_pi.hThread);
        m_pi.hProcess = NULL;
    }

    return true;
}

bool CWorkProcess::WaitForEnded(int nTimeout)
{
    if (m_pi.hProcess == NULL)
        return false;

    if (WaitForSingleObject(m_pi.hProcess, nTimeout * 1000) != WAIT_OBJECT_0)
        return false;

    return true;
}

/**************************************************************
 *  @brief : RunSubProcess
 *
 *  @param :
 *
 *    -_TCHAR* szCmd
 *
 *    -int nTimeout
 *
 *  @return : bool
 *
 *  @author : Jimmy
 *
 *  @date : 2019/3/8 星期五
 *
 *  @note :  启动一个子进程
 ***************************************************************/
bool CWorkProcess::RunSubProcess(_TCHAR *szCmd, int nTimeout)
{
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    // Start the child process.
    if (!CreateProcess(NULL,  // No module name (use command line)
                       szCmd, // Command line 命令 + 空格 + 参数
                       NULL,  // Process handle not inheritable
                       NULL,  // Thread handle not inheritable
                       FALSE, // Set handle inheritance to FALSE
                       0,     // No creation flags
                       NULL,  // Use parent's environment block
                       NULL,  // Use parent's starting directory
                       &si,   // Pointer to STARTUPINFO structure
                       &pi)   // Pointer to PROCESS_INFORMATION structure
    )
    {
        printf("CreateProcess failed (%d).\n", GetLastError());
        return false;
    }

    bool bRet = false;
    // 等待资金池结束
    if (WaitForSingleObject(pi.hProcess, nTimeout * 1000) != WAIT_OBJECT_0)
    {
        printf("run cmd timt out...\n");
        bRet = false;
        // ExitProcess(-1);
        // The preferred way to shut down a process is by using the ExitProcess function,
        // because this function sends notification of approaching termination to all DLLs attached to the process.
        TerminateProcess(pi.hProcess, (DWORD)-1);
    }
    else
    {
        DWORD exitCode = (DWORD)-1;
        // 获取子进程的退出状态
        GetExitCodeProcess(pi.hProcess, &exitCode);
        if (exitCode == 0)
        {
            printf("exit exe ok\n");
            bRet = true;
        }
        else
        {
            printf("exit exe error\n");
            bRet = false;
        }
    }

    // 关闭系统资源
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return bRet;
}