#pragma once

#include <windows.h>
#include <vector>
#include <functional>
#include <iostream>
#include <tuple>
using namespace std;

// 每次传递数据的包头
struct Header
{
	char type[20]; // 调用的功能类型
	UINT64 size;   // 数据大小
};

class Client
{
	typedef function<void(PBYTE pPayload, UINT64 size)> _EVENT; // 可以注册的事件类型

public:
	Client() : m_hMap(NULL), m_mapSize(0){}
	Client(const Client &) = delete;
	~Client() {}

	void init(const string &mapName, size_t size, const string &eventName);
	void send(const string &eventType, LPVOID pdata, size_t dataSize, const _EVENT &callback);

private:
	HANDLE m_hMap;
	size_t m_mapSize;
	string m_EventName;
	// HANDLE m_hSendEvent;
};
