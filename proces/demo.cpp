#include <stdio.h>
#include <io.h>
#include <windows.h>
#include <direct.h>
#include <shellapi.h>

// #include <atlstr.h>

#include <string>
using namespace std;

string GetExePath(void)
{
    char szFilePath[MAX_PATH + 1] = {0};
    GetModuleFileNameA(NULL, szFilePath, MAX_PATH);
    (strrchr(szFilePath, '\\'))[0] = 0; // 删除文件名，只获得路径字串
    return szFilePath;
}

LPCWSTR stringToLPCWSTR(string orig)
{
    size_t origsize = orig.length() + 1;
    const size_t newsize = 100;
    size_t convertedChars = 0;
    wchar_t *wcstring = (wchar_t *)malloc(sizeof(wchar_t) * (orig.length() - 1));
    mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);

    return wcstring;
}

string WCharToMByte(LPCWSTR lpcwszStr)
{
    string str;
    DWORD dwMinSize = 0;
    LPSTR lpszStr = NULL;
    dwMinSize = WideCharToMultiByte(CP_OEMCP, NULL, lpcwszStr, -1, NULL, 0, NULL, FALSE);
    if (0 == dwMinSize)
    {
        return FALSE;
    }
    lpszStr = new char[dwMinSize];
    WideCharToMultiByte(CP_OEMCP, NULL, lpcwszStr, -1, lpszStr, dwMinSize, NULL, FALSE);
    str = lpszStr;
    delete[] lpszStr;
    return str;
}

//CONST WCHAR *
LPCWSTR char2lpcwstr(const char *szStr)
{
    WCHAR wszClassName[256];
    memset(wszClassName, 0, sizeof(wszClassName));
    MultiByteToWideChar(CP_ACP, 0, szStr, strlen(szStr) + 1, wszClassName,
                        sizeof(wszClassName) / sizeof(wszClassName[0]));
    return wszClassName;
}

/*
LPCWSTR char2lpcwstr(const char szStr)
{
    CString str = CString(szStr);
    LPCWSTR wszClassName = new WCHAR[str.GetLength() + 1];
    wcscpy((LPTSTR)wszClassName, T2W((LPTSTR)str.GetBuffer(NULL)));
    str.ReleaseBuffer();
    return wszClassName;
}
*/

/*
LPCWSTR char2lpcwstr(const char szStr)
{
    CString str = CString(szStr);
    LPCWSTR wszClassName = A2CW(W2A(str));
    str.ReleaseBuffer();
    return wszClassName;
}
*/

int main()
{
    string working_path = GetExePath();
    string exe = working_path + "/demo.exe";
    string parameter = "hello world!";

    {
        SHELLEXECUTEINFO shExecInfo = {0};
        shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
        shExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
        shExecInfo.hwnd = NULL;
        shExecInfo.lpVerb = char2lpcwstr("open");
        shExecInfo.lpFile = stringToLPCWSTR(working_path + "demo.exe");
        shExecInfo.lpParameters = char2lpcwstr("hello world");
        shExecInfo.lpDirectory = stringToLPCWSTR(working_path);
        shExecInfo.nShow = SW_SHOW;
        shExecInfo.hInstApp = NULL;
        ShellExecuteEx(&shExecInfo);
        // 这一句是等待程序结束，再结束main函数
        WaitForSingleObject(shExecInfo.hProcess, INFINITE);
    }

    return 0;
}