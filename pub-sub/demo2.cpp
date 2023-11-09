#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <future>
using namespace std;

class Publisher
{
public:
    ~Publisher()
    {
        for (const future<void> &f : vf)
            f.wait();
    }

    void publish(const string &message)
    {
        for (const function<void(const string &)> &callback : subscribers)
            vf.emplace_back(async(launch::async, callback, message));
    }

    void subscribe(function<void(const string &)> callback)
    {
        subscribers.push_back(callback);
    }

private:
    vector<function<void(const string &)>> subscribers;
    vector<future<void>> vf;
};

int main()
{
    {
        Publisher publisher;

        publisher.subscribe([](const string &message)
                            { cout << "Subscriber 1 received: " << message << '\n'; });

        publisher.subscribe([](const string &message)
                            { cout << "Subscriber 2 received: " << message << '\n'; });

        publisher.publish("Hello, World!");
    }
    cout << "Main Over!\n";

    return 0;
}
