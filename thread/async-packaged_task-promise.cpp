#include <chrono>
#include <future>
#include <iostream>
#include <thread>
using namespace std;

int main()
{
    packaged_task<int()> task(
        []
        {
            chrono::milliseconds dura(2000);
            this_thread::sleep_for(dura);
            return 0;
        });

    future<int> f1 = task.get_future();
    thread(move(task)).detach();

    future<int> f2 = async(launch::async,
                           []()
                           {
                               chrono::milliseconds dura(2000);
                               this_thread::sleep_for(dura);
                               return 0;
                           });

    promise<int> p;
    future<int> f3 = p.get_future();
    thread(
        [](promise<int> p)
        {
            chrono::milliseconds dura(2000);
            this_thread::sleep_for(dura);
            p.set_value(0);
        },
        move(p))
        .detach();

    cout << "Waiting..." << flush;
    f1.wait();
    f2.wait();
    f3.wait();
    cout << "Done!\nResults are: " << f1.get() << " " << f2.get() << " " << f3.get() << "\n";
    
    return 0;
}
