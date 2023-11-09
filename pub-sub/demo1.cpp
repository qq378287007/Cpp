#include <iostream>
#include <string>
#include <vector>
#include <functional>
using namespace std;

class Publisher
{
public:
    void publish(const string &message)
    {
        for (const function<void(const string &)> &callback : subscribers)
            callback(message);
    }

    void subscribe(function<void(const string &)> callback)
    {
        subscribers.push_back(callback);
    }

private:
    vector<function<void(const string &)>> subscribers;
};

int main()
{
    Publisher publisher;

    publisher.subscribe([](const string &message)
                        { cout << "Subscriber 1 received: " << message << '\n'; });

    publisher.subscribe([](const string &message)
                        { cout << "Subscriber 2 received: " << message << '\n'; });

    publisher.publish("Hello, World!");
    
    return 0;
}
