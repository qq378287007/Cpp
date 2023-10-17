#include <windows.h>
#include <iostream>
using namespace std;

#define BUF_SIZE 4096

HANDLE h_Mypipe = NULL;

// 步骤1：定义管道名，点表示当前主机，pipe表示管道
#define MY_NAMED_PIPE TEXT("\\\\.\\pipe\\Named_Pipe")

int main(int argc, char **argv)
{

    // 步骤2：创建命名管道
    h_Mypipe = CreateNamedPipe(
        MY_NAMED_PIPE,              // 为命名管道创建名称
        PIPE_ACCESS_DUPLEX,         // 管道访问方式：PIPE_ACCESS_DUPLEX指双向模式
        PIPE_TYPE_MESSAGE |         // 命名管道句柄的写入方式：以数据块的方式写入管道
            PIPE_READMODE_MESSAGE | // 命名管道句柄的读取方式：以数据块的方式从管道读取
            PIPE_WAIT,              // 命名管道句柄的等待方式：阻塞方式
        PIPE_UNLIMITED_INSTANCES,   // 管道所能创建的最大实例个数：1~255，
        0,                          // 管道的输出缓冲区容量，0表示默认大小
        0,                          // 管道的输入缓冲区容量，0表示默认大小 1000, //管道的默认等待超时，单位毫秒
        0,
        NULL); // 管道的安全性，NULL表示windows提供的默认安全
    if (h_Mypipe == INVALID_HANDLE_VALUE)
    {
        cout << "Create Named_Pipe Failed..." << endl;
        return 1;
    }

    // 步骤3：等待客户端的连接
    if (!ConnectNamedPipe(h_Mypipe, NULL))
    {
        cout << "Connect Failed..." << endl;
        return 1;
    }
    cout << "Connect Successed..." << endl;

    DWORD wLen = 0;
    DWORD rLen = 0;
    char szBuffer[BUF_SIZE] = {0};

    // 步骤4：读写管道
    while (1)
    {
        // 向客户端发送数据
        cin.getline(szBuffer, BUF_SIZE);
        cout << "服务器端发送数据：" << szBuffer << endl;
        if (!WriteFile(h_Mypipe, szBuffer, strlen(szBuffer) + 1, &wLen, NULL))
            cout << "Write Failed..." << endl;
        else
            cout << "服务器端发送成功：共" << wLen << "byte" << endl;

        // 清除缓冲区
        // memset(szBuffer, 0, BUF_SIZE);

        // 读取客户端数据
        if (!ReadFile(h_Mypipe, szBuffer, BUF_SIZE, &rLen, NULL))
            cout << "Read Failed..." << endl;
        else
            cout << "服务器接收客户端数据：" << szBuffer << ", 共" << rLen << "byte" << endl;
    }

    // 步骤5：关闭管道
    DisconnectNamedPipe(h_Mypipe);
    CloseHandle(h_Mypipe);

    return 0;
}
