
#include <iostream>
#include <vector>
#include <string>
using namespace std;

template <class... Args>
class SlotBase
{
public:
    virtual ~SlotBase() = default;
    virtual void Exec(Args... args) = 0;
};

template <class T, class... Args>
class Slot : public SlotBase<Args...>
{
    typedef void (T::*Func)(Args...);

public:
    explicit Slot(T *pOjb, Func func) : m_pSlotBase(pOjb), m_func(func) {}

    void Exec(Args... args)
    {
        if (m_pSlotBase && m_func)
            (m_pSlotBase->*m_func)(args...);
    }

private:
    T *m_pSlotBase{nullptr};
    Func m_func{nullptr};
};

template <class... Args>
class Signal
{
public:
    template <class T>
    void Bind(T *obj, void (T::*func)(Args...))
    {
        m_pSlotSet.push_back(new Slot<T, Args...>(obj, func));
    }

    void operator()(Args... args)
    {
        for (int i = 0; i < (int)m_pSlotSet.size(); i++)
            m_pSlotSet[i]->Exec(args...);
    }

private:
    vector<SlotBase<Args...> *> m_pSlotSet;
};

template <class... Args>
class TestSignal
{
public:
    void emit(Args... args)
    {
        ValueChanged(args...);
    }

public:
    Signal<Args...> ValueChanged;
};

class TestSlot
{
public:
    void capture1(int param)
    {
        cout << __FUNCTION__ << " " << param << endl;
    }
    void capture2(string param)
    {
        cout << __FUNCTION__ << " " << param << endl;
    }
    void capture3(int param1, string param2, float param3)
    {
        cout << __FUNCTION__ << " " << param1 << " " << param2 << " " << param3 << endl;
    }
};

#define Connect(sender, signal, receiver, method) (sender)->signal.Bind(receiver, method)
int main()
{
    TestSignal<int> *pSinal1 = new TestSignal<int>();
    TestSignal<string> *pSinal2 = new TestSignal<string>();
    TestSignal<int, string, float> *pSinal3 = new TestSignal<int, string, float>();
    TestSlot *pSlot = new TestSlot();

    Connect(pSinal1, ValueChanged, pSlot, &TestSlot::capture1);
    Connect(pSinal2, ValueChanged, pSlot, &TestSlot::capture2);
    Connect(pSinal3, ValueChanged, pSlot, &TestSlot::capture3);
    pSinal1->emit(1);
    pSinal2->emit(string("hahhaha"));
    pSinal3->emit(1, string("hahhaha"), 2.3f);

    delete pSlot;
    delete pSinal3;
    delete pSinal2;
    delete pSinal1;

    return 0;
}
