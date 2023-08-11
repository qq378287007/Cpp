#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
using namespace std;

mutex m;
void run()
{
    unique_lock<mutex> lck(m, try_to_lock);
    cout << (lck.owns_lock() ? '*' : 'x');
}

int main()
{
    vector<thread> threads;
    for (int i = 0; i < 50; ++i)
        threads.emplace_back(run);
        
    for (auto &t : threads)
        t.join();

    return 0;
}