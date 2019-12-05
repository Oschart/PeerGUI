#include <iostream>
#include <string>
#include "Broker.h"
#include "Server.h"
using namespace std;

int main(){
   Broker b("localhost", 2468);
    while(1){
        b.serveRequest();
    }

}
