#include <iostream>
#include <vector>
#include <Windows.h>
using namespace std;

class Observer
{
public:
    virtual ~Observer() {}
    virtual void msgs(int, int) = 0;
    virtual void msgw(int, int) = 0;
};

class Mobile : public Observer
{
public:
    virtual void msgs(int _ms, int _mr)
    {
        cout << "收到短消息：存入" << _ms << "元，"
             << "余额" << _mr << "元；" << endl;
    }
    virtual void msgw(int _mw, int _mr)
    {
        cout << "收到短消息：取出" << _mw << "元，"
             << "余额" << _mr << "元；" << endl;
    }
};

class QQ : public Observer
{
public:
    virtual void msgs(int _qs, int _qr)
    {
        cout << "收到QQ消息：存入" << _qs << "元，"
             << "余额" << _qr << "元；" << endl;
    }
    virtual void msgw(int _qw, int _qr)
    {
        cout << "收到QQ消息：取出" << _qw << "元，"
             << "余额" << _qr << "元；" << endl;
    }
};

class Account
{
public:
    Account(int _rmb)
    {
        rmb = _rmb;
    }

    void addObserver(Observer *op)
    {
        vp.push_back(op);
    }

    void save(int _sr)
    {
        rmb += _sr;
        for (vector<Observer *>::iterator it = vp.begin(); it != vp.end(); it++)
        {
            (*it)->msgs(_sr, rmb);
        }
    }

    void withdraw(int _wr)
    {
        if (_wr > rmb)
        {
            _wr = rmb;
            rmb = 0;
        }
        else
            rmb -= _wr;
        for (vector<Observer *>::iterator it = vp.begin(); it != vp.end(); it++)
        {
            (*it)->msgw(_wr, rmb);
        }
    }

private:
    vector<Observer *> vp;
    int rmb;
};


int main()
{
    Account account(1000);        // 银行开户，并存入1000元

    Mobile mobile;                // 手机对象
    account.addObserver(&mobile); // 在银行账号中注册手机

    QQ qq;                        // QQ对象
    account.addObserver(&qq);     // 在银行账号中注册QQ

    cout << "存入200元->" << endl;
    account.save(200); 

    cout<<endl;
    Sleep(1000);

    cout << "取出800元<-" << endl;
    account.withdraw(800);

    cout<<endl;
    Sleep(1000);

    cout << "尝试取出1000元<-" << endl;
    account.withdraw(1000); 

    return 0;
}