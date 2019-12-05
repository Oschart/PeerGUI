#include "../Message.h"
#include <iostream>
#include <time.h>
#include <stdlib.h>

using namespace std;

int main()
{
    int t;
    cin >> t;
    srand(time(NULL));
    while(t--)
    {
        size_t size = rand()%9000;
        char* data = new char[size];
        for(int i = 0; i < size; ++i)
        {
            data[i] = 'a' + rand()%26;
        }
        void* vptr = data;
        Message* m1 = new Message(rand()%256, vptr, size, rand()*rand() + rand());
        m1->setTotal(rand()*rand() + rand());
        m1->setIndex(rand()*rand() + rand());

        Message* m2 = new Message(m1->marshal());

        //cout << (*m1 == m2? "OK": "FAILED") << endl;
        delete m1;
        delete m2;
    }
}