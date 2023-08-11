#include <future>
#include <thread>
#include <iostream>
using namespace std;

bool is_prime(int x) { return true; }

void test_prime(promise<bool> &result, int x) { result.set_value(is_prime(x)); }

int main()
{
    promise<bool> res;
    future<bool> f = res.get_future();

    thread t(test_prime, ref(res), 5);
    cout << f.get() << endl;
    t.join();
    return 0;
}