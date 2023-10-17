#pragma once

#include <windows.h>
#include <string>
#include <map>
#include <iostream>
#include <functional>
using namespace std;

// 每次传递数据的包头
struct Header
{
    char type[20]; // 调用的功能类型
    UINT64 size;   // 数据大小
};

class Server
{
    typedef function<void(PBYTE pPayload, UINT64 &size)> _FUNC; // 可以注册的事件类型
    typedef tuple<string, HANDLE, _FUNC> _NODE;                 // 每个事件节点

    struct Event
    {
        string type;
        _FUNC func;
    };

public:
    Server() : m_hMap(NULL), m_mapSize(0) {}
    Server(const Server &) = delete;
    ~Server() { CloseHandle(m_hMap); }

    void createShm(const string &mapName, size_t size);
    void appendRcvEvent(const string &eventName, const _FUNC &func);
    void eventLoop(const string &eventName);

private:
    HANDLE m_hMap;
    size_t m_mapSize;
    string m_eventName;
    map<string, _FUNC> m_handlers;
};