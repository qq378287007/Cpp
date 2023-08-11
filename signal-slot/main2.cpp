
#include <iostream>
#include <vector>
#include <string>
using namespace std;

template <class T>
class SlotBase
{
public:
    virtual ~SlotBase() = default;
    virtual void Exec(T param1) = 0;
};

template <class T, class T1>
class Slot : public SlotBase<T1>
{
    typedef void (T::*Func)(T1);

public:
    explicit Slot(T *pOjb, Func func) : m_pSlotBase(pOjb), m_func(func) {}

    void Exec(T1 param1)
    {
        if (m_pSlotBase && m_func)
            (m_pSlotBase->*m_func)(param1);
    }

private:
    T *m_pSlotBase{nullptr};
    Func m_func{nullptr};
};

template <class T1>
class Signal
{
public:
    template <class T>
    void Bind(T *obj, void (T::*func)(T1))
    {
        m_pSlotSet.push_back(new Slot<T, T1>(obj, func));
    }

    void operator()(T1 param1)
    {
        for (int i = 0; i < (int)m_pSlotSet.size(); i++)
            m_pSlotSet[i]->Exec(param1);
    }

private:
    vector<SlotBase<T1> *> m_pSlotSet;
};

template <class T1>
class TestSignal
{
public:
    void emit(T1 value)
    {
        ValueChanged(value);
    }

public:
    Signal<T1> ValueChanged;
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
};

#define Connect(sender, signal, receiver, method) (sender)->signal.Bind(receiver, method)
int main()
{
    TestSignal<int> *pSinal1 = new TestSignal<int>();
    TestSignal<string> *pSinal2 = new TestSignal<string>();
    TestSlot *pSlot = new TestSlot();

    Connect(pSinal1, ValueChanged, pSlot, &TestSlot::capture1);
    Connect(pSinal2, ValueChanged, pSlot, &TestSlot::capture2);
    pSinal1->emit(1);
    pSinal2->emit(string("hahhaha"));

    delete pSlot;
    delete pSinal2;
    delete pSinal1;

    return 0;
}
