#include <unordered_map>
#include <vector>
#include <string>
#include <thread>
#include <functional>
#include <iostream>
using namespace std;

/* Inherit from it to add Event-Capturing-ability to your class
 * @T_EventClass: class that holds event info when emitted, sent to function that should be run
 * @T_EventKeyClass: class/type of key that distinguishes the event, default std::string, must be hashable
 */
template <typename T_EventClass, typename T_EventKeyClass = std::string>
class EventEmitter
{
    using EventFunction_t = function<void(const T_EventClass &)>;

private:
    unordered_map<T_EventKeyClass, vector<EventFunction_t>> functions_bundle; // to store functions with its keys

public:
    // that can be passed as bits
    enum Options
    {
        isAsync,
        prepend
    };

    /* store this listener function with this eventKey
     * @eventKey: key to event, if duplicated the function will be added to the previous
     * @listener: function/lambda that should be called when event is emitted
     * @options: function configurationss
     */
    void onEvent(T_EventKeyClass eventKey, EventFunction_t listener, int options = 0)
    {
        // func to store
        EventFunction_t func = listener;

        if (options & isAsync)
        {
            func = [&listener](const T_EventClass &evt)
            {
                thread(listener, evt);
            };
        }
        else if (options & prepend)
        {
            functions_bundle[eventKey].insert(functions_bundle[eventKey].begin(), func);

            return;
        }

        // not prepend
        functions_bundle[eventKey].push_back(func);
    }

    /* overload, function could take no Event
     * @eventKey: key to event, if duplicated the function will be added to the previous
     * @listener: function/lambda that should be called when event is emitted
     * @options: function configurationss
     */
    void onEvent(T_EventKeyClass eventKey, function<void()> listener, int options = 0)
    {
        // func to store
        EventFunction_t func = [&listener](const T_EventClass &evt)
        {
            listener();
        };

        if (options & isAsync)
        {
            func = [&listener](const T_EventClass &evt)
            {
                thread(listener);
            };
        }
        else if (options & prepend)
        {
            functions_bundle[eventKey].insert(functions_bundle[eventKey].begin(), func);
            return;
        }

        // not prepend
        functions_bundle[eventKey].push_back(func);
    }

    /* same as onEvent, but function removes itself after calling
     * @eventKey: key to event, if duplicated the function will be added to the previous
     * @listener: function/lambda that should be called when event is emitted
     * @options: function configurationss
     */
    void once(T_EventKeyClass eventKey, EventFunction_t listener, int options = 0)
    {
        // make a function that can remove itself
        EventFunction_t func;
        func = [&listener, this, &eventKey, &func](const T_EventClass &evt)
        {
            listener(evt);

            this->removeListener(eventKey, func);
        };

        // then call onEvent to add it
        onEvent(eventKey, func, options);
    }

    /* overload, function could take no Event
     * @eventKey: key to event, if duplicated the function will be added to the previous
     * @listener: function/lambda that should be called when event is emitted
     * @options: function configurationss
     */
    void once(T_EventKeyClass eventKey, function<void()> listener, int options = 0)
    {
        // make a function that can remove itself
        EventFunction_t func;
        func = [&listener, this, &eventKey, &func](const T_EventClass &evt)
        {
            listener();

            this->removeListener(eventKey, func);
        };

        // then call onEvent to add it
        onEvent(eventKey, func, options);
    }

    /* store this listener function with this eventKey at beginning
     * @eventKey: key to event, if duplicated the function will be added to the previous
     * @listener: function/lambda that should be called when event is emitted
     * @options: function configurationss
     */
    void prependListener(T_EventKeyClass eventKey, EventFunction_t listener, int options = 0)
    {
        onEvent(eventKey, listener, options | prepend);
    }

    /* overload, function could take no Event
     * @eventKey: key to event, if duplicated the function will be added to the previous
     * @listener: function/lambda that should be called when event is emitted
     * @options: function configurationss
     */
    void prependListener(T_EventKeyClass eventKey, function<void()> listener, int options = 0)
    {
        onEvent(eventKey, listener, options | prepend);
    }

