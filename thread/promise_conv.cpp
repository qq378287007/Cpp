#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <tuple>
#include <future>
using namespace std;

bool is_prime(int x) { return true; }

mutex mtx;
condition_variable cv;
deque<tuple<promise<bool>, int>> reqs;

future<bool> request(int x)
{
    promise<bool> prm;
    future<bool> res = prm.get_future();
    {
        lock_guard<mutex> lck(mtx);
        reqs.emplace_back(move(prm), x);
    }
    cv.notify_one();
    return res;
}

void prime_service()
{
    while (true)
    {
        unique_lock<mutex> lck(mtx);
        while (reqs.empty())
            cv.wait(lck);

        promise<bool> res;
        int n = 0;
        tie(res, n) = move(reqs.front());
        reqs.pop_front();

        lck.unlock();

        if (n == 0)
            break;
        res.set_value(is_prime(n));
    }
}

int main()
{
    thread t(prime_service);

    int n0 = 12345;
    future<bool> r0 = request(n0);
    int n1 = 1255345;
    future<bool> r1 = request(n1);

    cout << n0 << ":" << boolalpha << r0.get() << endl;
    cout << n1 << ":" << boolalpha << r1.get() << endl;

    request(0);
    t.join();

    return 0;
}