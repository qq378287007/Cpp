#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

class CDownloadDelegateBase
{
public:
    virtual void Fire(const char *pURL, bool bOK) = 0;
};

template <typename O, typename T>
class CDownloadDelegate : public CDownloadDelegateBase
{
    typedef void (T::*Fun)(const char *, bool);

public:
    CDownloadDelegate(O *pObj = NULL, Fun pFun = NULL) : m_pObj(pObj), m_pFun(pFun) {}

    // virtual
    void Fire(const char *pURL, bool bOK) override
    {
        if (m_pFun != NULL && m_pObj != NULL)
            (m_pObj->*m_pFun)(pURL, bOK);
    }

private:
    O *m_pObj;
    Fun m_pFun;
};

template <typename O, typename T>
CDownloadDelegate<O, T> *MakeDelegate(O *pObject, void (T::*pFun)(const char *pURL, bool))
{
    return new CDownloadDelegate<O, T>(pObject, pFun);
}

class CDownloadEvent
{
    typedef vector<CDownloadDelegateBase *>::iterator ITR;

public:
    ~CDownloadEvent()
    {
        for (ITR itr = m_arDelegates.begin(); itr != m_arDelegates.end(); ++itr)
            delete *itr;
        m_arDelegates.clear();
    }

    void operator+=(CDownloadDelegateBase *p)
    {
        m_arDelegates.push_back(p);
    }

    void operator-=(CDownloadDelegateBase *p)
    {
        //等于p的值都remove到后面，返回新的end()值
        ITR itr = remove(m_arDelegates.begin(), m_arDelegates.end(), p);

        ITR itrTemp = itr;
        while (itrTemp != m_arDelegates.end())
        {
            delete *itr;
            ++itr;
        }
        
        //删除掉某个位置position或一段区域（begin, end)中的元素
        m_arDelegates.erase(itr, m_arDelegates.end());
    }

    void operator()(const char *pURL, bool bOK)
    {
        for (ITR itrTemp = m_arDelegates.begin(); itrTemp != m_arDelegates.end(); ++itrTemp)
            (*itrTemp)->Fire(pURL, bOK);
    }

private:
    vector<CDownloadDelegateBase *> m_arDelegates;
};

class CMyDownloaderEx
{
public:
    void DownloadFile(const char *pURL)
    {
        cout << "downloading: " << pURL << "" << endl;
        downloadEvent(pURL, true);
    }

    CDownloadEvent downloadEvent;
};

class CMyFileEx
{
public:
    void download()
    {
        CMyDownloaderEx downloader;
        downloader.downloadEvent += MakeDelegate(this, &CMyFileEx::OnDownloadFinished);
        downloader.DownloadFile("www.baidu.com");
    }

    // virtual
    void OnDownloadFinished(const char *pURL, bool bOK)
    {
        cout << "OnDownloadFinished, URL:" << pURL << "    status:" << bOK << endl;
    }
};

int main()
{
    CMyFileEx ff;
    ff.download();

    return 0;
}