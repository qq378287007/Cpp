#include <iostream>
using namespace std;

class IDownloadSink
{
public:
    virtual void OnDownloadFinished(const char *pURL, bool bOK) = 0;
};

class CMyDownloader
{
public:
    CMyDownloader(IDownloadSink *pSink) : m_pSink(pSink) {}

    void DownloadFile(const char *pURL)
    {
        cout << "downloading..." << pURL << "" << endl;
        if (m_pSink != NULL)
            m_pSink->OnDownloadFinished(pURL, true);
    }

private:
    IDownloadSink *m_pSink;
};

class CMyFile : public IDownloadSink
{
public:
    void download()
    {
        CMyDownloader downloader(this);
        downloader.DownloadFile("www.baidu.com");
    }

    virtual void OnDownloadFinished(const char *pURL, bool bOK)
    {
        cout << "onDownloadFinished..." << pURL << "   status:" << bOK << endl;
    }
};

int main()
{
    CMyFile file;
    file.download();

    return 0;
}