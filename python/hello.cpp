#include "D:/soft/Python311/include/Python.h"

int main()
{
    Py_Initialize();    // 初始化
    PyRun_SimpleString("print('hello')");
    Py_Finalize();      //释放资源
    return 0;
}

// g++ hello.cpp -o hello -LD:/soft/Python311/libs -lpython311 && hello.exe
