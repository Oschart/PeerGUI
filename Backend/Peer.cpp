#include "Peer.h"
Peer::Peer(char *_myHostname, int _myPort, char *_shostname, int _sport) : Server(_myHostname, _myPort)
{
    brokerIP = _shostname;
    brokerPort = _sport;
    this->udpSocket = new UDPSocket();
    listenerActive = 1;
    thread t(&Peer::listenerRun, this);
    t.detach();
    this->defaultImage = "/home/wan/Documents/MyData/SPACE.JPG";
    this->sessionToken = "9";
    rpcID = 10;

    //    createFolder("./previews");
    argCount[REQUEST_IMAGE] = 3;
    argCount[REQUEST_QUOTA] = 3;
    argCount[SET_QUOTA] = 3;
    argCount[ANSWER_QUOTA_REQUEST] = 4;
    argCount[ANSWER_IMAGE_REQUEST] = 3;
    argCount[GET_USER_PREVIEWS] = 1;
    argCount[NOTIFY_VIEW] = 2;
    argCount[RETRIEVE_PREVIOUSLY_SENT] = 1;

    loadReceiverQuota();
}

bool Peer::execute(Message *_message)
{
    int rpc_id = _message->getRPCId();

    condition_variable cv;
    mutex mx;
    unique_lock<mutex> lck(mx);
    this->rpcToMutex[rpc_id] = &cv;
    _message->setMessageType(Request);
    sendPartitioned(_message);
    int trials = 10;
    while ((cv.wait_for(lck, chrono::milliseconds(100)) == cv_status::timeout) && trials--)
    {
        sendPartitioned(_message);
    }
    return trials > 0;
}

void Peer::listenerRun()
{
    while (this->listenerActive)
    {
        this->serveRequest();
    }
}

int Peer::login(string username, string password)
{
    this->udpSocket->initializeClient(brokerIP, brokerPort);
    string args = username + separator + password;
    Message *toBeSent = new Message(LOGIN, stringToCharPtr(args), args.length(), (this->rpcID)++);
    toBeSent->setMessageType(Request);
    int result;
    if (this->execute(toBeSent))
    {
        int rpc_id = toBeSent->getRPCId();
        Message *received = this->rpcToMsg[rpc_id];
        this->rpcToMsg.erase(rpc_id);
        string content = string((char *)received->getMessage(), received->getMessageSize());
        if (content == "0")
        {
            cout << "Authentiaction failed\n";
            result = 0;
        }
        else
        {
            cout << "Authentication granted, token = " << content << endl;
            this->username = username;
            this->sessionToken = content;

            MyData = rootDIR + username + "/MyData/";
            MyImages = rootDIR + username + "/MyData/MyImages/";
            PREVIEWS = rootDIR + username + "/MyData/Previews/";
            GrantedImages = rootDIR + username + "/MyData/GrantedImages/";
            Quota_db = rootDIR + username + "/MyData/quota_db.txt";

            createFolder(rootDIR);
            createFolder(rootDIR + username);
            createFolder(MyData);
            createFolder(GrantedImages);
            createFolder(MyImages);
            createFolder(PREVIEWS);

            retrieveUserPreviews();
            retrievePreviouslySent();

            result = 1;
        }

        delete received;
    }
    else
    { //the waiting thread timed out
        cout << "Login operation timed out!\n";
        result = -1;
    }
    cout << "Token = " << this->sessionToken << endl;
    delete toBeSent;
    return result;
}

int Peer::signup(string username, string password)
{
    this->udpSocket->initializeClient(brokerIP, brokerPort);
    string args = username + separator + password;
    Message *toBeSent = new Message(REGISTER, stringToCharPtr(args), args.length(), (this->rpcID)++);
    toBeSent->setMessageType(Request);
    int result;
    if (this->execute(toBeSent))
    {

        int rpc_id = toBeSent->getRPCId();
        Message *received = this->rpcToMsg[rpc_id];
        this->rpcToMsg.erase(rpc_id);
        string content = string((char *)received->getMessage(), received->getMessageSize());
        if (content == "0")
        {
            cout << "Registration failed, invalid username\n";
            result = 0;
        }
        else if (content == "1")
        {
            cout << "Registration succeeded\n";
            result = 1;
        }
        else
            cout << "Invalid reply content!: " << content << endl;

        delete received;
    }
    else
    { //the waiting thread timed out
        cout << "Sign Up operation timed out!\n";
        result = -1;
    }
    delete toBeSent;
    return result;
}

