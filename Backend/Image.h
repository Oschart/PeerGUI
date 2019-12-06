#ifndef IMAGE_H
#define IMAGE_H

#include <vector>
#include <fstream>
using namespace std;

class Image {

private:
    vector<uint8_t> content;
    vector<uint8_t> codified;
    string owner;
    string title;
    int quota;
public:
    Image(vector<uint8_t> defImage, vector<uint8_t> _content, string _owner, int _quota) :
        codified(defImage), content(_content), owner(_owner), quota(_quota){
        codified.erase(codified.end()-1);
        codified.push_back(0xFE);
        for (int i = 0 ; i < owner.length(); i++){
            codified.push_back(uint8_t(owner[i]));
        }
        codified.push_back(uint8_t('$'));
        for (int i = 0 ; i < content.size(); i++){
            codified.push_back(content[i]);
        }
        for (int i = 0;  i < 4 ; i++){
            codified.push_back(_quota%256);
            _quota /= 256;
        }
        codified.push_back(0xFF);
        codified.push_back(0xD9);
    }

    Image(vector<uint8_t> _codified) : codified(_codified) {
        int status = 0;
        content.clear();
        owner = "";
        for (int i = 0 ; i < codified.size()-2 ; i++){
            if (!status && codified[i] == 0xFF && codified[i+1] == 0xFE) {
                status = 1;
                ++i;
            } else if (status == 1) {
                if (codified[i] == uint8_t('$')) status = 2;
                else {
                    owner += char(codified[i]);
                }
            }
            else if (status == 2) {
                content.push_back(codified[i]);
            }
        }
        quota = 0;
        for (int i = 3 ; i >= 0 ; i--){
            quota += (1 << 8*i)*content[content.size()-1];
            content.erase(content.end()-1);
        }
    }

    Image(vector<uint8_t> _content, string _owner) :
        content(_content), owner(_owner) { }

    vector<uint8_t> getContent() { return content; }
    vector<uint8_t> getCodified() { return codified; }
    string getOwner() { return owner; }
    int getQuota() { return quota; }

    static char* vectorToCharPtr (vector<uint8_t> v) {
        char * c = new char[v.size()];
        for (int i = 0 ; i < v.size(); i++) c[i] = char(v[i]);
        return c;
    }

    static vector<uint8_t> charPtrToVector (char * c, int len) {
        vector<uint8_t> v(len);
        for (int i = 0 ; i < len ; i++) v[i] = (unsigned char)(c[i]);
        return v;
    }

    static vector<uint8_t> stringToVector (string str) {
        vector<uint8_t> v(str.length());
        for (int i = 0 ; i < str.length() ; i++) v[i] = (unsigned char)(str[i]);
        return v;
    }

    static vector<uint8_t> readImage (string fileName, int& sz) {

        ifstream input(fileName, ios::in | ios::binary | ios::ate);
        if (!input) {
            cerr << "Cannot open file " << fileName << endl;
            input.close();
            return vector<uint8_t>();
        }
        else {
            input.seekg (0, input.end);
            sz = input.tellg();
            input.seekg (0, input.beg);
            char * ans = new char[sz];
            input.read(ans, sz);
            input.close();
            auto v = charPtrToVector(ans, sz);
            delete [] ans;
            return v;
        }
    }

    

    static void writeImage( string fileName, vector<uint8_t> bytes ) {
        ofstream output( fileName, ios::out | ios :: binary);
        if (!output) {
            cerr << "Cannot open file " << fileName << endl;
            output.close();
            return;
        }
        char * c = Image::vectorToCharPtr(bytes);
        output.write(c, bytes.size());
        delete [] c;
        return;
    }

    void setTitle(string _title)
    {
        title = _title;
    }

    string getTitle()
    {
        return title;
    }

    size_t getSize()
    {
        return codified.size();
    }

};



#endif // IMAGE_H
