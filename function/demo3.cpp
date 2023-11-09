#include <iostream>
#include <functional>

// 类成员函数
struct Randy
{
    std::string RandyCommonFunc(std::string sesame)
    {
        return "Randy common function: " + sesame;
    }

    static std::string RandyStaticFunc(std::string t_arg)
    {
        return "Randy static function: " + t_arg;
    }
};

// 模板类
template <typename T>
struct RandyTemplateStruct
{
    // 模板对象函数
    T RandyTemplateFunc(T sesame)
    {
        return "Randy Template function: " + sesame;
    }

    // 模板对象静态函数
    static T RandyTemplateStaticFunc(T t_arg)
    {
        return "Randy Template static function: " + t_arg;
    }
};

int main(int argc, char *argv[])
{
    // 回调函数
    std::function<std::string(std::string)> RandyFuncCallBack;

    Randy randy;

    // 类成员函数
    RandyFuncCallBack = std::bind(&Randy::RandyCommonFunc, &randy, std::placeholders::_1);
    std::cout << RandyFuncCallBack("RandyCommonFunc called.") << std::endl;

    // 类成员函数
    RandyFuncCallBack = randy.RandyStaticFunc;
    std::cout << RandyFuncCallBack("RandyStaticFunc called.") << std::endl;

    // 模板对象函数
    RandyTemplateStruct<std::string> randyTemplate;
    RandyFuncCallBack = std::bind(&RandyTemplateStruct<std::string>::RandyTemplateFunc, &randyTemplate, std::placeholders::_1);
    std::cout << RandyFuncCallBack("RandyTemplateFunc called.") << std::endl;

    // 模板对象静态函数
    RandyFuncCallBack = RandyTemplateStruct<std::string>::RandyTemplateStaticFunc;
    std::cout << RandyFuncCallBack("RandyTemplateStaticFunc called.") << std::endl;
    return 0;
}