pair<IP, Port> Peer::getAddress(string otherPeer)
{
    this->udpSocket->initializeClient(brokerIP, brokerPort);
    string args = this->sessionToken + separator + otherPeer;
    Message *toBeSent = new Message(GET_IP, stringToCharPtr(args), args.length(), (this->rpcID)++);
    toBeSent->setMessageType(Request);
    if (this->execute(toBeSent))
    {
        int rpc_id = toBeSent->getRPCId();
        Message *received = this->rpcToMsg[rpc_id];
        this->rpcToMsg.erase(rpc_id);

        string content = string((char *)received->getMessage(), received->getMessageSize());

        if (content == "0")
        {
            cout << "Get IP operation failed\n";
        }
        else
        {
            vector<string> args = extractArgs(content);
            if (args.size() == 2)
            {
                cout << "Received IP and port\n";
                return make_pair(unsigned(stoll(args[0])), unsigned(stoll(args[1])));
            }
            else
            {
                cout << "The Reply has invalid arguments\n";
            }
        }

        delete received;
    }
    else
    { //the waiting thread timed out
        cout << "Get IP operation timed out!\n";
    }
    return make_pair(0U, 0U);
}

int Peer::getPreviews()
{
    this->udpSocket->initializeClient(brokerIP, brokerPort);
    string args = this->sessionToken;
    Message *toBeSent = new Message(GET_PREVIEW_FEED, stringToCharPtr(args), args.length(), (this->rpcID)++);
    toBeSent->setMessageType(Request);
    int res;
    if (this->execute(toBeSent))
    {
        res = 1;
        int rpc_id = toBeSent->getRPCId();
        Message *received = this->rpcToMsg[rpc_id];
        this->rpcToMsg.erase(rpc_id);
        cout << "PREVIEWS RECEIVED\n";
        vector<uint8_t> flatArgs = Image::charPtrToVector((char *)received->getMessage(), received->getMessageSize());

        vector<Image> previews = extractImages(flatArgs);

        for (int i = 0; i < previews.size(); i++)
        {
            cout << "OWNER ====> " << previews[i].getOwner() << endl;
            string storedImageTitle = addUsertoName(previews[i].getTitle(), previews[i].getOwner());

            string path = PREVIEWS + storedImageTitle;
            Image::writeImage(path, previews[i].getContent());

            cout << "Image Title = " << storedImageTitle << endl;
        }

        delete received;
    }
    else
    { //the waiting thread timed out
        cout << "Get Previews operation timed out!\n";
        res = -1;
    }
    delete toBeSent;
    return res;
}

int Peer::retrieveUserPreviews()
{

    this->udpSocket->initializeClient(brokerIP, brokerPort);
    string args = this->sessionToken;
    Message *toBeSent = new Message(RETRIEVE_USER_PREVIEWS, stringToCharPtr(args), args.length(), (this->rpcID)++);
    toBeSent->setMessageType(Request);
    int res;
    if (this->execute(toBeSent))
    {
        res = 1;
        int rpc_id = toBeSent->getRPCId();
        Message *received = this->rpcToMsg[rpc_id];
        this->rpcToMsg.erase(rpc_id);
        cout << "Restored Images RECEIVED\n";

        vector<uint8_t> flatArgs = Image::charPtrToVector((char *)received->getMessage(), received->getMessageSize());

        vector<Image> previews = extractImages(flatArgs);

        for (int i = 0; i < previews.size(); i++)
        {
            cout << "OWNER ====> " << previews[i].getOwner() << endl;

            string path = MyImages + previews[i].getTitle();
            Image::writeImage(path, previews[i].getContent());
        }

        delete received;
    }
    else
    { //the waiting thread timed out
        cout << "Get Previews operation timed out!\n";
        res = -1;
    }
    delete toBeSent;
    return res;
}

