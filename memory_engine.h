// memory_engine.h
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
    std::string handlePrompt(const std::string &prompt);
};
