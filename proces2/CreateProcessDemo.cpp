#include <windows.h>
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    TCHAR commandLine1[] = TEXT("subapp.exe -l"); // �����в���
    STARTUPINFO si = {sizeof(STARTUPINFO)};       // �ӽ��̵Ĵ�����Ϣ
    PROCESS_INFORMATION pi;                       // �ӽ��̵�ID/�߳���Ϣ

    BOOL bRet = CreateProcess(
        NULL,         // һ�㶼�ǿգ�����һ��������������˲���ָ��"cmd.exe",��һ�������в��� "/c otherBatFile")
        commandLine1, // �����в���
        NULL,         //_In_opt_    LPSECURITY_ATTRIBUTES lpProcessAttributes,
        NULL,         //_In_opt_    LPSECURITY_ATTRIBUTES lpThreadAttributes,
        FALSE,        //_In_        BOOL                  bInheritHandles,
        0,            // �ӽ���ʹ���´���
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

    // �ȴ��ӽ��̽���
    WaitForSingleObject(pi.hProcess, -1);
    cout << "process is finished\n";

    // �ӽ��̵��˳���
    DWORD returnCode;
    GetExitCodeProcess(pi.hProcess, &returnCode);
    cout << "process return code: " << returnCode << endl;

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    return 0;
}