int Peer::getAllUsers(vector<string> &usernames)
{
    this->udpSocket->initializeClient(brokerIP, brokerPort);
    string args = this->sessionToken;
    Message *toBeSent = new Message(GET_ALL_USERS, stringToCharPtr(args), args.length(), (this->rpcID)++);
    toBeSent->setMessageType(Request);
    int res;
    if (this->execute(toBeSent))
    {
        res = 1;
        int rpc_id = toBeSent->getRPCId();
        Message *received = this->rpcToMsg[rpc_id];
        this->rpcToMsg.erase(rpc_id);
        cout << "PREVIEWS RECEIVED\n";
        vector<uint8_t> flatArgs = Image::charPtrToVector((char *)received->getMessage(), received->getMessageSize());

        usernames = extractArgs(VectorToString(flatArgs));

        delete received;
        res = 1;
    }
    else
    { //the waiting thread timed out
        cout << "Get all users operation timed out!\n";
        res = -1;
    }
    delete toBeSent;
    return res;
}

int Peer::getUserPreviews(string otherpeer)
{

    pair<IP, Port> peerToAddress = this->getAddress(otherpeer);
    if (peerToAddress == make_pair(0U, 0U))
    {
        cout << "There's no peer record with such name in the broker\n";
        return 0;
    }
    cout << "Peer Address was received successfully, getting previews...\n";
    this->udpSocket->initializeClient(peerToAddress.first, peerToAddress.second);

    cout << "Addrersssss == " << peerToAddress.first << " " << peerToAddress.second << endl;
    string args = "1";
    Message *toBeSent = new Message(GET_USER_PREVIEWS, stringToCharPtr(args), args.length(), (this->rpcID)++);
    toBeSent->setMessageType(Request);
    int res;
    if (this->execute(toBeSent))
    {
        res = 1;
        int rpc_id = toBeSent->getRPCId();
        Message *received = this->rpcToMsg[rpc_id];
        this->rpcToMsg.erase(rpc_id);
        cout << "USER PREVIEWS RECEIVED\n";

        vector<uint8_t> flatArgs = Image::charPtrToVector((char *)received->getMessage(), received->getMessageSize());

        vector<Image> previews = extractImages(flatArgs);
        for (int i = 0; i < previews.size(); i++)
        {
            string storedImageTitle = addUsertoName(previews[i].getTitle(), previews[i].getOwner());
            string path = PREVIEWS + storedImageTitle;
            Image::writeImage(path, previews[i].getContent());
        }
        delete received;
    }
    else
    {
        cout << "Get User Previews operation timed out!\n";
        res = -1;
    }
    return res;
}

int Peer::retrievePreviouslySent()
{
    vector<string> usernames;
    getAllUsers(usernames);
    int res = 1;
    for (string otherpeer : usernames)
    {
        if(otherpeer == username) continue;
        pair<IP, Port> peerToAddress = this->getAddress(otherpeer);
        if (peerToAddress == make_pair(0U, 0U))
        {
            cout << "There's no peer record with such name in the broker\n";
            continue;
        }
        cout << "Peer Address was received successfully, getting previews...\n";
        this->udpSocket->initializeClient(peerToAddress.first, peerToAddress.second);

        cout << "Addrersssss == " << peerToAddress.first << " " << peerToAddress.second << endl;
        string args = username;
        Message *toBeSent = new Message(RETRIEVE_PREVIOUSLY_SENT, stringToCharPtr(args), args.length(), (this->rpcID)++);
        toBeSent->setMessageType(Request);
        int res;
        if (this->execute(toBeSent))
        {
            res = 1;
            int rpc_id = toBeSent->getRPCId();
            Message *received = this->rpcToMsg[rpc_id];
            this->rpcToMsg.erase(rpc_id);
            cout << "Previously Sent RECEIVED\n";

            vector<uint8_t> flatArgs = Image::charPtrToVector((char *)received->getMessage(), received->getMessageSize());

            vector<Image> prev_sent = extractImages(flatArgs);
            for (int i = 0; i < prev_sent.size(); i++)
            {
                string storedImageTitle = prev_sent[i].getTitle();
                string path = MyImages + storedImageTitle;
                Image::writeImage(path, prev_sent[i].getContent());
                int quota = prev_sent[i].getQuota();
                receiverQuota[storedImageTitle][otherpeer] = quota;
            }
            delete received;
        }
        else
        {
            cout << "Previously Sent Timed Out On " << otherpeer << "\n";
            res = -1;
        }
        
    }
    return res;
}

