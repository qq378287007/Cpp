#pragma once

#include <memory>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
using namespace std;

template <typename T>
class CThreadSafeQueue
{
private:
    mutex head_mutex;
    mutex tail_mutex;
    condition_variable data_cond;
    queue<T> _queue;

public:
    CThreadSafeQueue(){};

    CThreadSafeQueue(const CThreadSafeQueue &other) = delete;
    CThreadSafeQueue &operator=(const CThreadSafeQueue &other) = delete;

    shared_ptr<T> try_pop()
    {
        lock_guard<mutex> lc(head_mutex);
        if (_queue.empty())
            return shared_ptr<T>();
        auto pos = shared_ptr<T>(move(_queue.front()));
        _queue.pop();
    }

    bool try_pop(T &value)
    {
        lock_guard<mutex> lc(head_mutex);
        if (_queue.empty())
            return false;
        value = move(_queue.front());
        _queue.pop();
        return true;
    }

    shared_ptr<T> wait_and_pop()
    {
        unique_lock<mutex> lc(head_mutex);
        // data_cond.wait(lc, [&]{ return !empty(); });
        while (empty())
            data_cond.wait(lc);

        if (_queue.empty())
            return shared_ptr<T>(NULL);
        shared_ptr<T> sptr(new T(move(_queue.front())));
        _queue.pop();
        return sptr;
    }

    void wait_and_pop(T &value)
    {
        unique_lock<mutex> lc(head_mutex);
        // data_cond.wait(lc, [&]{ return !empty(); });
        while (empty())
            data_cond.wait(lc);

        if (_queue.empty())
            return shared_ptr<T>();
        value = move(_queue.front());
        _queue.pop();
    }

    void push(T new_value)
    {
        lock_guard<mutex> lc(tail_mutex);
        _queue.push(new_value);
        data_cond.notify_one();
    }

    bool empty()
    {
        lock_guard<mutex> lc(tail_mutex);
        return _queue.empty();
    }

    void stop()
    {
        data_cond.notify_all();
    }
};