#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    cout << "this is subApp message\n";

    if (argc < 2)
    {
        cout << "work... failed, the return code will be 1\n";
        return 1;
    }

    cout << "work... pass, the return code will be 0\n";
    return 0;
}