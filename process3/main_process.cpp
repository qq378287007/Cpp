#include <windows.h>

#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    wchar_t commandLine[] = L"sub_process.exe"; 
    STARTUPINFO si = {sizeof(STARTUPINFO)}; // 在产生子进程时，子进程的窗口相关信息
    PROCESS_INFORMATION pi;                 // 子进程的ID/线程相关信息

    BOOL bRet = CreateProcess( // 调用失败，返回0；调用成功返回非0；
        NULL,                  // 一般都是空；（另一种批处理情况：此参数指定"cmd.exe",下一个命令行参数 "/c otherBatFile")
        //commandLine,          // 命令行参数
        TEXT("sub_process.exe"),
        NULL,                  //_In_opt_    LPSECURITY_ATTRIBUTES lpProcessAttributes,
        NULL,                  //_In_opt_    LPSECURITY_ATTRIBUTES lpThreadAttributes,
        FALSE,                 //_In_        BOOL                  bInheritHandles,
        CREATE_NEW_CONSOLE,    // 新的进程使用新的窗口。
        NULL,                  //_In_opt_    LPVOID                lpEnvironment,
        NULL,                  //_In_opt_    LPCTSTR               lpCurrentDirectory,
        &si,                   //_In_        LPSTARTUPINFO         lpStartupInfo,
        &pi);                  //_Out_       LPPROCESS_INFORMATION lpProcessInformation
    if (!bRet)
    {
        cout << "Create Process error!" << endl;
        return 1;
    }

    cout << "process is running..." << endl;

    // 等待子进程结束
    WaitForSingleObject(pi.hProcess, -1);
    cout << "process is finished" << endl;

    // 获取子进程的返回值
    DWORD returnCode;
    GetExitCodeProcess(pi.hProcess, &returnCode);
    cout << "process return code: " << returnCode << endl;

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return 0;
}
