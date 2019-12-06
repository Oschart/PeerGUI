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
#define MyData "/home/wan/Documents/MyData/"
#define MyImages "/home/wan/Documents/MyData/MyImages/"

#define PREVIEWS "/home/wan/Documents/MyData/Previews/"
#define GrantedImages "/home/wan/Documents/MyData/GrantedImages/"

#define MyImages_db "/home/wan/Documents/MyData/my_images_db.txt"
#define GrantedImages_db "/home/wan/Documents/MyData/granted_images_db.txt"
#define Previews_db "/home/wan/Documents/MyData/previews_db.txt"

#define MAX_SIZE 8192
#define DEF_IMG(sz1) Image::readImage("/home/wan/Documents/MyData/SPACE.JPG", sz1)

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
    ANSWER_QUOTA_REQUEST,
    ANSWER_IMAGE_REQUEST,
    GET_USER_TITLES_CALLBACK,
    REQUEST_IMAGE_CALLBACK,
    REQUEST_QUOTA_CALLBACK,
    SET_QUOTA_CALLBACK
};

using namespace std;

struct imageQuotaRequest
{
    string requester;
    string imageName;
    int quota;
    imageQuotaRequest(string _requester, string _imageName, int _quota)
    {
        requester = _requester;
        imageName = _imageName;
        quota = _quota;
    }
};

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

    void sendRequest(opType operation, string otherpeer, string args);

    void answerQuotaRequest(int request_id, int decision);
    void approveQuotaRequest(string otherpeer, string imageName, int quota);
    void denyQuotaRequest(string otherpeer, string imageName);
    
    void answerImageRequest(int request_id, int decision);
    void approveImageRequest(string otherpeer, string imageName, int quota);
    void denyImageRequest(string otherpeer, string imageName);

    Message *doOperation(Message *_received, IP user_ip, Port user_port);

    void viewGrantedImage(string imageName);
    void setQuotaGrantedImage(string imageName, int quota);
    void clearTempImages();

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
    //map<string, string> imageToPeer;

    vector<imageQuotaRequest> quotaRequests;
    vector<imageQuotaRequest> imageRequests;

    pair<IP, Port> getAddress(string otherPeer);
    void listenerRun();

    map<opType, int> argCount;

    set<string> myImageTitles;
    set<string> grantedImagesTitles;
    set<string> previewsTitles;

    vector<string> tempImages;

    void cacheDB(string path, set<string> &cache);
    void appendFileAndCache(string path, set<string> &cache, string entry);
};
//#include "Peer.cpp"
#endif // PEER_H
