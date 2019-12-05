Peer::Peer(char *_myHostname, int _myPort, char *_shostname, int _sport) : Server(_myHostname, _myPort)
{
    brokerIP = _shostname;
    brokerPort = _sport;
    this->udpSocket = new UDPSocket();
    listenerActive = 1;
    thread t(&Peer::listenerRun, this);
    t.detach();
    this->defaultImage = "./SPACE.JPG";
    this->sessionToken = "9";
    rpcID = 10;

    //    createFolder("./previews");
    argCount[REQUEST_IMAGE] = 3;
    argCount[REQUEST_QUOTA] = 3;
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
    while ((cv.wait_for(lck, chrono::seconds(1)) == cv_status::timeout) && trials--)
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

int Peer::login(string username, string password){
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
            cout << "Authentiaction failed\n"; result = 0;
        }
        else
        {
            cout << "Authentication granted, token = " << content << endl;
            this->username = username;
            this->sessionToken = content;
            createFolder(MyData);
            createFolder(GrantedImages);
            createFolder(MyImages);
            createFolder(PREVIEWS);

            cacheDB(MyImages_db, myImageTitles);
            cacheDB(GrantedImages_db, grantedImagesTitles);
            cacheDB(Previews_db, previewsTitles);

            result = 1;
        }

        delete received;
    }
    else
    { //the waiting thread timed out
        cout << "Login operation timed out!\n"; result = -1;
    }
    cout << "Token = " << this->sessionToken << endl;
    delete toBeSent;
    return result;
}

