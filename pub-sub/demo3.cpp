#include <stdio.h>
#include <pthread.h>
#include <string>
#include <list>
#include <map>
using namespace std;

/* 定义事件与对应的数据类型 */
char *event_data = "MuttonPrice";
struct EventData
{
    int e = 0;
};

char *event_data1 = "MuttonPrice1";
struct EventData1
{
    char *i = "receive";
};

char *event_data2 = "MuttonPrice2";
struct EventData2
{
    EventData data;
    EventData1 data1;
};

/*订阅事件处理模块 3个 */
class Subscriber
{
public:
    static void HandleReceiveEvent(void *_this, void *data)
    {
        Subscriber *th = (Subscriber *)_this;
        EventData *d = (EventData *)data;
        d->e += th->_data;
        printf("%s, data:%d \n", __FUNCTION__, d->e);
    }
    int _data = 10;
};

class Subscriber1
{
public:
    static void HandleReceiveEvent(void *_this, void *data)
    {
        Subscriber1 *th = (Subscriber1 *)_this;
        EventData1 *d = (EventData1 *)data;
        string str(d->i);
        str += th->_data;
        printf("%s, data:%s \n", __FUNCTION__, str.c_str());
    }
    char *_data = " Subscriber 1";
};

class Subscriber2
{
public:
    static void HandleReceiveEvent(void *_this, void *data)
    {
        Subscriber2 *th = (Subscriber2 *)_this;
        EventData2 *d = (EventData2 *)data;
        d->data.e += th->_data;

        string str(d->data1.i);
        str += th->_data2;

        printf("%s, data:%d , %s \n", __FUNCTION__, d->data.e, str.c_str());
    }
    int _data = 30;
    char *_data2 = " Subscriber 1";
};

/*事件处理中心*/
class EventMsgCentre
{
public:
    typedef void (*HandleEvent)(void *, void *);
    /* 订阅事件节点 */
    struct EventSubscriberNode
    {
        void *_this;
        HandleEvent func;
    };

    /* 发布事件节点 */
    struct EventPublishNode
    {
        char *event;
        void *data;
    };

public:
    EventMsgCentre()
    {
        pthread_mutex_init(&_mutexSubscriber, nullptr);
        pthread_mutex_init(&_mutexPublish, nullptr);
    }

    void SubscriberEvent(char *event, EventSubscriberNode node)
    {
        pthread_mutex_lock(&_mutexSubscriber);
        sMap[event].push_back(node);
        pthread_mutex_unlock(&_mutexSubscriber);
    }

    void releaseSubscriberEvent(char *event, EventSubscriberNode node)
    {
        pthread_mutex_lock(&_mutexSubscriber);
        auto it = sMap.find(event);
        if (it != sMap.end())
        {
            for (auto ite = it->second.begin(); ite != it->second.end();)
            {
                if (ite->_this == node._this && ite->func == node.func)
                {
                    ite = it->second.erase(ite);
                }
                else
                {
                    ++ite;
                }
            }
        }
        pthread_mutex_unlock(&_mutexSubscriber);
    }

    void PublishEvent(char *event, void *data)
    {
        EventPublishNode node;
        node.event = event;
        node.data = data;

        pthread_mutex_lock(&_mutexPublish);
        plist.push_back(node);
        pthread_mutex_unlock(&_mutexPublish);
    }

    /* 事件处理中心  */
    static void *EventProcess(void *_this)
    {
        EventMsgCentre *th = (EventMsgCentre *)_this;
        while (1)
        {
            EventPublishNode cur{nullptr, nullptr};
            pthread_mutex_lock(&th->_mutexPublish);
            if (th->plist.empty())
            {
                pthread_mutex_unlock(&th->_mutexPublish);
                continue;
            }
            cur = th->plist.front();
            th->plist.pop_front();
            pthread_mutex_unlock(&th->_mutexPublish);

            pthread_mutex_lock(&th->_mutexSubscriber);
            auto it = th->sMap.find(cur.event);
            if (it != th->sMap.end())
            {
                for (auto ite = it->second.begin(); ite != it->second.end(); ++ite)
                {
                    ite->func(ite->_this, cur.data);
                }
            }
            pthread_mutex_unlock(&th->_mutexSubscriber);
        }
    }

    void theardProc()
    {
        int ret = pthread_create(&pt_id, nullptr, EventProcess, this);
    }

public:
    pthread_mutex_t _mutexSubscriber;
    pthread_mutex_t _mutexPublish;
    pthread_t pt_id;
    list<EventPublishNode> plist;
    map<char *, list<EventSubscriberNode>> sMap;
};

/* 测试。 */
int main()
{
    EventMsgCentre eveMsg; /* 初始化 */
    eveMsg.theardProc();   /* 启线程 */

    /* 订阅事件 */
    Subscriber ber;
    ber._data = 100;
    EventMsgCentre::EventSubscriberNode node;
    node._this = &ber;
    node.func = Subscriber::HandleReceiveEvent;
    eveMsg.SubscriberEvent(event_data, node);

    Subscriber1 ber1;
    ber1._data = " Subscriber1_100";
    EventMsgCentre::EventSubscriberNode node1;
    node1._this = &ber1;
    node1.func = Subscriber1::HandleReceiveEvent;
    eveMsg.SubscriberEvent(event_data1, node1);

    Subscriber2 ber2;
    ber2._data = 200;
    ber2._data2 = " Subscriber2_200";
    EventMsgCentre::EventSubscriberNode node2;
    node2._this = &ber2;
    node2.func = Subscriber2::HandleReceiveEvent;
    eveMsg.SubscriberEvent(event_data2, node2);

    /* 发布事件 */
    EventData d{9};
    eveMsg.PublishEvent(event_data, &d);

    EventData1 d1{"event_data1"};
    eveMsg.PublishEvent(event_data1, &d1);

    EventData2 d2{2, "event_data"};
    eveMsg.PublishEvent(event_data2, &d2);

    while (1)
    {
    }

    return 1;
}