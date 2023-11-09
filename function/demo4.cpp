#include <iostream>
#include <functional>

std::string PrintRandy(std::string sesame)
{
    return ">>>>>>>>>> 三戒纪元： " + sesame + " <<<<<<<<<<";
}

int main(int argc, char *argv[])
{
    // 回调函数
    std::function<std::string(std::string)> RandyFuncCallBack;

    // 未包装任何对象实体
    // std::cout << RandyFuncCallBack("RandyFuncCallBack") << std::endl;
    // 结果为：Aborted (core dumped)

    RandyFuncCallBack = PrintRandy;
    std::cout << RandyFuncCallBack("RandyFuncCallBack") << std::endl;

    // 拷贝
    std::function<std::string(std::string)> CallBack_Copy(RandyFuncCallBack);
    std::cout << CallBack_Copy("CallBack_Copy") << std::endl;

    // 拷贝赋值运算符
    std::function<std::string(std::string)> CallBack_CopyAssign = RandyFuncCallBack;
    std::cout << CallBack_CopyAssign("CallBack_CopyAssign.") << std::endl;

    // 移动赋值运算符
    std::function<std::string(std::string)> &&CallBack_MoveAssign = std::move(RandyFuncCallBack);
    std::cout << CallBack_MoveAssign("CallBack_MoveAssign") << std::endl;
    std::cout << RandyFuncCallBack("RandyFuncCallBack") << std::endl;

    return 0;
}
