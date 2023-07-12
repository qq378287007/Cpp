#include <thread>
#include <condition_variable>
#include <mutex>
#include <iostream>
#include <vector>
using namespace std;

class Barrier
{
public:
    explicit Barrier(size_t iCount) : mThreshold(iCount), mCount(iCount), mGeneration(0) {}

    void Wait()
    {
        unique_lock<mutex> lck{mtx};
        auto lGen = mGeneration;
        if (!--mCount)
        {
            mGeneration++;
            mCount = mThreshold;
            cond.notify_all();
        }
        else
        {
            cond.wait(lck, [this, lGen]
                      { return lGen != mGeneration; });
        }
    }

private:
    size_t mThreshold;
    size_t mCount;
    size_t mGeneration;

    mutex mtx;
    condition_variable cond;
};

class abc
{
    size_t n_threads;

public:
    abc() : n_threads(0){};

    void num_threads(size_t l) { n_threads = l; }

    Barrier task_bar{n_threads};

    bool abc_write(auto id)
    {
        thread_local int wr_count = 0;
        if (wr_count == 1)
        {
            cout << "write thread waiting" << id << endl;
            task_bar.Wait();
            wr_count = 0;
        };
        cout << "write thread running " << id << endl;
        ++wr_count;
        return true;
    }

    bool abc_read(auto id)
    {
        thread_local int rd_count = 0;
        if (rd_count == 1)
        {
            cout << "read thread waiting" << id << endl;
            task_bar.Wait();
            rd_count = 0;
        };
        cout << "read thread running " << id << endl;
        ++rd_count;
        return true;
    }
};

int main()
{
    abc obj1;
    obj1.num_threads(4);
    
    thread t1(
        [&obj1]()
        {
            for (int i = 0; i < 5; i++)
            {
                while (!obj1.abc_write(1))
                    ;
            };
        });
    thread t2(
        [&obj1]()
        {
            for (int i = 0; i < 5; i++)
            {
                while (!obj1.abc_read(2))
                    ;
            };
        });
    thread t3(
        [&obj1]()
        {
            for (int i = 0; i < 5; i++)
            {
                while (!obj1.abc_write(3))
                    ;
            };
        });
    thread t4(
        [&obj1]()
        {
            for (int i = 0; i < 5; i++)
            {
                while (!obj1.abc_read(4))
                    ;
            };
        });
    t1.join();
    t2.join();
    t3.join();
    t4.join();

    // cout << "done: " << obj1.done << endl;
    // cout << "done: " << obj2.done << endl;
    // cout << "wr_count: " << obj1.wr_count << endl;
    return 0;
}