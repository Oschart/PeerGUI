#include "Backend/ParseUtil.h"

using namespace std;

void trim(string &s)
{
    for (int i = 0; i < s.length(); ++i)
    {
        if (s[i] == ' ')
        {
            s = s.substr(0, i);
            return;
        }
    }
}

string inQuotes(string s)
{
    size_t ft = s.find_first_of("\'") + 1;
    size_t lt = s.find_last_of("\'");
    if(ft >= lt) return s;
    return s.substr(ft, lt - ft);
}


char *stringToCharPtr(string s)
{
    char *str = new char[s.size()];
    memcpy(str, s.c_str(), s.size());
    return str;
}


string CharPtrToString(char* str)
{
    if(!str) throw invalid_argument("Null Pointer at CharPtrToString!\n");
    return string(str);
}

string VectorToString(vector<uint8_t> vec)
{
    string str(vec.begin(), vec.end());
    return str;
}

string getNext(vector<uint8_t> &data, int &i)
{
    string str = "";
    for(; i < data.size(); ++i)
    {
        if(char(data[i]) == ',')
        {
            ++i;
            return str;
        }
        str += char(data[i]);
    }
    return str;
}

vector<string> extractArgs(string s)
{
    vector<string> args;
    stringstream st(s);
    string arg;
    while (getline(st, arg, ','))
    {
        args.push_back(arg);
    }
    return args;
}

string addUsertoName(string imageName, string user){
    string _user = "_" + user;
    imageName.insert(imageName.find_last_of("."), _user);
    return imageName;
}

string removeUserfromName(string& imageName){
    int lastDot = imageName.find_last_of(".");
    int lastUnderscore = imageName.find_last_of("_");
    string username = imageName.substr(lastUnderscore + 1, lastDot - lastUnderscore - 1);
    imageName.erase(lastUnderscore, lastDot - lastUnderscore);
    return username;
}

vector<vector<uint8_t>> extractArgsUntil(vector<uint8_t> &flat, int num)
{
    vector<vector<uint8_t>> args;
    int i = 0;
    while(num--)
    {
        args.push_back(Image::stringToVector(getNext(flat, i)));
    }

    flat = vector<uint8_t>(flat.begin() + i, flat.end());

    return args;
}


string flatten(vector<string> args)
{
    string flatArgs = "";
    for(int i = 0; i < args.size(); ++i)
    {
        flatArgs += args[i];
        if(i < args.size() - 1) flatArgs += ",";
    }
    return flatArgs;
}


vector<Image> extractImages(vector<uint8_t> &data)
{
    vector<Image> imgs;

    for(int i = 0; i < data.size()-1; ++i)
    {
        string title = getNext(data, i);
        string size_str = getNext(data, i);
        size_t size;

        size = stoi(size_str);
        vector<uint8_t> content(data.begin() + i, data.begin() + i + size);
        Image img = Image(content);
        img.setTitle(title);
        imgs.push_back(img);
        i += size;

    }
    return imgs;
}



vector<uint8_t> flattenImages(vector<Image> imgs)
{
    vector<uint8_t> flat;

    for(auto img : imgs)
    {
        vector<uint8_t> v = Image::stringToVector(img.getTitle());
        flat.insert(flat.end(), v.begin(), v.end());

        flat.push_back(',');

        string size_str = to_string(img.getSize());
        v = Image::stringToVector(size_str);
        flat.insert(flat.end(), v.begin(), v.end());


        flat.push_back(',');

        v = img.getCodified();
        flat.insert(flat.end(), v.begin(), v.end());
        flat.push_back(',');

    }

    return flat;
}

double sinceThen(chrono::system_clock::time_point then){
    chrono::system_clock::time_point now1 = chrono::system_clock::now();
    chrono::duration<double> elapsed = now1 - then;
    return elapsed.count();
}

bool createFolder(string pathname)
{
    //errno == EEXIST
    if (mkdir(pathname.c_str(), 0777) == -1)
    {
        if (errno != EEXIST)
            cerr << "Error :  " << strerror(errno) << endl;
        else
            cout << "Using Existing Folder\n";
        return false;
    }
    else
    {
        cout << "New folder " + pathname + " created.\n";
        return true;
    }
}

bool deleteFile(string pathname)
{
    if( remove( pathname.c_str()) != 0 )
        cout << "Error deleting file " << pathname << "\n";
    else
        cout << "File " << pathname << " Successfully Deleted\n";
}

void saveDBRecord(string path, string key, string data)
{
    ofstream out(path, ios_base::app);
    out << key << ':' << data << endl;
    out.close();
}

void filter(string &s)
{
    while(!isalnum(s.back())) s.pop_back();
}