int Peer::uploadImagePreview(string imageName, string imagePath)
{
    this->udpSocket->initializeClient(BROKER_IP, BROKER_PORT);
    int sz1, sz2;
    Image toSend = Image(DEF_IMG(sz1), Image::readImage(imagePath, sz2), this->username, 0);

    toSend.setTitle(imageName);
    vector<Image> images;
    images.push_back(toSend);
    vector<uint8_t> flattenedV = flattenImages(images);
    string flattenedP(flattenedV.begin(), flattenedV.end());
    string args = this->sessionToken + separator + flattenedP;

    Message *toBeSent = new Message(UPLOAD_PREVIEW, stringToCharPtr(args), args.length(), (this->rpcID)++);
    toBeSent->setMessageType(Request);
    int res;
    if (this->execute(toBeSent))
    {
        int rpc_id = toBeSent->getRPCId();
        Message *received = this->rpcToMsg[rpc_id];
        this->rpcToMsg.erase(rpc_id);
        string content = string((char *)received->getMessage(), received->getMessageSize());
        if (content == "1")
        {
            cout << "Upload Succeeded\n";
            res = 1;
        }
        else if (content == "0")
        {
            cout << "Upload Failed\n";
            res = 0;
        }
        else
        {
            cout << "Invalid Content\n";
            res = -1;
        }
        delete received;
    }
    else
    {
        cout << "Upload Previews operation timed out!\n";
        res = -1;
    }
    return res;
}

int Peer::requestImage(string otherpeer, string imageName, int quota)
{
    pair<IP, Port> peerToAddress = this->getAddress(otherpeer);
    cout << "this passes ..?" << endl;
    if (peerToAddress == make_pair(0U, 0U))
    {
        cout << "There's no peer record with such name in the broker\n";
        return -1;
    }
    cout << "Peer Address was received successfully, requesting image...\n";
    this->udpSocket->initializeClient(peerToAddress.first, peerToAddress.second);
    cout << "I will send to this " << peerToAddress.second << endl;

    string args = username + separator + imageName + separator + to_string(quota);
    Message *toBeSent = new Message(REQUEST_IMAGE, stringToCharPtr(args), args.length(), (this->rpcID)++);
    toBeSent->setMessageType(Request);
    int res;
    if (this->execute(toBeSent))
    {
        int rpc_id = toBeSent->getRPCId();
        Message *received = this->rpcToMsg[rpc_id];
        this->rpcToMsg.erase(rpc_id);
        string content = string((char *)received->getMessage(), received->getMessageSize());

        if (content == "1")
        {
            cout << "Image request sent successfully\n";
            res = 1;
        }
        else
        {
            cout << "Undefined response for image request\n";
            return -1;
        }

        delete received;
    }
    else
    {
        cout << "Request Image operation timed out!, will send to the broker\n";
        toBeSent->setOperation(CACHE_MSG);
        string content = string((char *)toBeSent->getMessage(), toBeSent->getMessageSize());
        content = sessionToken + separator + otherpeer + separator + to_string(REQUEST_IMAGE) + separator + content;
        cout << "CONTENT:  " << content << endl;
        toBeSent->setMessage(stringToCharPtr(content), content.length());
        this->udpSocket->initializeClient(BROKER_IP, BROKER_PORT);
        if (this->execute(toBeSent))
        {
            int rpc_id = toBeSent->getRPCId();
            Message *received = this->rpcToMsg[rpc_id];
            this->rpcToMsg.erase(rpc_id);
            string content = string((char *)received->getMessage(), received->getMessageSize());

            if (content == "1")
            {
                cout << "Image request sent successfully\n";
                res = 2;
            }
            else
            {
                cout << "Undefined response for image request\n";
                res = -1;
            }
            delete received;
        }
        else
        {
            //timeout on the send to broker after timeout on send to peer
            res = -1;
        }
    }
    return res;
}

