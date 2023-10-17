#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
using namespace std;

#define FileMapping_NAME "Xidian"
#define FILESIZE 4096

int main()
{
    // 创建一个有名字标识的共享内存。
    HANDLE hmap = CreateFileMappingA(INVALID_HANDLE_VALUE,
                                     NULL,
                                     PAGE_READWRITE | SEC_COMMIT,
                                     0,
                                     FILESIZE,
                                     FileMapping_NAME);
    if (hmap == NULL) // 如果句柄指向NULL，表示创建失败
    {
        cerr << "Create shared memory failed" << endl;
        return 1;
    }

    // 映射文件到指针
    LPVOID lpdata = MapViewOfFile(hmap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
    if (lpdata == NULL) // 映射失败
    {
        cerr << "Mapping failed!" << endl;
    }
    else
    {
        char s[] = "Hello Xidian!";
        memcpy(lpdata, s, sizeof(s)); // 向这个内存中写入数据
    }

    system("pause");

    UnmapViewOfFile(lpdata); // 解除映射
    CloseHandle(hmap);

    return 0;
}
