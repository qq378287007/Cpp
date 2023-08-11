#include <atomic>
#include <iostream>
#include <thread>
using namespace std;

atomic_int total{0};
void func()
{
    for (int i = 0; i < 100000; i++)
    {
        total += 1;
    }
}
int main()
{
    thread th1(func);
    thread th2(func);

    th1.join();
    th2.join();
    cout << "total:" << total << endl;
    return 0;
}