int Peer::requestImageQuota(string otherpeer, string imageName, int quota)
{
    pair<IP, Port> peerToAddress = this->getAddress(otherpeer);
    if (peerToAddress == make_pair(0U, 0U))
    {
        cout << "There's no peer record with such name in the broker\n";
        return -1;
    }
    //removeUserfromName(imageName);
    int res;
    cout << "Peer Address was received successfully, requesting image ...\n";
    this->udpSocket->initializeClient(peerToAddress.first, peerToAddress.second);

    string args = username + separator + imageName + separator + to_string(quota);
    Message *toBeSent = new Message(REQUEST_QUOTA, stringToCharPtr(args), args.length(), (this->rpcID)++);

    toBeSent->setMessageType(Request);
    if (this->execute(toBeSent))
    {
        int rpc_id = toBeSent->getRPCId();
        Message *received = this->rpcToMsg[rpc_id];
        this->rpcToMsg.erase(rpc_id);
        string content = string((char *)received->getMessage(), received->getMessageSize());
        if (content == "1")
        {
            cout << "Quota request sent successfully\n";
            res = 1;
        }
        else
        {
            cout << "Undefined response for quota request\n";
            res = -1;
        }

        delete received;
    }
    else
    {
        cout << "Request Image Quota operation timed out!, will send to the broker\n";
        toBeSent->setOperation(CACHE_MSG);
        string content = string((char *)toBeSent->getMessage(), toBeSent->getMessageSize());
        content = sessionToken + separator + otherpeer + separator + to_string(REQUEST_QUOTA) + separator + content;
        toBeSent->setMessage(stringToCharPtr(content), content.length());
        this->udpSocket->initializeClient(BROKER_IP, BROKER_PORT);
        if (this->execute(toBeSent))
        {
            int rpc_id = toBeSent->getRPCId();
            Message *received = this->rpcToMsg[rpc_id];
            this->rpcToMsg.erase(rpc_id);
            string content = string((char *)received->getMessage(), received->getMessageSize());

            if (content == "1")
            {
                cout << "Image request sent successfully\n";
                res = 2;
            }
            else
            {
                cout << "Undefined response for image request\n";
                res = -1;
            }
            delete received;
        }
        else
        {
            //timeout on the send to broker after timeout on send to peer
            res = -1;
        }
    }
    return res;
}

int Peer::notifyView(string title)
{
    string otherpeer = removeUserfromName(title);

    pair<IP, Port> peerToAddress = this->getAddress(otherpeer);
    if (peerToAddress == make_pair(0U, 0U))
    {
        cout << "There's no peer record with such name in the broker\n";
        return -1;
    }
    int res;
    cout << "Peer Address was received successfully, notifying...\n";
    this->udpSocket->initializeClient(peerToAddress.first, peerToAddress.second);

    string args = username + separator + title;
    Message *toBeSent = new Message(NOTIFY_VIEW, stringToCharPtr(args), args.length(), (this->rpcID)++);

    toBeSent->setMessageType(Request);
    if (this->execute(toBeSent))
    {
        int rpc_id = toBeSent->getRPCId();
        Message *received = this->rpcToMsg[rpc_id];
        this->rpcToMsg.erase(rpc_id);
        string content = string((char *)received->getMessage(), received->getMessageSize());
        if (content == "1")
        {
            cout << "View notification sent successfully to peer\n";
            res = 1;
        }
        else
        {
            cout << "Undefined response for view notification\n";
            res = -1;
        }

        delete received;
    }
    else
    {
        cout << "View notification operation timed out!, will send to the broker\n";
        toBeSent->setOperation(CACHE_MSG);
        string content = string((char *)toBeSent->getMessage(), toBeSent->getMessageSize());
        content = sessionToken + separator + otherpeer + separator + to_string(NOTIFY_VIEW) + separator + content;
        toBeSent->setMessage(stringToCharPtr(content), content.length());
        this->udpSocket->initializeClient(BROKER_IP, BROKER_PORT);
        if (this->execute(toBeSent))
        {
            int rpc_id = toBeSent->getRPCId();
            Message *received = this->rpcToMsg[rpc_id];
            this->rpcToMsg.erase(rpc_id);
            string content = string((char *)received->getMessage(), received->getMessageSize());

            if (content == "1")
            {
                cout << "View notification sent successfully to broker\n";
                res = 2;
            }
            else
            {
                cout << "Undefined response for view notification\n";
                res = -1;
            }
            delete received;
        }
        else
        {
            //timeout on the send to broker after timeout on send to peer
            res = -1;
        }
    }
    return res;
}

