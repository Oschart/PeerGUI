
#include "Broker.h"
#include <iostream>
#include <fstream>
#include <streambuf>
#include <set>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdexcept>

using namespace std;

Broker::Broker(char *_listen_hostname, int _listen_port) : Server(_listen_hostname, _listen_port)
{

    createFolder(user_dir);
    loadAuthDB();
    loadPreviewDB();
    tokenCount = 1;

    argCount[LOGIN] = 2;
    argCount[REGISTER] = 2;
    argCount[GET_IP] = 2;
    argCount[GET_PREVIEW_FEED] = 1;
    argCount[GET_USER_PREVIEW] = 2;
    argCount[UPLOAD_PREVIEW] = 3;
}


void Broker::loadAuthDB()
{
    ifstream in(USER_DB);
    string row;
    while (getline(in, row))
    {
        stringstream st(row);
        string uname, password;
        getline(st, uname, ':');
        trim(uname);
        getline(st, password);
        password = inQuotes(password);
        // ***************
        userPassword[uname] = password;
        users.push_back(uname);
        createFolder(user_dir + string("/") + uname);
    }
}

void Broker::loadPreviewDB()
{
    ifstream in(PREVIEW_DB);
    string row;
    while (getline(in, row))
    {
        stringstream st(row);
        string uname, title;
        getline(st, uname, ':');
        trim(uname);
        getline(st, title);
        //title = inQuotes(title);
        // ***************
        userPreviewTitles[uname].push_back(title);
    }
}

bool Broker::validToken(Token token)
{
    return tokenUser.find(token) != tokenUser.end();
}



// Note: this method recycles the received message, it doesn't deep copy it
Message *Broker::doOperation(Message *_received, IP user_ip, Port user_port)
{
    Message *response = _received;
    opType op = opType(_received->getOperation());
    vector<uint8_t> flatArgs = Image::charPtrToVector((char *)_received->getMessage(), _received->getMessageSize());
    vector<vector<uint8_t>> args = extractArgsUntil(flatArgs, argCount[op]);
    vector<uint8_t> payload = flatArgs;
    payload.pop_back();
    string msgBody = "0"; // Default reply (invalid operation)
    Token token;
    bool valid = true;
    /*for (auto arg : args)
    {
        if (arg.empty())
        {
            valid = false;
            break;
        }
    }*/

    if (op != REGISTER && op != LOGIN)
    {
        try
        {
            token = stoi(VectorToString(args[0]));
        }
        catch (exception &e)
        {
            cerr << "Invalid Token\n";
            valid = false;
        }
        valid = validToken(token);
    }
    else
        valid = true;

    if (valid)
    {

        switch (op)
        {
        case LOGIN:
        { // Login Request
            string username = VectorToString(args[0]);
            string password = VectorToString(args[1]);
            msgBody = to_string(Login(username, password));
            token = userToken[username];
            break;
        }
        case REGISTER:
        { // Register Request
            string username = VectorToString(args[0]);
            string password = VectorToString(args[1]);
            msgBody = to_string(Register(username, password));
            break;
        }
        case GET_IP:
        { // Get peer ip
            string other_username = VectorToString(args[1]);
            msgBody = getUserAddr(other_username).toString();
            break;
        }
        case GET_PREVIEW_FEED:
        { // Get preview feed
            msgBody = VectorToString(flattenImages(getPreviewFeed()));
            break;
        }
        case GET_USER_PREVIEW:
        { // Get all previews of a user
            string other_username = VectorToString(args[1]);
            msgBody = VectorToString(flattenImages(getUserPreviews(other_username)));
            break;
        }
        case UPLOAD_PREVIEW:
        { // Upload a preview
            int sz;
            string title = VectorToString(args[1]);
            filter(title);
            Image img = Image(payload);
            uploadPreview(tokenUser[token], title, img);
            msgBody = "1";
            break;
        }
        case GET_USER_TITLES_CALLBACK:
        { // Upload a preview
            int sz;
            
            break;
        }
        case REQUEST_IMAGE_CALLBACK:
        { // Upload a preview
            int sz;
            string title = VectorToString(args[1]);
            filter(title);
            Image img = Image(payload);
            uploadPreview(tokenUser[token], title, img);
            msgBody = "1";
            break;
        }
        case REQUEST_QUOTA_CALLBACK:
        { // Upload a preview
            int sz;
            string title = VectorToString(args[1]);
            filter(title);
            Image img = Image(payload);
            uploadPreview(tokenUser[token], title, img);
            msgBody = "1";
            break;
        }
        default:
            cerr << "Unsupported Operation!\n";
            break;
        }

        if (op != REGISTER)
        {
            updateLastActive(token);
            string username = tokenUser[token];
            userAddrMutex.lock();
            userAddr[username] = Address(user_ip, user_port);
            userAddrMutex.unlock();
        }
    }

    response->setMessageType(Reply);
    response->setMessage(stringToCharPtr(msgBody), msgBody.size());
    return response;
}

