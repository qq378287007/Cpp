#include <iostream>
#include <future>
#include <thread>

int TripleX_func(int x)
{
    x = x * 3;
    std::cout << "3X thread id:" << std::endl;
    std::cout << std::this_thread::get_id() << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
    return x;
}
int main()
{
    // 将函数（某种操作后的值）打包起来
    // std::packaged_task<函数返回类型(参数类型)> 变量名(函数名)
    std::packaged_task<int(int)> pt{TripleX_func};
    // 并将结果返回给future，类型是int
    std::future<int> fu = pt.get_future();
    // future提供了一些函数比如get(),wait(),wait_for()。
    // 一般用get()来获取future所得到的结果
    // 如果异步操作还没有结束，那么会在此等待异步操作的结束，并获取返回的结果。
    std::thread t(std::ref(pt), 5);
    std::cout << fu.get() << std::endl;
    // 输出3X线程和main线程的id，可以发现是两个不同的ID。
    std::cout << "main thread id:" << std::endl;
    std::cout << std::this_thread::get_id() << std::endl;
    t.join();
    return 0;
}