int Peer::setImageQuota(string otherpeer, string imageName, int quota)
{
    pair<IP, Port> peerToAddress = this->getAddress(otherpeer);
    if (peerToAddress == make_pair(0U, 0U))
    {
        cout << "There's no peer record with such name in the broker\n";
        return -1;
    }

    cout << "Peer Address was received successfully, setting image quota...\n";
    this->udpSocket->initializeClient(peerToAddress.first, peerToAddress.second);

    int res;
    string args = username + separator + imageName + separator + to_string(quota);
    Message *toBeSent = new Message(SET_QUOTA, stringToCharPtr(args), args.length(), (this->rpcID)++);
    toBeSent->setMessageType(Request);
    if (this->execute(toBeSent))
    {
        int rpc_id = toBeSent->getRPCId();
        Message *received = this->rpcToMsg[rpc_id];
        this->rpcToMsg.erase(rpc_id);
        string content = string((char *)received->getMessage(), received->getMessageSize());

        if (content == "1")
        {
            cout << "Setting image quota request sent successfully to peer\n";
            receiverQuota[imageName][otherpeer] = quota;
            res = 1;
        }
        else
        {
            cout << "Undefined response for Setting image quota request\n";
            res = -1;
        }

        delete received;
    }
    else
    {
        cout << "View notification operation timed out!, will send to the broker\n";
        toBeSent->setOperation(CACHE_MSG);
        string content = string((char *)toBeSent->getMessage(), toBeSent->getMessageSize());
        content = sessionToken + separator + otherpeer + separator + to_string(SET_QUOTA) + separator + content;
        toBeSent->setMessage(stringToCharPtr(content), content.length());
        this->udpSocket->initializeClient(BROKER_IP, BROKER_PORT);
        if (this->execute(toBeSent))
        {
            int rpc_id = toBeSent->getRPCId();
            Message *received = this->rpcToMsg[rpc_id];
            this->rpcToMsg.erase(rpc_id);
            string content = string((char *)received->getMessage(), received->getMessageSize());

            if (content == "1")
            {
                cout << "View notification sent successfully to broker\n";
                receiverQuota[imageName][otherpeer] = quota;
                res = 2;
            }
            else
            {
                cout << "Undefined response for view notification\n";
                res = -1;
            }
            delete received;
        }
        else
        {
            //timeout on the send to broker after timeout on send to peer
            res = -1;
        }
    }
    return res;
}

// --------------------------------------------------

void Peer::answerImageRequest(int request_id, int decision)
{
    imageQuotaRequest request = imageRequests[request_id];
    if (decision == 1)
    {
        approveImageRequest(request.requester, request.imageName, request.quota);
    }
    else
    {
        denyImageRequest(request.requester, request.imageName);
    }
    imageRequests.erase(imageRequests.begin() + request_id);
}

void Peer::denyImageRequest(string otherpeer, string imageName)
{

    string args = string("0") + separator + username + separator + imageName + separator + "0";
    sendRequest(ANSWER_IMAGE_REQUEST, otherpeer, args);
}

void Peer::approveImageRequest(string otherpeer, string imageName, int quota)
{
    int sz;
    string path = MyImages + imageName;
    Image img(DEF_IMG(sz), Image::readImage(path, sz), username, quota);
    vector<Image> imageVec;
    imageVec.push_back(img);

    string flatImage = VectorToString(flattenImages(imageVec));
    string args = string("1") + separator + username + separator + imageName + separator + flatImage;
    sendRequest(ANSWER_IMAGE_REQUEST, otherpeer, args);

    //removeUserfromName(imageName);
    receiverQuota[imageName][otherpeer] = quota;
}

// --------------------------------------------------

void Peer::answerQuotaRequest(int request_id, int decision)
{
    imageQuotaRequest request = quotaRequests[request_id];
    if (decision == 1)
    {
        approveQuotaRequest(request.requester, request.imageName, request.quota);
    }
    else
    {
        denyQuotaRequest(request.requester, request.imageName);
    }
    quotaRequests.erase(quotaRequests.begin() + request_id);
}

void Peer::denyQuotaRequest(string otherpeer, string imageName)
{
    string args = string("0") + separator + username + separator + imageName + separator + "0";
    sendRequest(ANSWER_QUOTA_REQUEST, otherpeer, args);
}

void Peer::approveQuotaRequest(string otherpeer, string imageName, int quota)
{
    cout << "From approve: image name = " << imageName << endl;
    string args = string("1") + separator + username + separator + imageName + separator + to_string(quota);
    sendRequest(ANSWER_QUOTA_REQUEST, otherpeer, args);

    removeUserfromName(imageName);
    receiverQuota[imageName][otherpeer] = quota;
}

