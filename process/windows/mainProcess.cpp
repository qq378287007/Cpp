#include <windows.h>
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    TCHAR commandLine[] = TEXT("subProcess.exe"); // 命令行参数
    STARTUPINFO si = {sizeof(STARTUPINFO)};       // 子进程的窗口信息
    PROCESS_INFORMATION pi;                       // 子进程的ID/线程信息

    // si.dwFlags = STARTF_USESHOWWINDOW; // 指定wShowWindow成员有效
    // si.wShowWindow = TRUE;             // 显示新建进程的主窗口

    BOOL bRet = CreateProcess(
        NULL,        // LPCSTR lpApplicationName 可执行文件名
        commandLine, // LPSTR lpCommandLine 命令行参数
        NULL,        // LPSECURITY_ATTRIBUTES lpProcessAttributes 进程安全性
        NULL,        // LPSECURITY_ATTRIBUTES lpThreadAttributes 线程安全性
        FALSE,       // BOOL bInheritHandles 当前进程的可继承句柄是否被新进程继承
        0,           // DWORD dwCreationFlags 新进程的优先级以及其他创建标志, CREATE_NEW_CONSOLE
        NULL,        // LPVOID lpEnvironment 环境变量
        NULL,        // LPCSTR lpCurrentDirectory 当前目录
        &si,         // LPSTARTUPINFO lpStartupInfo 新进程中主窗口的位置、大小和标准句柄等
        &pi);        // LPPROCESS_INFORMATION lpProcessInformation【out】新建进程的ID号、句柄等标志信息
    if (!bRet)
    {
        cout << "Create Process error!\n";
        cout << "Error code: " << GetLastError() << endl;
        return 1;
    }

    cout << "Work in MainProcess!\n";

    // 等待子进程结束
    WaitForSingleObject(pi.hProcess, -1);
    cout << "subProcess is finished.\n";

    // 子进程的退出码
    DWORD returnCode;
    GetExitCodeProcess(pi.hProcess, &returnCode);
    cout << "subProcess return code: " << returnCode << endl;

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    cout << "process id: " << pi.dwProcessId << endl;
    cout << "thread id in new process: " << pi.dwThreadId << endl;

    // 终止子进程
    //TerminateProcess(pi.hProcess, 300);

    // 终止本进程，状态码
    //ExitProcess(1001);

    return 0;
}