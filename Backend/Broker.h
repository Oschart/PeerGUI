#ifndef BROKER_H
#define BROKER_H

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include "Server.h"

using namespace std;
typedef unsigned int IP;
typedef unsigned int Port;
typedef unsigned int Token;
typedef chrono::system_clock::time_point time_point;

#define SESSION_EXPIRY 10 // In seconds
#define USER_DB "user_db.txt"
#define PREVIEW_DB "preview_db.txt"
#define user_dir "./UserData"
#define MAX_PREVIEWS 20
#define PREVIEWS_PER_USER 2
#define DEF_IMG(sz1) Image::readImage("SPACE.JPG", sz1)

enum opType
{
    LOGIN,
    REGISTER,
    GET_IP,
    GET_PREVIEW_FEED,
    GET_USER_PREVIEW,
    UPLOAD_PREVIEW,
    GET_USER_TITLES_CALLBACK = 11,
    REQUEST_IMAGE_CALLBACK,
    REQUEST_QUOTA_CALLBACK,
    SET_QUOTA_CALLBACK

};

struct Address
{
    IP ip;
    Port port;
    Address() { ip = port = 0; }
    Address(IP _ip, Port _port) { ip = _ip, port = _port; }
    string toString() { return to_string(ip) + "," + to_string(port); }
};

class Broker : public Server
{
public:
    Broker(char *_listen_hostname, int _listen_port);
    void loadAuthDB();
    void loadPreviewDB();
    Message *doOperation(Message *_received, IP user_ip, Port user_port);
    int updateLastActive(Token _token);
    bool validToken(Token token);
    Token Login(string username, string password);
    int Register(string username, string password);
    Address getUserAddr(string username);
    int uploadPreview(string username, string title, Image preview);
    vector<Image> getUserPreviews(string username);
    vector<Image> getPreviewFeed();

    ~Broker();

private:
    map<opType, int> argCount;
    vector<string> users;
    map<string, Address> userAddr;
    map<string, string> userPassword;
    map<string, vector<string>> userPreviewTitles;
    map<string, Token> userToken;
    map<Token, string> tokenUser;
    map<string, time_point> lastActive;
    Token tokenCount;

    mutex lastActiveMutex;
    mutex userTokenMutex;
    mutex userPasswordMutex;
    mutex userAddrMutex;

};

//#include "Broker.cpp"
#endif
