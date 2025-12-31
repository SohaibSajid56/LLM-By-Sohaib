#pragma once

#include <string>
#include "btree.h"
#include "hashtable.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype> // tolower, ispunct, isspace
#include <vector>
using namespace std;

string normalizePrompt(string s);

class MemoryEngine
{
private:
    BTree btree;
    HashTable hashtable;
    string filename = "/data/memory.txt";

public:
    MemoryEngine();

    bool get(const string &key, string &value, string &source);
    void put(const string &key, const string &value);

    vector<pair<string, string>> getHashTableSnapshot() const;

    void loadFromDisk();
    void saveToDisk(const string &key, const string &value);
};
