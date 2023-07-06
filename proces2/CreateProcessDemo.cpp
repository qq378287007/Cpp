#include <windows.h>
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    TCHAR commandLine1[] = TEXT("subapp.exe -l"); // 命令行参数
    STARTUPINFO si = {sizeof(STARTUPINFO)};       // 子进程的窗口信息
    PROCESS_INFORMATION pi;                       // 子进程的ID/线程信息

    BOOL bRet = CreateProcess(
        NULL,         // 一般都是空；（另一种批处理情况：此参数指定"cmd.exe",下一个命令行参数 "/c otherBatFile")
        commandLine1, // 命令行参数
        NULL,         //_In_opt_    LPSECURITY_ATTRIBUTES lpProcessAttributes,
        NULL,         //_In_opt_    LPSECURITY_ATTRIBUTES lpThreadAttributes,
        FALSE,        //_In_        BOOL                  bInheritHandles,
        0,            // 子进程使用新窗口
        NULL,         //_In_opt_    LPVOID                lpEnvironment,
        NULL,         //_In_opt_    LPCTSTR               lpCurrentDirectory,
        &si,          //_In_        LPSTARTUPINFO         lpStartupInfo,
        &pi);         //_Out_       LPPROCESS_INFORMATION lpProcessInformation
    if (!bRet)
    {
        cout << "Create Process error!\n";
        cout << "Error code: " << GetLastError() << endl;
        return 1;
    }

    cout << "process is running...\n";

    // 等待子进程结束
    WaitForSingleObject(pi.hProcess, -1);
    cout << "process is finished\n";

    // 子进程的退出码
    DWORD returnCode;
    GetExitCodeProcess(pi.hProcess, &returnCode);
    cout << "process return code: " << returnCode << endl;

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    return 0;
}