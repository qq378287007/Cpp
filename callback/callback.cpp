#include <iostream>
using namespace std;

// typedef void(__stdcall *DownloadCallback)(const char *pURL, bool OK);
typedef void (*DownloadCallback)(const char *pURL, bool OK);

void DownLoadFile(const char *pURL, DownloadCallback callback)
{

    cout << "downloading..." << pURL << "" << endl;
    callback(pURL, true);
}

// void __stdcall onDownloadFinished(const char *pURL, bool bOK)
void onDownloadFinished(const char *pURL, bool bOK)
{
    cout << "onDownloadFinished..." << pURL << "   status: " << bOK << endl;
}

int main()
{
    DownLoadFile("http://wwww.baidu.com", onDownloadFinished);

    return 0;
}