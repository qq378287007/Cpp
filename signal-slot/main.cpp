
#include <iostream>
#include <vector>
using namespace std;

class SlotBase
{
public:
    virtual ~SlotBase() = default;
    virtual void Exec(int param1) = 0;
};

template <class T>
class Slot : public SlotBase
{
    typedef void (T::*Func)(int);

public:
    explicit Slot(T *pOjb, Func func) : m_pSlotBase(pOjb), m_func(func) {}

    void Exec(int param1)
    {
        if (m_pSlotBase && m_func)
            (m_pSlotBase->*m_func)(param1);
    }

private:
    T *m_pSlotBase{nullptr};
    Func m_func{nullptr};
};

class Signal
{
public:
    template <class T>
    void Bind(T *obj, void (T::*func)(int))
    {
        m_pSlotSet.push_back(new Slot<T>(obj, func));
    }

    void operator()(int param1)
    {
        for (int i = 0; i < (int)m_pSlotSet.size(); i++)
            m_pSlotSet[i]->Exec(param1);
    }

private:
    vector<SlotBase *> m_pSlotSet;
};

class TestSignal
{
public:
    void emit(int value)
    {
        ValueChanged(value);
    }

public:
    Signal ValueChanged;
};

class TestSlot
{
public:
    void capture(int param)
    {
        cout << __FUNCTION__ << " " << param << endl;
    }
};

#define Connect(sender, signal, receiver, method) (sender)->signal.Bind(receiver, method)
int main()
{
    TestSignal *pSinal1 = new TestSignal();
    TestSlot *pFunc1 = new TestSlot();

    Connect(pSinal1, ValueChanged, pFunc1, &TestSlot::capture);
    pSinal1->emit(1);

    delete pFunc1;
    delete pSinal1;

    return 0;
}
