#include <iostream>
#include <functional>

template <typename T>
T RandyTemplate(T t_arg)
{
    return "三戒纪元: " + t_arg;
}

// 模板函数对象
template <typename T>
struct RandyStruct
{
    T operator()(T t_arg)
    {
        return "Randy struct: " + t_arg;
    }
};

int main(int argc, char *argv[])
{
    // 回调函数
    std::function<std::string(std::string)> RandyFuncCallBack;

    // 函数指针
    std::string (*RandyFuncPtr)(std::string);

    // std::function包装模板函数
    RandyFuncCallBack = RandyTemplate<std::string>;
    std::cout << RandyFuncCallBack("std::function called.") << std::endl;

    // 函数指针
    RandyFuncPtr = RandyTemplate;
    RandyFuncCallBack = RandyFuncPtr;
    std::cout << RandyFuncCallBack("function ptr called.") << std::endl;

    // 模板函数对象
    RandyFuncCallBack = RandyStruct<std::string>();
    std::cout << RandyFuncCallBack("struct function called.") << std::endl;
    return 0;
}
