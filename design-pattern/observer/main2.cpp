#include <iostream>
#include <vector>
#include <memory>
using namespace std;

// 订阅者
class Subscriber
{
public:
    virtual ~Subscriber() {}
    virtual void update(string msg) = 0;
};

class Mobile : public Subscriber
{
public:
    void update(string msg) override
    {
        cout << "Mobile receive: " << msg << endl;
    }
};

class QQ : public Subscriber
{
public:
    void update(string msg) override
    {
        cout << "QQ receive: " << msg << endl;
    }
};

// 发布者
class Publisher
{
public:
    void attach(shared_ptr<Subscriber> sub)
    {
        vp.push_back(sub);
    }
    void deatch(shared_ptr<Subscriber> sub)
    {
        // vp.remove(sub);
        // vp.erase(sub);
    }
    void notify(string msg)
    {
        for (shared_ptr<Subscriber> sub : vp)
            sub->update(msg);
    }

private:
    vector<shared_ptr<Subscriber>> vp;
};

int main()
{
    Publisher pub;
    pub.attach(make_shared<Mobile>());
    pub.attach(make_shared<QQ>());
    pub.notify("update");
    return 0;
}
