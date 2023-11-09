#include <iostream>
#include <vector>
#include <memory>
using namespace std;

class Observer
{
public:
    virtual ~Observer() {}
    virtual void msg_save(int, int) = 0;
    virtual void msg_write(int, int) = 0;
};

class Mobile : public Observer
{
public:
    virtual void msg_save(int _ms, int _mr)
    {
        cout << "Received SMS: deposited " << _ms << " yuan, "
             << "balance " << _mr << " yuan;" << endl;
    }
    virtual void msg_write(int _mw, int _mr)
    {
        cout << "Received SMS: Retrieve " << _mw << "yuan, "
             << "balance " << _mr << " yuan;" << endl;
    }
};

class QQ : public Observer
{
public:
    virtual void msg_save(int _qs, int _qr)
    {
        cout << "Received QQ message: deposited " << _qs << " yuan, "
             << "balance " << _qr << " yuan;" << endl;
    }
    virtual void msg_write(int _qw, int _qr)
    {
        cout << "Received QQ message: Retrieve" << _qw << " yuan, "
             << "balance " << _qr << " yuan;" << endl;
    }
};

class Account
{
public:
    Account(int _rmb = 0)
    {
        rmb = _rmb;
    }

    void addObserver(shared_ptr<Observer> op)
    {
        vp.push_back(op);
    }

    void save(int _sr)
    {
        rmb += _sr;
        for (shared_ptr<Observer> o : vp)
            o->msg_save(_sr, rmb);
    }

    void withdraw(int _wr)
    {
        if (_wr > rmb)
            _wr = rmb;
        rmb -= _wr;
        for (shared_ptr<Observer> o : vp)
            o->msg_save(_wr, rmb);
    }

private:
    vector<shared_ptr<Observer>> vp;
    int rmb;
};

int main()
{
    Account account(1000);                      // 银行开户，并存入1000元
    account.addObserver(make_shared<Mobile>()); // 在银行账号中注册手机
    account.addObserver(make_shared<QQ>());     // 在银行账号中注册QQ

    cout << "deposit 200 yuan->" << endl;
    account.save(200);
    cout << endl;

    cout << "withdraw 800 yuan<-" << endl;
    account.withdraw(800);
    cout << endl;

    cout << "try to withdraw 1000 yuan<-" << endl;
    account.withdraw(1000);

    return 0;
}