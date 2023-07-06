#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

#include <iostream>
#include <chrono>
using namespace std;

class ThreadPool
{
public:
    ThreadPool(size_t threads = thread::hardware_concurrency())
        : stop(false)
    {
        for (size_t i = 0; i < threads; ++i)
            workers.emplace_back(
                [this]
                {
                    while (true)
                    {
                        function<void()> task;
                        {
                            unique_lock<mutex> lck(mtx);
                            // conv.wait(lck, [this]{ return stop || !tasks.empty(); });
                            while (!stop && tasks.empty())
                                conv.wait(lck);
                            if (stop && tasks.empty())
                                return;
                            task = move(tasks.front());
                            tasks.pop();
                        }
                        task();
                    }
                });
    }

    ~ThreadPool()
    {
        {
            unique_lock<mutex> lck(mtx);
            stop = true;
        }
        conv.notify_all();
        for (thread &worker : workers)
            worker.join();
    }

    template <class F, class... Args>
    auto enqueue(F &&f, Args &&...args) -> future<typename result_of<F(Args...)>::type>
    {
        using return_type = typename result_of<F(Args...)>::type;

        auto task = make_shared<packaged_task<return_type()>>(bind(forward<F>(f), forward<Args>(args)...));

        future<return_type> res = task->get_future();
        {
            unique_lock<mutex> lck(mtx);

            if (stop)
                throw runtime_error("enqueue on stopped ThreadPool");

            tasks.emplace(
                [task]()
                {
                    (*task)();
                });
        }
        conv.notify_one();
        return res;
    }

private:
    vector<thread> workers;
    queue<function<void()>> tasks;

    mutex mtx;
    condition_variable conv;
    bool stop;
};

int main()
{
    ThreadPool pool(4);
    vector<future<int>> results;

    for (int i = 0; i < 8; ++i)
        results.emplace_back(
            pool.enqueue(
                [i]
                {
                    cout << "hello " << i << endl;
                    this_thread::sleep_for(chrono::seconds(1));
                    cout << "world " << i << endl;
                    return i * i;
                }));

    for (auto &&result : results)
        cout << result.get() << ' ';
    cout << endl;

    // pool中可放入多种可调用函数
    auto out = pool.enqueue(
        []
        {
            return string("123");
        });
    cout << "out: " << out.get() << endl;

    return 0;
}
