#include <windows.h>
#include <iostream>
using namespace std;

int main(int argc, char **argv)
{
    // 步骤1：打开共享文件句柄
    HANDLE shared_file = OpenFileMapping(
        FILE_MAP_ALL_ACCESS, // 访问模式:可读写
        FALSE,
        TEXT("ShareMemory") // 共享内存名称
    );
    if (shared_file == NULL)
    {
        cout << "Could not open file mapping object..." << endl;
        return 1;
    }

    // 步骤2：映射缓存区视图，得到指向共享内存的指针
    LPVOID lpBUF = MapViewOfFile(
        shared_file,         // 已创建的文件映射对象句柄
        FILE_MAP_ALL_ACCESS, // 访问模式:可读写
        0,                   // 文件偏移的高32位
        0,                   // 文件偏移的低32位
        0                    // 映射视图的大小,0表示从偏移量到文件映射的末尾，因为共享文件open端不知道其大小，所以写0
    );
    if (lpBUF == NULL)
    {
        cout << "Could not create file mapping object..." << endl;
        CloseHandle(shared_file);
        return 1;
    }

    HANDLE H_Mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, TEXT("sm_mutex"));
    if (H_Mutex == NULL)
    {
        cout << "open mutex failed..." << endl;
        return 1;
    }

    HANDLE H_Event = OpenEvent(EVENT_ALL_ACCESS, FALSE, TEXT("sm_event"));
    if (H_Event == NULL)
    {
        cout << "open mutex failed..." << endl;
        return 1;
    }

    // 步骤3：操作共享内存
    char Buffer[97];
    while (1)
    {
        cout << "B proccess: Receive data from process A" << endl;
        WaitForSingleObject(H_Event, INFINITE);

        WaitForSingleObject(H_Mutex, INFINITE); // 使用互斥体加锁
        memcpy(Buffer, lpBUF, strlen((char *)lpBUF) + 1);
        ReleaseMutex(H_Mutex); // 放锁
        cout << Buffer << endl;
    }

    CloseHandle(H_Event);
    CloseHandle(H_Mutex);

    // 步骤4：解除映射和关闭句柄
    UnmapViewOfFile(lpBUF);
    CloseHandle(shared_file);

    return 0;
}
