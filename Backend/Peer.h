#ifndef PEER_H
#define PEER_H
#include <string>
#include <thread>
#include <initializer_list>
#include <atomic>
#include <vector>
#include <set>
#include "Message.h"
#include "Server.h"
//#include "ParseUtil.cpp"

#define BROKER_IP "10.7.57.2"
#define BROKER_PORT 2468
typedef unsigned int IP;
typedef unsigned int Port;

#define separator ","
#define CODED "_c"
#define MyData "./MyData"
#define MyImages "./MyData/MyImages"

#define PREVIEWS "./MyData/Previews"
#define GrantedImages "./MyData/GrantedImages"

#define MyImages_db "./MyData/my_images_db.txt"
#define GrantedImages_db "./MyData/granted_images_db.txt"
#define Previews_db "./MyData/previews_db.txt"

#define MAX_SIZE 8192
#define DEF_IMG(sz1) Image::readImage("SPACE.JPG", sz1)

enum opType
{
    LOGIN,
    REGISTER,
    GET_IP,
    GET_PREVIEW_FEED,
    GET_USER_PREVIEW,
    UPLOAD_PREVIEW,
    SYNC_QUOTA,
    GET_USER_TITLES,
    REQUEST_IMAGE,
    REQUEST_QUOTA,
    SET_QUOTA,
    GET_USER_TITLES_CALLBACK,
    REQUEST_IMAGE_CALLBACK,
    REQUEST_QUOTA_CALLBACK,
    SET_QUOTA_CALLBACK
};

using namespace std;
class Peer : public Server
{

public:
    Peer(char *_myHostname, int _myPort, char *_shostname, int _sport);
    bool execute(Message *);
    int login(string username, string password);
    int signup(string username, string password);
    void getPreviews();
    void getUserPreviews(string otherpeer);

    void getUserTitles(string otherpeer);
    //void getUserTitlesCallback(string otherpeer);

    void uploadImagePreview(string imageName, string imagePath);
    void requestImage(string otherpeer, string imageName, int quota);
    void requestImageQuota(string otherpeer, string imageName, int quota);
    void setImageQuota(string otherpeer, string imageName, int quota);

    Message *doOperation(Message *_received, IP user_ip, Port user_port);
    Image loadMyImage(string path, int quota);
    string getMyTitles();
    void uploadLocalImage(string path);
    ~Peer();

private:
    char *brokerIP;
    int brokerPort;
    string username;
    string password;
    bool sessionActive;
    bool listenerActive;
    string sessionToken;
    string defaultImage;
    atomic<int> rpcID;
    map<string, string> imageToPeer;

    pair<IP, Port> getAddress(string otherPeer);
    void listenerRun();

    map<opType, int> argCount;

    set<string> myImageTitles;
    set<string> grantedImagesTitles;
    set<string> previewsTitles;

    void cacheDB(string path, set<string> &cache);
    void appendFileAndCache(string path, set<string> &cache, string entry);
};
//#include "Peer.cpp"
#endif // PEER_H