int Peer::signup(string username, string password){
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
            {cout << "Registration failed, invalid username\n"; result = 0;}
        else if (content == "1")
            {cout << "Registration succeeded\n"; result = 1;}
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
                return make_pair(unsigned(stoi(args[0])), unsigned(stoi(args[1])));
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

void Peer::getPreviews()
{
    this->udpSocket->initializeClient(brokerIP, brokerPort);
    string args = this->sessionToken;
    Message *toBeSent = new Message(GET_PREVIEW_FEED, stringToCharPtr(args), args.length(), (this->rpcID)++);
    toBeSent->setMessageType(Request);
    if (this->execute(toBeSent))
    {
        int rpc_id = toBeSent->getRPCId();
        Message *received = this->rpcToMsg[rpc_id];
        this->rpcToMsg.erase(rpc_id);
        cout << "PREVIEWS RECEIVED\n";
        vector<uint8_t> flatArgs = Image::charPtrToVector((char *)received->getMessage(), received->getMessageSize());

        vector<Image> previews = extractImages(flatArgs);

        for (int i = 0; i < previews.size(); i++)
        {
            string imageTitle = previews[i].getTitle();
            //imageTitle.pop_back();
            string path = PREVIEWS + string("/") + imageTitle;
            this->imageToPeer[imageTitle] = previews[i].getOwner();
            Image::writeImage(path, previews[i].getContent());

            string storedImageTitle = imageTitle + string("_") + previews[i].getOwner();
            appendFileAndCache(Previews_db, previewsTitles, storedImageTitle);
        }

        delete received;
    }
    else
    { //the waiting thread timed out
        cout << "Get Previews operation timed out!\n";
    }
    delete toBeSent;
}

void Peer::getUserPreviews(string otherpeer)
{

    this->udpSocket->initializeClient(BROKER_IP, BROKER_PORT);

    string args = this->sessionToken + separator + otherpeer;
    Message *toBeSent = new Message(GET_USER_PREVIEW, stringToCharPtr(args), args.length(), (this->rpcID)++);
    toBeSent->setMessageType(Request);
    if (this->execute(toBeSent))
    {
        int rpc_id = toBeSent->getRPCId();
        Message *received = this->rpcToMsg[rpc_id];
        this->rpcToMsg.erase(rpc_id);
        cout << "USER PREVIEWS RECEIVED\n";
        vector<uint8_t> flatArgs = Image::charPtrToVector((char *)received->getMessage(), received->getMessageSize());

        vector<Image> previews = extractImages(flatArgs);

        for (int i = 0; i < previews.size(); i++)
        {
            string imageTitle = previews[i].getTitle();
            //imageTitle.pop_back();
            string path = PREVIEWS + string("/") + imageTitle;
            this->imageToPeer[imageTitle] = previews[i].getOwner();
            Image::writeImage(path, previews[i].getContent());
        }

        delete received;
    }
    else
    {
        cout << "Get User Previews operation timed out!\n";
    }
}

void Peer::getUserTitles(string otherpeer)
{
    pair<IP, Port> peerToAddress = this->getAddress(otherpeer);
    if (peerToAddress == make_pair(0U, 0U))
    {
        cout << "There's no peer record with such name in the broker\n";
        return;
    }
    cout << "Peer Address was received successfully, getting titles...\n";
    this->udpSocket->initializeClient(peerToAddress.first, peerToAddress.second);

    string args = this->username;
    Message *toBeSent = new Message(GET_USER_TITLES, stringToCharPtr(args), args.length(), (this->rpcID)++);
    toBeSent->setMessageType(Request);
    if (this->execute(toBeSent))
    {
        int rpc_id = toBeSent->getRPCId();
        Message *received = this->rpcToMsg[rpc_id];
        this->rpcToMsg.erase(rpc_id);
        cout << "USER TITLES RECEIVED: \n";

        string content = string((char *)received->getMessage(), received->getMessageSize());
        vector<string> args = extractArgs(content);

        for (int i = 0; i < args.size(); i++)
            cout << args[i] << endl;

        delete received;
    }
    else
    {
        cout << "Get User Previews operation timed out!\n";
    }
}

void Peer::uploadImagePreview(string imageName, string imagePath)
{
    int sz1, sz2;
    Image toSend = Image(Image::readImage(this->defaultImage, sz1), Image::readImage(imagePath, sz2), this->username, 0);
    Image img2(toSend.getCodified());

    toSend.setTitle(imageName);
    vector<Image> images;
    images.push_back(toSend);
    vector<uint8_t> flattenedV = flattenImages(images);
    string flattenedP(flattenedV.begin(), flattenedV.end());
    string args = this->sessionToken + separator + flattenedP;

    Message *toBeSent = new Message(UPLOAD_PREVIEW, stringToCharPtr(args), args.length(), (this->rpcID)++);
    toBeSent->setMessageType(Request);
    if (this->execute(toBeSent))
    {
        int rpc_id = toBeSent->getRPCId();
        Message *received = this->rpcToMsg[rpc_id];
        this->rpcToMsg.erase(rpc_id);
        string content = string((char *)received->getMessage(), received->getMessageSize());
        if (content == "1")
        {
            cout << "Upload Succeeded\n";
        }
        else if (content == "0")
        {
            cout << "Upload Failed\n";
        }
        else
        {
            cout << "Invalid Content\n";
        }
        delete received;
    }
    else
    {
        cout << "Upload Previews operation timed out!\n";
    }
}

void Peer::requestImage(string otherpeer, string imageName, int quota)
{
    pair<IP, Port> peerToAddress = this->getAddress(otherpeer);
    if (peerToAddress == make_pair(0U, 0U))
    {
        cout << "There's no peer record with such name in the broker\n";
        return;
    }
    cout << "Peer Address was received successfully, requesting image...\n";
    this->udpSocket->initializeClient(peerToAddress.first, peerToAddress.second);

    string args = username + separator + imageName + separator + to_string(quota);
    Message *toBeSent = new Message(REQUEST_IMAGE, stringToCharPtr(args), args.length(), (this->rpcID)++);
    toBeSent->setMessageType(Request);
    if (this->execute(toBeSent))
    {
        int rpc_id = toBeSent->getRPCId();
        Message *received = this->rpcToMsg[rpc_id];
        this->rpcToMsg.erase(rpc_id);
        string content = string((char *)received->getMessage(), received->getMessageSize());

        if (content == "0")
        {
            cout << "Request Image denied\n";
        }
        else
        {
            string path = GrantedImages + string("/") + imageName + CODED;
            vector<uint8_t> payload = Image::stringToVector(content);
            cout << "Payload : " << payload.size() << endl;
            Image img = extractImages(payload)[0];
            cout << "Image File size = " << img.getSize() << endl;
            Image::writeImage(path, img.getCodified());

            appendFileAndCache(GrantedImages_db, grantedImagesTitles, imageName);
        }

        delete received;
    }
    else
    {
        cout << "Request Image operation timed out!\n";
    }
}

void Peer::requestImageQuota(string otherpeer, string imageName, int quota)
{
    pair<IP, Port> peerToAddress = this->getAddress(otherpeer);
    if (peerToAddress == make_pair(0U, 0U))
    {
        cout << "There's no peer record with such name in the broker\n";
        return;
    }
    cout << "Peer Address was received successfully, requesting image quota...\n";
    this->udpSocket->initializeClient(peerToAddress.first, peerToAddress.second);

    string args = imageName + separator + to_string(quota);
    Message *toBeSent = new Message(REQUEST_QUOTA, stringToCharPtr(args), args.length(), (this->rpcID)++);

    toBeSent->setMessageType(Request);
    if (this->execute(toBeSent))
    {
        int rpc_id = toBeSent->getRPCId();
        Message *received = this->rpcToMsg[rpc_id];
        this->rpcToMsg.erase(rpc_id);
        string content = string((char *)received->getMessage(), received->getMessageSize());
        if (content == "0")
        {
            cout << "Request Quota refused\n";
        }
        else
        {
            int sz;
            string path = GrantedImages + string("/") + imageName + CODED;
            Image img = Image(Image::readImage(path, sz));
            string owner = img.getOwner();
            img = Image(DEF_IMG(sz), img.getContent(), owner, quota);
            Image::writeImage(path, img.getCodified());
        }

        delete received;
    }
    else
    {
        cout << "Request Image Quota operation timed out!\n";
    }
}

void Peer::setImageQuota(string otherpeer, string imageName, int quota)
{
    pair<IP, Port> peerToAddress = this->getAddress(otherpeer);
    if (peerToAddress == make_pair(0U, 0U))
    {
        cout << "There's no peer record with such name in the broker\n";
        return;
    }

    cout << "Peer Address was received successfully, setting image quota...\n";
    this->udpSocket->initializeClient(peerToAddress.first, peerToAddress.second);

    string args = otherpeer + separator + imageName + separator + to_string(quota);
    Message *toBeSent = new Message(SET_QUOTA, stringToCharPtr(args), args.length(), (this->rpcID)++);
    toBeSent->setMessageType(Request);
    if (this->execute(toBeSent))
    {
        int rpc_id = toBeSent->getRPCId();
        Message *received = this->rpcToMsg[rpc_id];
        this->rpcToMsg.erase(rpc_id);
        string content = string((char *)received->getMessage(), received->getMessageSize());

        if (content == "0")
        {
            cout << "Setting image quota failed\n";
        }
        else if (content == "1")
        {
            cout << "Setting image quota succeeded\n";
        }
        else
        {
            cout << "Invalid content in the reply\n";
        }

        delete received;
    }
    else
    {
        cout << "Setting Image Quota operation timed out!\n";
    }
}

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
            string other_username = VectorToString(args[0]);
            string title = VectorToString(args[1]);
            filter(title);
            int quota = stoi(VectorToString(args[2]));
            vector<Image> img;
            img.push_back(loadMyImage(title, quota));
            msgBody = VectorToString(flattenImages(img));
            //SaveQuotaRecord(other_username, title, quota);
            break;
        }
        case REQUEST_QUOTA:
        {
            // TODO: ask user for permission
            msgBody = "1";
            break;
        }
        case SET_QUOTA:
        {
            
            break;
        }
        case GET_USER_TITLES:
        {
            msgBody = getMyTitles();
            break;
        }
        default:
            cerr << "Peer: Unsupported Operation!\n";
            break;
        }
    }

    response->setMessageType(Reply);
    response->setMessage(stringToCharPtr(msgBody), msgBody.size());
    return response;
}

