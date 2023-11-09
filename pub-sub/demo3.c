#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

/* 定义事件与对应的数据类型 */
char *event_data = "MuttonPrice";
typedef struct
{
    int e; // = 0;
} EventData;
char *event_data1 = "MuttonPrice1";
typedef struct
{
    char *i; // = "receive";
} EventData1;
char *event_data2 = "MuttonPrice2";
typedef struct
{
    EventData data;
    EventData1 data1;
} EventData2;

/*  注册订阅的处理函数，当有事件会触发对应的函数  */
void HandlereceiveEvent(void *data) // Observer 1
{
    EventData *d = (EventData *)data;
    printf("%s, data: %d \n", __FUNCTION__, d->e);
}
void HandlereceiveEvent1(void *data) // Observer 2
{
    EventData1 *d = (EventData1 *)data;
    printf("%s, data: %s \n", __FUNCTION__, d->i);
}
void HandlereceiveEvent2(void *data) // Observer 1
{
    EventData2 *d = (EventData2 *)data;
    printf("%s, data: %d, %s \n", __FUNCTION__, d->data.e, d->data1.i);
}

typedef void (*HandleEvent)(void *);

/* 订阅事件节点 */
typedef struct EventSubscriberNode
{
    char *event;
    HandleEvent func;
    struct EventSubscriberNode *next; // = nullptr;
} EventSubscriberNode;
EventSubscriberNode *eventSubscriberHead = NULL;

/* 发布事件节点 */
/* 发布的时候添加到链表就行，当事件处理完成后，会删除这个事件 */
typedef struct EventPublishNode
{
    char *event;
    void *data;
    struct EventPublishNode *next; // = nullptr;
} EventPublishNode;
EventPublishNode *eventPublishHead = NULL;

/*   采用链表的方式存储   */
pthread_mutex_t _mutexSubscriber;
pthread_mutex_t _mutexPublish;

void init()
{
    eventSubscriberHead = (EventSubscriberNode *)malloc(sizeof(EventSubscriberNode));
    eventSubscriberHead->event = NULL;
    eventSubscriberHead->func = NULL;
    eventSubscriberHead->next = NULL;

    eventPublishHead = (EventPublishNode *)malloc(sizeof(EventPublishNode));
    eventPublishHead->event = NULL;
    eventPublishHead->next = NULL;
    eventPublishHead->data = NULL;

    pthread_mutex_init(&_mutexSubscriber, NULL);
    pthread_mutex_init(&_mutexPublish, NULL);
}

void SubscriberEvent(char *event, HandleEvent func)
{
    EventSubscriberNode *node = (EventSubscriberNode *)malloc(sizeof(EventSubscriberNode));
    node->event = event;
    node->func = func;
    node->next = NULL;

    pthread_mutex_lock(&_mutexSubscriber);
    EventSubscriberNode *current = eventSubscriberHead;
    while (current->next != NULL)
        current = current->next;
    current->next = node;
    pthread_mutex_unlock(&_mutexSubscriber);
}

void releaseSubscriberEvent(char *event, HandleEvent func)
{
    pthread_mutex_lock(&_mutexSubscriber);
    for (EventSubscriberNode *current = eventSubscriberHead; current != NULL; current = current->next)
    {
        if (current->event == event && current->func == func)
        {
            if (current->next == NULL)
            {
                free(current);
                current->event = NULL;
                current->func = NULL;
                current = NULL;
                break;
            }
            EventSubscriberNode *de = current;
            current->event = current->next->event;
            current->func = current->next->func;
            current->next = current->next->next;
            free(de);
            de = NULL;
            break;
        }
    }
    pthread_mutex_unlock(&_mutexSubscriber);
}

void PublishEvent(char *event, void *data)
{
    EventPublishNode *node = (EventPublishNode *)malloc(sizeof(EventPublishNode));
    node->event = event;
    node->data = data;
    node->next = NULL;

    pthread_mutex_lock(&_mutexPublish);
    EventPublishNode *current = eventPublishHead;
    while (current->next != NULL)
        current = current->next;
    current->next = node;
    pthread_mutex_unlock(&_mutexPublish);
}

/* 事件处理中心  */
void *EventProcess(void *p)
{
    while (1)
    {
        EventPublishNode *cur = NULL;

        pthread_mutex_lock(&_mutexPublish);
        for (EventPublishNode *curPublish = eventPublishHead; curPublish != NULL; curPublish = curPublish->next)
            if (curPublish->event != NULL)
            {
                if (curPublish->next == NULL)
                {
                    cur = curPublish;
                    break;
                }

                cur = curPublish->next;
                void *d = curPublish->data;
                char *e = curPublish->event;

                curPublish->event = curPublish->next->event;
                curPublish->data = curPublish->next->data;
                curPublish->next = curPublish->next->next;

                cur->data = d;
                cur->event = e;

                break;
            }
        pthread_mutex_unlock(&_mutexPublish);

        if (cur == NULL)
            continue;

        pthread_mutex_lock(&_mutexSubscriber);
        for (EventSubscriberNode *curSubscriber = eventSubscriberHead; curSubscriber != NULL; curSubscriber = curSubscriber->next)
            if (curSubscriber->event == cur->event && curSubscriber->event != NULL)
                curSubscriber->func(cur->data);
        pthread_mutex_unlock(&_mutexSubscriber);

        free(cur->data);
        free(cur);
        cur->event = NULL;
        cur->data = NULL;
        cur = NULL;
    }
    return NULL;
}

void theardProc()
{
    pthread_t pt_id;
    int ret = pthread_create(&pt_id, NULL, EventProcess, NULL);
}

int main()
{
    init();       // 初始化事件处理中心
    theardProc(); // 启动线程运行事件处理中心

    /* 注册订阅事件 */
    SubscriberEvent(event_data, HandlereceiveEvent);
    SubscriberEvent(event_data1, HandlereceiveEvent1);
    SubscriberEvent(event_data2, HandlereceiveEvent2);

    /* 发布事件 */
    EventData *d = (EventData *)malloc(sizeof(EventData));
    d->e = 18888;
    PublishEvent(event_data, d);

    EventData1 *d1 = (EventData1 *)malloc(sizeof(EventData1));
    d1->i = "event_data1";
    PublishEvent(event_data1, d1);

    EventData2 *d2 = (EventData2 *)malloc(sizeof(EventData2));
    d2->data.e = 3000;
    d2->data1.i = "event3000";
    PublishEvent(event_data2, d2);

    //Sleep(20);

    return 0;
}
