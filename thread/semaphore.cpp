#include <mutex>
#include <condition_variable>
#include <thread>
using namespace std;

class Semaphore
{
    mutex mtx;
    condition_variable conv;
    int available;

public:
    explicit Semaphore(int init = 0) : available(init) {}
    void post()
    {
        {
            lock_guard<mutex> lck(mtx);
            ++available;
        }
        conv.notify_one();
    }
    void wait()
    {
        unique_lock<mutex> lck(mtx);
        while (available <= 0)
            conv.wait(lck);
        --available;
    }
};

int main()
{
    Semaphore s(-2);
    thread t([&s]{
        s.post();
        s.post();
        s.post();
    });

    t.join();

    s.wait();

    return 0;
}