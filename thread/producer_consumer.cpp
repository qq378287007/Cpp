#include <chrono>
#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <iostream>
using namespace std;

mutex mtx;
condition_variable conv;
queue<int> msgQueue;

void producer(int start, int end)
{
    for (int x = start; x < end; x++)
    {
        this_thread::sleep_for(chrono::milliseconds(200));
        {
            lock_guard<mutex> guard(mtx);
            msgQueue.push(x);
            cout << "Produce message " << x << endl;
        }
        conv.notify_all();
    }
}

void consumer(int demand)
{
    for (int i = 0; i < demand; i++)
    {
        unique_lock<mutex> lck(mtx);
        // conv.wait(lck, []{ return msgQueue.size() > 0; });
        while (msgQueue.size() <= 0)
            conv.wait(lck);

        cout << "Consume message " << msgQueue.front() << endl;
        msgQueue.pop();
    }
}

int main()
{
    thread producer1(producer, 0, 10);
    thread producer2(producer, 10, 20);
    thread producer3(producer, 20, 30);
    thread consumer1(consumer, 20);
    thread consumer2(consumer, 10);

    producer1.join();
    producer2.join();
    producer3.join();
    consumer1.join();
    consumer2.join();

    return 0;
}
