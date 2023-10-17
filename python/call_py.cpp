#include "D:/soft/Python311/include/Python.h"

#include <iostream>
using namespace std;

int main()
{
    // 初始化python解释器
    Py_Initialize();
    if (!Py_IsInitialized())
    {
        cout << "python init fail" << endl;
        return 0;
    }
    // 初始化python系统文件路径，保证可以访问到 .py文件
    // PyRun_SimpleString：把python代码当作一个字符串传给解释器来执行。
    PyRun_SimpleString("import sys");
    /*把python脚本文件放入当前目录下的script文件夹下
    sys.path是一个列表 list, 它里面包含了已经添加到系统的环境变量路径。
    当我们要添加自己的引用模块搜索目录时，可以通过列表 list 的 append()方法；*/
    PyRun_SimpleString("sys.path.append('./script')");

    // PyImport_ImportModule：动态加载python模块，相当于导入python脚本文件
    // 调用python文件名。当前的测试python文件名是test.py。在使用这个函数的时候，只需要写文件的名称就可以了。不用写后缀。
    PyObject *pModule = PyImport_ImportModule("sayHello");
    if (pModule == NULL)
    {
        cout << "module not found" << endl;
        return 1;
    }

    /*PyObject* PyObject_GetAttrString(PyObject *o, char *attr_name)
     PyObject_GetAttrString()返回模块对象中的attr_name属性或函数，
     相当于Python中表达式语句：o.attr_name
     相当于找到导入的python脚本文件里边的某个函数*/
    PyObject *pFunc = PyObject_GetAttrString(pModule, "say");
    if (!pFunc || !PyCallable_Check(pFunc))
    {
        cout << "not found function add_num" << endl;
        return 0;
    }

    /*PyObject_CallObject：在C程序中调用python函数
    参数1：通过导入函数获得的函数对象
    参数2：被调用函数所需的参数*/
    PyObject_CallObject(pFunc, NULL);

    /* 撤销Py_Initialize()和随后使用Python/C API函数进行的所有初始化，
    并销毁自上次调用Py_Initialize()以来创建并为被销毁的所有子解释器。*/
    Py_Finalize();

    return 0;
}
