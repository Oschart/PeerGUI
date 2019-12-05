#include <iostream>
#include "Peer.h"
#include  "Image.h"
using namespace std;

int main(){
    Peer a("localhost", 0, "localhost", 2468);
    string username = "fadi";
    string password = "a7aneek";
    a.signup(username, password);
    a.login(username, password);
    a.uploadImagePreview("oscar7amama.png", "skull2.png");    
    int n;
	cin >> n;
	/*
    username = "eslam";
    password = "secret";
    a.login(username, password);

    */
//    a.getPreviews();
    
}

