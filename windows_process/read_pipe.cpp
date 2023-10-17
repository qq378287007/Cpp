#include <Windows.h>

#include <iostream>

using namespace std;

#define BUF_SIZE 4096

HANDLE h_Mypipe = NULL;

// 步骤1：定义管道名，点表示当前主机，pipe表示管道

#define MY_NAMED_PIPE TEXT("\\\\.\\pipe\\Named_Pipe")

int main(int argc, char **argv)
{
    // 步骤2：判断是否有可用的命名管道
    // 函数WaitNamedPipe：等待某个管道变成可用状态
    // 形参1:表示命名管道的名称
    // 形参2:NMPWAIT_USE_DEFAULT_WAIT使用管道创建时的默认超时设定;NMPWAIT_WAIT_FOREVER永远等待
    if (!WaitNamedPipe(MY_NAMED_PIPE, NMPWAIT_USE_DEFAULT_WAIT))
    {
        cout << "No Named_Pipe Accessible..." << endl;
        return 1;
    }
    cout << "Named_Pipe Accessible..." << endl;

    // 步骤3：打开指定命名管道
    // 函数CreateFile：创建或打开对象（这里对象指的是管道）
    h_Mypipe = CreateFile(
        MY_NAMED_PIPE,                // 创建或打开的对象(管道)名称
        GENERIC_READ | GENERIC_WRITE, // 对象的访问方式：读访问|写访问
        0,                            // 对象是否共享：0表示不共享
        NULL,                         // 指向一个SECURITY_ATTRIBUTES结构的指针
        OPEN_EXISTING,                // 对象的创建方式：OPEN_EXISTING表示打开对象(管道)
        FILE_ATTRIBUTE_NORMAL,        // 设置对象的属性和标志
        NULL);
    if (h_Mypipe == INVALID_HANDLE_VALUE)
    {
        cout << "Open Named_Pipe Failed..." << endl;
        return 1;
    }

    DWORD wLen = 0;
    DWORD rLen = 0;
    char szBuffer[BUF_SIZE] = {0};

    // 步骤4：读写管道
    while (1)
    {
        // 读取服务器端数据
        if (!ReadFile(h_Mypipe, szBuffer, BUF_SIZE, &rLen, NULL))
            cout << "Read Failed..." << endl;
        else
            cout << "客户端接收服务器端数据：" << szBuffer << ", 共" << rLen << "byte" << endl;

        // 清除缓冲区
        // memset(szBuffer, 0, BUF_SIZE);

        // 客户端发送数据
        cin.getline(szBuffer, BUF_SIZE);
        cout << "客户端发送数据：" << szBuffer << endl;

        if (!WriteFile(h_Mypipe, szBuffer, strlen(szBuffer) + 1, &wLen, NULL))
            cout << "Write Failed..." << endl;
        else
            cout << "客户端发送成功：共" << wLen << "byte" << endl;
    }

    // 步骤5：关闭管道
    CloseHandle(h_Mypipe);

    return 0;
}
