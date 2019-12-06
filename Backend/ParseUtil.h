#ifndef PARSEUTIL_H
#define PARSEUTIL_H
#include <iostream>
#include <string>
#include <sstream>
#include <string.h>
#include <vector>
#include <chrono>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include "Image.h"


void trim(string &s);

string inQuotes(string s);


char *stringToCharPtr(string s);


string CharPtrToString(char* str);

string VectorToString(vector<uint8_t> vec);

string getNext(vector<uint8_t> &data, int &i);

vector<string> extractArgs(string s);

vector<vector<uint8_t>> extractArgsUntil(vector<uint8_t> &flat, int num);


string flatten(vector<string> args);

string addUsertoName(string imageName, string user);


vector<Image> extractImages(vector<uint8_t> &data);



vector<uint8_t> flattenImages(vector<Image> imgs);

double sinceThen(chrono::system_clock::time_point then);
bool createFolder(string pathname);
bool deleteFile(string pathname);


void saveDBRecord(string path, string key, string data);


void filter(string &s);

#endif