Image Peer::loadMyImage(string title, int quota)
{
    int sz;
    string path = MyImages + string("/") + title;

    if (!isdigit(title.back()) && !isalnum(title.back()))
    {
        cerr << "Hey, title is invalid!!\n";
    }
    // Pop_back???
    Image img = Image(DEF_IMG(sz), Image::readImage(path, sz), username, quota);

    return img;
}

string Peer::getMyTitles()
{
    string res = "";

    for (auto title : myImageTitles)
    {
        res += title + ",";
    }
}

void Peer::uploadLocalImage(string path)
{
    int sz;
    vector<uint8_t> vec = Image::readImage(path, sz);
    Image myimg = Image(DEF_IMG(sz), vec, username, 0);

    string imageName = path.substr(path.find_last_of('/'));
    string myPath = MyImages + string("/") + imageName;
    Image::writeImage(path, myimg.getContent());

    appendFileAndCache(MyImages_db, myImageTitles, imageName);
}

void cacheDB(string path, set<string> &cache)
{
    ifstream in(path);
    string entry;
    while (getline(in, entry))
    {
        cache.insert(entry);
    }
}

void appendFileAndCache(string path, set<string> &cache, string entry)
{
    if (cache.find(entry) == cache.end())
    {
        ofstream out(path, ios_base::app);
        out << entry << endl;

        cache.insert(entry);
        out.close();
    }
}

Peer::~Peer()
{
    this->listenerActive = false;
}