    /* fire the event resulting in calling all functions in order, if event not found, nothing happens
     * @eventKey: of event to fire
     * @eventInfo: event object to send to functions to be called
     */
    void emitEvent(T_EventKeyClass eventKey, const T_EventClass &eventInfo)
    {
        // if found
        if (functions_bundle.find(eventKey) != functions_bundle.end())
        {
            // iterate through all functins and call them
            for (auto &func : functions_bundle[eventKey])
                func(eventInfo);
        }
    }

    /* erase all functions that has been bound to given key
     * @eventKey: to remove its functions if found
     */
    void removeAllListeners(T_EventKeyClass eventKey)
    {
        // get itr to event
        auto eventItr = functions_bundle.find(eventKey);
        if (eventItr != functions_bundle.end())
            // erase that event with all its listeneres if found
            functions_bundle.erase(eventItr);
    }

    /* erase all listeners */
    void removeAllListeners()
    {
        functions_bundle.clear();
    }

    /* @eventKey: to get its functions
     */
    vector<EventFunction_t> listeners(T_EventKeyClass eventKey)
    {
        return vector<EventFunction_t>(functions_bundle[eventKey]);
    }

    /* return vector of events that had been registered */
    vector<T_EventKeyClass> events()
    {
        return vector<T_EventKeyClass>(functions_bundle.begin(), functions_bundle.end());
    }

    /* remove one listener from registered listeneres if found
     * @eventKey: event that the listener is attached to
     * @listener: function to remove
     */
    void removeListener(T_EventKeyClass eventKey, EventFunction_t listener)
    {
        // if eventKey is found
        if (functions_bundle.find(eventKey) != functions_bundle.end())
        {
            // refrence to vector of functions
            vector<EventFunction_t> &funs = functions_bundle[eventKey];

            // find and erase
            auto itr = funs.find(listener);
            if (itr != funs.end())
                funs.erase(itr);

            // if event became empty, erase it
            if (funs.size() == 0)
                functions_bundle.erase(functions_bundle.find(eventKey));
        }
    }

    /* get listeners count if event is found, else 0
     * @eventKey: key to event to find
     */
    int listenersCount(T_EventKeyClass eventKey)
    {
        // if found
        if (functions_bundle.find(eventKey) != functions_bundle.end())
            return functions_bundle[eventKey].size(); // return its size

        return 0;
    }
};

struct Event
{
    std::string name;
};

// make our class that inherits from EventEmitter
class Car : public EventEmitter<Event> // you should provide your own Event Class and pass it in the template
{
public:
    int x, y; // just for now
};

void myEggFuncyFunction()
{
    cout << "this is an egg function, don\'t mess with it\n";
}

int main(int argc, char *argv[])
{
    Car c;
    c.x = 5;
    c.y = 8;

    /* assign listeners to events */

    // it could take void parametres
    c.onEvent("move", [&c]() { // notice how to access car members
        cout << "first move listener   "
             << "car is moving now\n";
        cout << "car x corrdinate is " << c.x << endl; // notice how we accessed car member [x]

        cout << ".. listener ended" << endl;
    });

    // or take an event object to describe its details
    c.onEvent("move", [](const Event &evt) { // it must be [const]
        cout << "second move listener   "
             << "car is still moving now\n";
        cout << evt.name << endl;

        cout << ".. listener ended" << endl;
    });

    c.onEvent("move", myEggFuncyFunction); // notice you can pass ordinary functions

    c.onEvent("stop", []() { // this would be called only if emit stop
        cout << "stopped\n";
    });

    c.emitEvent("move", {"move event info"}); // now all move listeners will be called in order (synchronous)

    /* copy listeners */

    auto lis = c.listeners("move"); // this is acopy vector of listeners
    Event e{"foo-bar-event"};       // we should make out event object so we could pass it
    for (auto l : lis)
        l(e);

    /* remove listeners */

    c.removeAllListeners("move");

    c.emitEvent("move", {"move event info"}); // this would call nothing

    return 0;
}