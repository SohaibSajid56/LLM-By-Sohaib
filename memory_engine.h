#pragma once

#include <string>
#include "btree.h"
#include "hashtable.h"
#include <fstream>
#include <sstream>
using namespace std;

class MemoryEngine
{
private:
    BTree btree;
    HashTable hashtable;
    string filename = "memory.txt";

public:
    MemoryEngine();

    bool get(const string &key, string &value) const;
    void put(const string &key, const string &value);

    void loadFromDisk();
    void saveToDisk(const string &key, const string &value);
};
