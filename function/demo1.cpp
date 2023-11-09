#include <iostream>
#include <functional>

// 普通函数
int commonFunc(int lhs, int rhs)
{
    return lhs + rhs;
}

// Lambda表达式
auto lamdaFunc = [](int lhs, int rhs)
{
    return lhs + rhs;
};

// 函数对象 换成 struct 亦可
class Functor
{
public:
    int operator()(int lhs, int rhs)
    {
        return lhs + rhs;
    }
};

// 成员函数
class Randy
{
public:
    // 1.成员函数
    int RandyMemberFunc(int lhs, int rhs) { return lhs + rhs; }
    // 2.静态成员函数
    static int RandyStaticFunc(int lhs, int rhs) { return lhs + rhs; }
};

int main()
{
    // 定义一个 std::function 函数
    std::function<int(int, int)> qcj_CallBack;
    int result = -1;

    // 普通函数
    qcj_CallBack = commonFunc;
    result = qcj_CallBack(2, 1);
    std::cout << "common function: " << result << std::endl;

    // 普通函数指针
    qcj_CallBack = &commonFunc;
    result = qcj_CallBack(2, 2);
    std::cout << "common function pointer: " << result << std::endl;

    // Lambda表达式
    qcj_CallBack = lamdaFunc;
    result = qcj_CallBack(2, 3);
    std::cout << "lambda function: " << result << std::endl;

    // 函数对象
    Functor functor_randy;
    qcj_CallBack = functor_randy;
    result = qcj_CallBack(2, 4);
    std::cout << "function object: " << result << std::endl;

    // 类成员函数（使用std::bind绑定类成员函数, std::placeholders::_2为占位符，顺序与原函数入参对应，次序可调换）
    // qcj_CallBack = std::bind(&Randy::RandyMemberFunc, randy, std::placeholders::_2, std::placeholders::_1);
    // 上式这么写也可以，调用时 第2个入参在前，第1个入参在后
    Randy randy;
    qcj_CallBack = std::bind(&Randy::RandyMemberFunc, randy, std::placeholders::_1, std::placeholders::_2);
    result = qcj_CallBack(2, 5);
    std::cout << "class member function: " << result << std::endl;

    // 类静态成员函数
    qcj_CallBack = Randy::RandyStaticFunc;
    result = qcj_CallBack(2, 6);
    std::cout << "class static member function: " << result << std::endl;

    return 0;
}
