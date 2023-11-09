//EventEmitter.hpp
#include <iostream>
#include <stdexcept>
#include <functional>
#include <typeinfo>
#include <string>
#include <map>
using namespace std;

class EventEmitter
{
    map<string, void *> events;
    map<string, bool> events_once;

    template <typename Callback>
    struct traits : public traits<decltype(&Callback::operator())>
    {
    };

    template <typename ClassType, typename R, typename... Args>
    struct traits<R (ClassType::*)(Args...) const>
    {

        typedeffunction<R(Args...)> fn;
    };

    template <typename Callback>
    typename traits<Callback>::fn
    to_function(Callback &cb)
    {

        return static_cast<typename traits<Callback>::fn>(cb);
    }

    int _listeners = 0;

public:
    int maxListeners = 10;

    int listeners()
    {
        return this->_listeners;
    }

    template <typename Callback>
    void on(const string &name, Callback cb)
    {

        auto it = events.find(name);
        if (it != events.end())
        {
            throw newruntime_error("duplicate listener");
        }

        if (++this->_listeners >= this->maxListeners)
        {
            cout
                << "warning: possible EventEmitter memory leak detected. "
                << this->_listeners
                << " listeners added. "
                << endl;
        };

        auto f = to_function(cb);
        auto fn = new decltype(f)(to_function(cb));
        events[name] = static_cast<void *>(fn);
    }

    template <typename Callback>
    void once(const string &name, Callback cb)
    {
        this->on(name, cb);
        events_once[name] = true;
    }

    void off()
    {
        events.clear();
        events_once.clear();
        this->_listeners = 0;
    }

    void off(const string &name)
    {

        auto it = events.find(name);

        if (it != events.end())
        {
            events.erase(it);
            this->_listeners--;

            auto once = events_once.find(name);
            if (once != events_once.end())
            {
                events_once.erase(once);
            }
        }
    }

    template <typename... Args>
    void emit(string name, Args... args)
    {

        auto it = events.find(name);
        if (it != events.end())
        {

            auto cb = events.at(name);
            auto fp = static_cast<function<void(Args...)> *>(cb);
            (*fp)(args...);
        }

        auto once = events_once.find(name);
        if (once != events_once.end())
        {
            this->off(name);
        }
    }

    EventEmitter(void) {}

    ~EventEmitter(void)
    {
        events.clear();
    }
};

#include <assert.h>

#define ASSERT(message, ...)                     \
    do                                           \
    {                                            \
        if (!(__VA_ARGS__))                      \
        {                                        \
            cerr << "FAIL: " << message << endl; \
            exit(0);                             \
        }                                        \
        else                                     \
        {                                        \
            cout << "OK: " << message << endl;   \
            testcount++;                         \
        }                                        \
    } while (0);

int main()
{
    int testcount = 0;
    int testplan = 9;

    //
    // sanity test.
    //
    ASSERT("sanity: true is true", true == true);

    EventEmitter ee;

    ASSERT("maxListeners should be 10", ee.maxListeners == 10);

    ee.on("event1", [&](int a, string &b)
          {
    ASSERT("first arg should be equal to 10", a == 10);
    ASSERT("second arg should be foo", b == "foo"); });

    ee.emit("event1", 10, (string) "foo");

    try
    {
        ee.on("event1", []() {});
    }
    catch (...)
    {
        ASSERT("duplicate listener", true);
    }

    ee.on("event2", [&]()
          { ASSERT("no params", true); });

    ee.emit("event2");

    int count1 = 0;
    ee.on("event3", [&]()
          {
    if (++count1 == 10) {
      ASSERT("event was emitted correct number of times", true);
    } });

    for (int i = 0; i < 10; i++)
    {
        ee.emit("event3");
    }

    int count2 = 0;
    ee.on("event4", [&]()
          {
    count2++;
    if (count2 > 1) {
      ASSERT("event was fired after it was removed", false);
    } });

    ee.emit("event4");
    ee.off("event4");
    ee.emit("event4");
    ee.emit("event4");

    int count3 = 0;
    ee.on("event5", [&]()
          {
    count3++;
    ASSERT("events were fired even though all listeners were removed", false); });

    ASSERT("the correct number of listeners has been reported", ee.listeners() == 4);

    ee.off();

    ASSERT("no additional events fired after all listeners removed",
           count1 == 10 &&
               count2 == 1 &&
               count3 == 0);

    ASSERT("testcount == plan", testcount == testplan)
    
    return 0;
}
