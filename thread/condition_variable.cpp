#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
using namespace std;

// 使用条件变量的三剑客：
/*
1.互斥元
2.条件变量
3.条件值done
*/
mutex mA;
condition_variable cv;
bool done = false;

void run1()
{
    unique_lock<mutex> lck(mA);
    cout << "wating" << endl;
    // wait内部调用unique_lock.unlock先解锁
    // 当条件满足,被唤醒后，会unique_lock.lock
    // cv.wait(lck, [] { return done; });
    while (!done)
        cv.wait(lck);
    cout << "wake up" << endl;
}

void run2()
{
    {
        lock_guard<mutex> lck(mA);
        cout << "Notify" << endl;
        this_thread::sleep_for(1s);
        done = true;
    }
    cv.notify_one();
}

int main()
{
    thread t1(run1);
    thread t2(run2);

    t1.join();
    t2.join();


    cout << "main" << endl;
    return 0;
}