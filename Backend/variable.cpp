#include <iostream>
#include <string>
#include <thread>
#include <atomic>
using namespace std; 
static atomic<int> n;
int m = 0;

void run(){
    for(int i = 0; i < 10000000; i++){
        n++;
        m++;
    }
}

int main(){
    n = 0;
    thread t(run);
    for(int i = 0; i < 10000000; i++){
        //cout << "B\n";
        n--;
        m--;
    }
    t.join();
    cout << n << endl;
    cout << m << endl;
}