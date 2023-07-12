#include <future>
#include <thread>
#include <iostream>
using namespace std;

int main()
{
    packaged_task<int(int, int)> task(
        [](int a, int b)
        {
            return a * b;
        });
    future<int> f = task.get_future();
    thread t(move(task), 3, 5);
    cout << f.get() << endl;
    t.join();
    return 0;
}