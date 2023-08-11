#include <iostream>
#include <mutex>
#include <thread>
using namespace std;

mutex m;
void run(int n)
{
    for (int i = 0; i < 5; ++i)
    {
        m.lock();
        cout << "thread" << n << std::endl;
        m.unlock();
    }
}

int main()
{
    thread t1(run, 1);
    thread t2(run, 2);
    t1.join();
    t2.join();
    return 0;
}