#include <iostream>
#include <thread>
using namespace std;

void run(int n)
{
    for (int i = 0; i < 5; ++i)
        cout << "thread" << n << endl;
}

int main()
{
    thread t1(run, 1);
    thread t2(run, 2);
    t1.join();
    t2.join();
    return 0;
}