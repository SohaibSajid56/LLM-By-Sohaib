#pragma once

#include <string>
#include "btree.h"
#include "hashtable.h"
using namespace std;

class MemoryEngine
{
private:
    BTree btree;
    HashTable hashtable;

public:
    MemoryEngine();

    bool get(const string &prompt, string &outResponse) const;

    void put(const string &prompt, const string &response);
};