void Peer::sendRequest(opType operation, string otherpeer, string args)
{
    pair<IP, Port> peerToAddress = this->getAddress(otherpeer);
    if (peerToAddress == make_pair(0U, 0U))
    {
        cout << "There's no peer record with such name in the broker\n";
        return;
    }

    cout << "Peer Address was received successfully, setting image quota...\n";
    this->udpSocket->initializeClient(peerToAddress.first, peerToAddress.second);

    Message *toBeSent = new Message(operation, stringToCharPtr(args), args.length(), (this->rpcID)++);
    toBeSent->setMessageType(Request);

    if (this->execute(toBeSent))
    {
        int rpc_id = toBeSent->getRPCId();
        Message *received = this->rpcToMsg[rpc_id];
        this->rpcToMsg.erase(rpc_id);
        string content = string((char *)received->getMessage(), received->getMessageSize());

        // Don't care about the response: it's an order
        delete received;
    }
    else
    {
        cout << "Operation " << int(operation) << " timed out!\n";
    }
}

// ------------------------------------------------

Message *Peer::doOperation(Message *_received, IP user_ip, Port user_port)
{
    Message *response = _received;
    opType op = opType(_received->getOperation());
    vector<uint8_t> flatArgs = Image::charPtrToVector((char *)_received->getMessage(), _received->getMessageSize());
    vector<vector<uint8_t>> args = extractArgsUntil(flatArgs, argCount[op]);

    string msgBody = "0"; // Default reply (invalid operation)
    bool valid = true;

    if (valid)
    {

        switch (op)
        {
        case REQUEST_IMAGE:
        {
            cout << "REQUEST IMAGE BEGIN" << endl;
            string other_username = VectorToString(args[0]);
            string title = VectorToString(args[1]);
            filter(title);
            cout << "after filter   " << title << endl;
            int quota = stoll(VectorToString(args[2]));

            imageQuotaRequest request(other_username, title, quota);
            imageRequests.push_back(request);
            cout << "request image end" << endl;
            msgBody = "1"; // Request received

            //SaveQuotaRecord(other_username, title, quota);
            break;
        }
        case ANSWER_IMAGE_REQUEST:
        {

            string response = VectorToString(args[0]);
            string sender = VectorToString(args[1]);
            string imageName = VectorToString(args[2]);

            if (response == "1")
            {
                cout << "Image request approved\n";
                //cout << "the size of args[3] " << args[3].size() << endl;
                cout << "the sender " << sender << endl;
                cout << "the image name " << imageName << endl;
                string full_image_name = addUsertoName(imageName, sender);
                vector<Image> vec = extractImages(flatArgs);
                Image::writeImage(GrantedImages + full_image_name, vec[0].getCodified());
            }
            else if (response == "0")
            {
                cout << "Image request denied\n";
            }
            else
            {
                cout << "Undefined paramater in answer Image request\n";
            }
            msgBody = "1"; // Request received
            break;
        }
        case REQUEST_QUOTA:
        {
            string other_username = VectorToString(args[0]);
            string title = VectorToString(args[1]);
            filter(title);
            int quota = stoll(VectorToString(args[2]));

            imageQuotaRequest request(other_username, title, quota);
            quotaRequests.push_back(request);

            msgBody = "1"; // Request received

            //SaveQuotaRecord(other_username, title, quota);
            break; // TODO: ask user for permission
            msgBody = "1";
            break;
        }
        case SET_QUOTA:
        {
            string sender = VectorToString(args[0]);
            string imageName = VectorToString(args[1]);

            string image_name = addUsertoName(imageName, sender);
            int quota = stoll(VectorToString(args[2]));
            setQuotaGrantedImage(image_name, quota);
            msgBody = "1"; // Request received

            receiverQuota[imageName][sender] = quota;
            break;
        }
        case NOTIFY_VIEW:
        {
            string viewer = VectorToString(args[0]);
            string imageTitle = VectorToString(args[1]);
            receiverQuota[imageTitle][viewer]--;
            msgBody = "1";
            break;
        }
        case ANSWER_QUOTA_REQUEST:
        {
            string response = VectorToString(args[0]);
            string sender = VectorToString(args[1]);
            string imageName = VectorToString(args[2]);

            if (response == "1")
            {
                cout << "Quota request approved\n";
                int quota = stoll(VectorToString(args[3]));
                //string full_image_name = addUsertoName(imageName, sender);
                string full_image_name = imageName;
                cout << "Full Image name = " << full_image_name << endl;
                setQuotaGrantedImage(full_image_name, quota);
            }
            else if (response == "0")
            {
                cout << "Quota request denied\n";
            }
            else
            {
                cout << "Undefined paramater in answer quota request\n";
            }
            msgBody = "1"; // Request received
            break;
        }
        case GET_USER_PREVIEWS:
        {
            msgBody = getMyImages();
            break;
        }
        case RETRIEVE_PREVIOUSLY_SENT:
        {
            string otherpeer = VectorToString(args[0]);
            vector<Image> imgs = getImagesOwnedBy(otherpeer);
            msgBody = VectorToString(flattenImages(imgs));
            break;

        }
        default:
            cerr << "Peer: Unsupported Operation!\n";
            cout << op << endl;
            break;
        }
    }

    response->setMessageType(Reply);
    response->setMessage(stringToCharPtr(msgBody), msgBody.size());
    return response;
}

