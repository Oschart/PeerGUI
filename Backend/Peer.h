#ifndef PEER_H
#define PEER_H

#include <QFile>
#include <QStackedWidget>
#include <QFileSystemModel>
#include <QWidget>
#include <QListWidget>
#include <QListView>
#include <QFileDialog>
#include <QDebug>

#include <string>
#include <thread>
#include <initializer_list>
#include <atomic>
#include <vector>
#include <set>
#include "Message.h"
#include "Server.h"
//#include "ParseUtil.cpp"

#define BROKER_IP "full_slack"
#define BROKER_PORT 2468
typedef unsigned int IP;
typedef unsigned int Port;

#define separator ","
#define CODED "_c"
#define rootDIR "/home/wan/Documents/PICky/"



#define MAX_SIZE 8192
#define DEF_IMG(sz1) Image::readImage("/home/wan/Documents/PICky/SPACE.JPG", sz1)

enum opType
{
    //broker
    LOGIN = 0,
    REGISTER = 1,
    GET_IP = 2,
    GET_PREVIEW_FEED = 3,
    UPLOAD_PREVIEW = 4,
    GET_ALL_USERS = 5,
    CACHE_MSG = 6,
    NOTIFY_VIEW = 7,
    RETRIEVE_USER_PREVIEWS = 8,
    RETRIEVE_PREVIOUSLY_SENT = 9,
    //broker and peer
    ANSWER_QUOTA_REQUEST,
    ANSWER_IMAGE_REQUEST,
    //peer
    GET_USER_PREVIEWS,
    REQUEST_IMAGE,
    REQUEST_QUOTA,
    SET_QUOTA,
    
 
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
    int getPreviews();
    int getUserPreviews(string otherpeer);
    int retrieveUserPreviews();
    int retrievePreviouslySent();

    int getAllUsers(vector<string> &usernames);

    //void getUserImages(string otherpeer);
    //void getUserTitlesCallback(string otherpeer);

    int uploadImagePreview(string imageName, string imagePath);
    int requestImage(string otherpeer, string imageName, int quota);
    int requestImageQuota(string otherpeer, string imageName, int quota);
    int setImageQuota(string otherpeer, string imageName, int quota);

    void sendRequest(opType operation, string otherpeer, string args);

    void answerQuotaRequest(int request_id, int decision);
    void approveQuotaRequest(string otherpeer, string imageName, int quota);
    void denyQuotaRequest(string otherpeer, string imageName);
    
    void answerImageRequest(int request_id, int decision);
    void approveImageRequest(string otherpeer, string imageName, int quota);
    void denyImageRequest(string otherpeer, string imageName);

    Message *doOperation(Message *_received, IP user_ip, Port user_port);

    int notifyView(string imageName);
    void setQuotaGrantedImage(string imageName, int quota);
    void clearTempImages();

    string getMyImages();
    void uploadLocalImage(string path);
    
    void loadReceiverQuota();
    void writeBackQuotaDB();

    vector<Image> getImagesOwnedBy(string otherpeer);
    
    vector<imageQuotaRequest> quotaRequests;
    vector<imageQuotaRequest> imageRequests;

    map<string, map<string, int> > receiverQuota;
    string MyData;
    string MyImages;
    string PREVIEWS;
    string GrantedImages;
    string Quota_db;
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

    pair<IP, Port> getAddress(string otherPeer);
    void listenerRun();

    map<opType, int> argCount;

};
//#include "Peer.cpp"
#endif // PEER_H