int Broker::updateLastActive(Token token)
{
    string uname = tokenUser[token];
    lock_guard<mutex> lock(lastActiveMutex);
    lastActive[uname] = chrono::system_clock::now();
    return 1;
}

Token Broker::Login(string username, string password)
{
    lock_guard<mutex> lock(userTokenMutex);
    if (tokenCount == 0)
    {
        cerr << "Too many tokens are generated!\n";
        tokenCount = 1;
    }
    if (userPassword.count(username) > 0 && userPassword[username] == password)
    {
        tokenUser.erase(userToken[username]); // Erase the old token (if any)
        userToken[username] = tokenCount;
        tokenUser[tokenCount] = username;
        return tokenCount++;
    }
    else
    {
        return 0; // Invalid username or password
    }
}

bool validUsername(string username)
{
    string invalid_chars = "$ &*";
    size_t find = username.find_first_of(invalid_chars);

    return !(username.empty() || find != string::npos);
}

int Broker::Register(string username, string password)
{
    lock_guard<mutex> lock(userPasswordMutex);

    if (validUsername(username) && userPassword.count(username) == 0)
    {
        userPassword[username] = password;
        ofstream out(USER_DB, ios_base::app);
        out << username << " : \'" << password << "\'" << endl;

        createFolder(user_dir + string("/") + username);
        return 1;
    }
    else
    {
        return 0; // Username already exists or invalid
    }
}

Address Broker::getUserAddr(string username)
{
    return userAddr[username];
}

int Broker::uploadPreview(string username, string title, Image preview)
{
    string dir = user_dir + string("/") + username + string("/") + title;

    Image::writeImage(dir, preview.getContent());

    userPreviewTitles[username].push_back(title);
    ofstream out;
    out.open(PREVIEW_DB, ofstream::app);
    out << username << ":" << title << endl;
    out.close();
    return 1;
}

vector<Image> Broker::getUserPreviews(string username)
{
    vector<Image> previews;

    vector<string> &imageFiles = userPreviewTitles[username];
    for (int i = 0; i < imageFiles.size(); ++i)
    {
        string title = imageFiles[i];
        string dir = user_dir + string("/") + username + string("/") + title;
        int size;
        vector<uint8_t> content(Image::readImage(dir, size));
        Image preview = Image(content, username);
        previews.push_back(preview);
    }

    return previews;
}

vector<Image> Broker::getPreviewFeed()
{
    vector<Image> previews;
    int prev_cnt = 0;

    for (auto it : userPreviewTitles)
    {

        for (int i = 0; i < min(int(it.second.size()), PREVIEWS_PER_USER); ++i)
        {
            // TOADJUST
            string title = it.second[i];
            filter(title);
            string username = it.first;
            string dir = user_dir + string("/") + username + string("/") + title;
            int size;
            //dir.pop_back();
            vector<uint8_t> content(Image::readImage(dir, size));
            if (content.size()){
              Image preview = Image(DEF_IMG(size), content, username, 0);
              preview.setTitle(title);
              previews.push_back(preview);
              ++prev_cnt;
            }
        }

        if (prev_cnt >= MAX_PREVIEWS)
            break;
    }

    return previews;
}

Broker::~Broker()
{
}