vector<Image> Peer::getImagesOwnedBy(string otherpeer)
{
    vector<Image> imgs;
    vector<string> usernames = scrap_dir(GrantedImages);
    for(string title: usernames)
    {
        string path = GrantedImages + title;
        string uname = removeUserfromName(title);
        if(uname == otherpeer)
        {
            int sz;
            imgs.push_back(Image(Image::readImage(path, sz)));
        }
    }
    return imgs;
}

void Peer::setQuotaGrantedImage(string imageName, int newQuota)
{
    int sz;
    string codifiedPath = GrantedImages + imageName;
    vector<uint8_t> cod = Image::readImage(codifiedPath, sz);

    Image img = Image(cod);
    img = Image(DEF_IMG(sz), img.getContent(), img.getOwner(), newQuota);

    // Write back codified image
    Image::writeImage(codifiedPath, img.getCodified());
}

void Peer::clearTempImages()
{
    vector<string> filenames = scrap_dir(PREVIEWS);
    for (string name : filenames)
    {
        deleteFile(PREVIEWS + name);
    }
}

Image Peer::loadMyImage(string title, int quota)
{
    int sz;
    string path = MyImages + title;

    if (!isdigit(title.back()) && !isalnum(title.back()))
    {
        cerr << "Hey, title is invalid!!\n";
    }
    // Pop_back???
    Image img = Image(DEF_IMG(sz), Image::readImage(path, sz), username, quota);

    return img;
}

string Peer::getMyImages()
{
    string titles = "";
    QDir directory(MyImages.c_str());
    vector<Image> previews;
    QStringList images = directory.entryList(QStringList(), QDir::Files);
    foreach (QString filename, images)
    {
        string title = filename.toUtf8().constData();
        string dir = MyImages + title;
        int size;
        Image preview(DEF_IMG(size), Image::readImage(dir, size), username, 0);
        preview.setTitle(title);
        previews.push_back(preview);
    }
    cout << "Titles = " << titles << endl;
    return VectorToString(flattenImages(previews));
}

void Peer::uploadLocalImage(string path)
{
    int sz;
    vector<uint8_t> vec = Image::readImage(path, sz);
    Image myimg = Image(DEF_IMG(sz), vec, username, 0);

    string imageName = path.substr(path.find_last_of('/'));
    string myPath = MyImages + imageName;
    Image::writeImage(path, myimg.getContent());
}

void Peer::loadReceiverQuota()
{
    ifstream in(Quota_db);
    string line;
    while (getline(in, line))
    {
        stringstream st(line);
        string title, receiver;
        string entry;
        int quota;
        in >> title;
        while (getline(st, entry, '$'))
        {
            in >> receiver >> quota;
            receiverQuota[title][receiver] = quota;
        }
    }
    in.close();
}

void Peer::writeBackQuotaDB()
{
    ofstream out(Quota_db);
    string line;
    for (auto it : receiverQuota)
    {
        out << it.first << ' ';
        for (auto p : it.second)
        {
            out << p.first << ',' << p.second << '$';
        }
        out << endl;
    }
    out.close();
}

Peer::~Peer()
{
    this->listenerActive = false;
}
