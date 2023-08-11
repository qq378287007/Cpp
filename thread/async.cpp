#include <future>
#include <iostream>
#include <thread>
#include <chrono>
using namespace std;

int theFinalAnswer()
{
    cout << __func__ << " is running.\n";
    this_thread::sleep_for(chrono::seconds(1));
    cout << "answer is ready.\n";
    return 42;
}

int main()
{
    future<int> lazyAns = async(launch::async, theFinalAnswer);
    this_thread::sleep_for(chrono::milliseconds(100));
    cout << lazyAns.get() << endl;
    return 0;
}