#include "Server.h"

void Server::createShm(const string &mapName, size_t size)
{
    // 调用 CreateFileMapping 创建一个内存文件映射对象
    m_hMap = ::CreateFileMapping(
        INVALID_HANDLE_VALUE, // handle to file to map
        NULL,                 // optional security attributes
        PAGE_READWRITE,       // protection for mapping object
        0,                    // high-order 32 bits of object size
        size,                 // low-order 32 bits of object size
        mapName.c_str());     // name of file-mapping object

    if (m_hMap == nullptr) // 创建失败
        throw;

    m_mapSize = size;
}

void Server::appendRcvEvent(const string &eventType, const _FUNC &func)
{
    m_handlers.emplace(eventType, func);
}

void Server::eventLoop(const string &eventName)
{
    string rcvEventName = eventName + "ctos";

    HANDLE hRcvEvent = CreateEvent(NULL, FALSE, FALSE, rcvEventName.c_str());
    if (hRcvEvent == nullptr) // 错误处理
        exit(1);

    while (true)
    {
        cout << "等待命令中..." << endl;
        WaitForSingleObject(hRcvEvent, INFINITE); // 收到信号，自动重置

        // 解包
        LPVOID pBuffer = ::MapViewOfFile(m_hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        Header header;
        memmove_s(&header, sizeof(header), pBuffer, sizeof(header));

        // 处理数据
        _FUNC func = m_handlers.at(header.type);
        func((PBYTE)pBuffer + sizeof(header), header.size);

        // 重新封包
        memset(pBuffer, 0, sizeof(header));
        memmove((PBYTE)pBuffer, &header, sizeof(header));

        string sndEventName = eventName + "stoc";
        HANDLE hsndEvent = OpenEvent(EVENT_ALL_ACCESS, NULL, sndEventName.c_str());
        if (hsndEvent == nullptr) // 错误处理
            exit(1);

        SetEvent(hsndEvent);
    }
}
