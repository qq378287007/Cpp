#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <cassert>

#include <iostream>
#include <chrono>
using namespace std;

class ThreadPool
{
public:
    explicit ThreadPool(size_t threads = thread::hardware_concurrency())
        : stop(false)
    {
        for (size_t i = 0; i < threads; ++i)
        {
            workers.emplace_back(
                [this]()
                {
                    while (true)
                    {
                        function<void()> task;

                        {
                            unique_lock<mutex> lck(queue_mutex);
                            // condition.wait(lck, [this](){ return stop || !tasks.empty(); });
                            while (!stop && tasks.empty())
                                condition.wait(lck);
                            if (stop && tasks.empty())
                                return;
                            task = move(tasks.front());
                            tasks.pop();
                        }

                        task(); // 执行任务
                    }
                }); // lambda表达式构建
        }
    }

    template <typename F, typename... Args>
    auto enqueue(F &&f, Args &&...args) -> future<typename result_of<F(Args...)>::type>
    {
        using return_type = invoke_result_t<F, Args...>;
        auto task = make_shared<packaged_task<return_type()>>(bind(forward<F>(f), forward<Args>(args)...));
        future<return_type> res = task->get_future();  
        {
            unique_lock<mutex> lck(queue_mutex);

            if (stop)
                throw runtime_error("enqueue on stopped Thread pool");

            tasks.emplace(
                [task = move(task)]()
                {
                    (*task)();
                });
        }
        condition.notify_one();
        return res;
    }

    ~ThreadPool()
    {
        {
            unique_lock<mutex> lck(queue_mutex);
            stop = true;
        }
        condition.notify_all(); // 唤醒所有线程，清理任务
        for (thread &worker : workers)
            worker.join(); // 阻塞，等待所有线程执行结束
    }

private:
    vector<thread> workers;
    queue<function<void()>> tasks;
    mutex queue_mutex;
    condition_variable condition;
    bool stop;
};

// lock_guard，一个
// scoped_lock是lock_guard的升级版，多个（C++17，优先）
// unique_lock，condition_variable需要，可提前解锁
template <typename T>
class SafeQueue
{
public:
    void push(const T &item)
    {
        {
            scoped_lock lck(mtx);
            q.push(item);
        }
        conv.notify_one();
    }
    void push(T &&item)
    {
        {
            scoped_lock lck(mtx);
            q.push(move(item));
            // q.emplace(item);//与上面等价
        }
        conv.notify_one();
    }
    /*
        template <typename U>
        void push(U &&item)
        {
            {
                static_assert(is_same<U, T>::value == true);
                scoped_lock lck(mtx);
                q.push(forward(item));
            }
            conv.notify_one();
        }
    */
    bool pop(T &item)
    {
        unique_lock lck(mtx);
        // conv.wait(lck, [&](){ return !q.empty() || stop_flag; });
        while (!stop_flag && q.empty())
            conv.wait(lck);

        if (q.empty())
            return false;
        item = move(q.front());
        q.pop();
        return true;
    }
    size_t size() const
    {
        scoped_lock lck(mtx);
        return q.size();
    }
    bool empty() const
    {
        scoped_lock lck(mtx);
        return q.empty();
    }
    void stop()
    {
        {
            scoped_lock lck(mtx);
            stop_flag = true;
        }
        conv.notify_all();
    }

private:
    condition_variable conv;
    mutable mutex mtx;
    queue<T> q;
    bool stop_flag = false;
};

class SimplePool
{
public:
    explicit SimplePool(size_t threads = thread::hardware_concurrency())
    {
        for (size_t i = 0; i < threads; ++i)
        {
            workers.emplace_back(
                [this]()
                {
                    while (true)
                    {
                        function<void()> task;
                        if (!q.pop(task))
                            return;

                        if (task)
                            task();
                    }
                });
        }
    }

    ~SimplePool()
    {
        q.stop();
        for (auto &thd : workers)
            thd.join();
    }

    void enqueue(function<void()> item)
    {
        q.push(move(item));
    }

private:
    SafeQueue<function<void()>> q;
    vector<thread> workers;
};

class MultiplePool
{
public:
    explicit MultiplePool(size_t thread_num = thread::hardware_concurrency())
        : queues_(thread_num), thread_num_(thread_num)
    {
        auto worker = [this](size_t id)
        {
            while (true)
            {
                function<void()> task;
                if (!queues_[id].pop(task))
                    break;
                if (task)
                    task();
            }
        };
        workers_.reserve(thread_num_);
        for (size_t i = 0; i < thread_num_; ++i)
            workers_.emplace_back(worker, i);
    }

    ~MultiplePool()
    {
        for (auto &queue : queues_)
            queue.stop(); // 停止每一个任务队列

        for (auto &worker : workers_)
            worker.join(); // 阻塞，等待每个线程执行结束
    }

    int schedule_by_id(function<void()> fn, size_t id = -1)
    {
        if (fn == nullptr)
            return -1;
        if (id == -1)
            id = rand() % thread_num_; // 随机插入到一个线程的任务队列中
        else
            assert(id < thread_num_); // 插入指定线程的任务队列
        queues_[id].push(move(fn));
        return 0;
    }

private:
    vector<SafeQueue<function<void()>>> queues_; // 每个线程对应一个任务队列
    size_t thread_num_;
    vector<thread> workers_;
};

void test_thread_pool()
{
    cout << "test_thread_pool()" << endl;
    ThreadPool threadPool;
    threadPool.enqueue([]
                       { cout << "hello\n"; });
    auto future = threadPool.enqueue([](string str)
                                     { return "hello" + str; },
                                     "world");
    cout << future.get() << endl;
}

string funA(string str)
{
    return "hello" + str;
}

void test_simple_thread_pool()
{
    cout << "test_simple_thread_pool()" << endl;
    SimplePool threadPool;
    threadPool.enqueue([]
                       { cout << "hello\n"; });
    // * 此处必须使用shared_ptr进行包装，
    // * 否则在std::function<void()>中会尝试生成std::packaged_task的拷贝构造函数，
    // ! packaged_task禁止拷贝操作
    auto task = make_shared<packaged_task<string()>>(bind(funA, "world"));
    future<string> res = task->get_future();
    threadPool.enqueue([task = move(task)]
                       { (*task)(); });
    // ! 以下实现方法是错误的
    //  auto task = packaged_task<string()>(bind(funA, "world"));
    //  future<string> res = task.get_future();
    //  threadPool.enqueue(move(task));
    cout << res.get() << endl;
}

void test_multiple_thread_pool()
{
    cout << "test_multiple_thread_pool" << endl;
    MultiplePool threadPool;
    threadPool.schedule_by_id([]
                              { cout << "hello\n"; });
    auto task = make_shared<packaged_task<string()>>(bind(funA, "world"));
    future<string> res = task->get_future();
    threadPool.schedule_by_id([task = move(task)]
                              { (*task)(); });
    cout << res.get() << endl;
}

int main()
{
    SimplePool sPool(4);
    for (int i = 0; i < 8; ++i)
        sPool.enqueue(
            [i]()
            {
                cout << "hello " << i << endl;
                this_thread::sleep_for(chrono::seconds(1));
                cout << "world " << i << endl;
            });

    MultiplePool mPool(4);
    for (int i = 0; i < 8; ++i)
        mPool.schedule_by_id(
            [i]()
            {
                cout << "hello " << i << endl;
                this_thread::sleep_for(chrono::seconds(1));
                cout << "world " << i << endl;
            });

    return 0;
}

// g++ threadPool2.cpp -o threadPool2 -std=c++17 && threadPool2.exe
