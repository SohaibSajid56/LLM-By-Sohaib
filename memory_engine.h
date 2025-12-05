// memory_engine.h
#pragma once

#include <string>
#include "btree.h"
#include "hashtable.h"

// Simple key-value memory engine using B-Tree + HashTable
class MemoryEngine
{
private:
    BTree btree;
    HashTable hashtable;

public:
    MemoryEngine();

    // Returns true if prompt exists, fills outResponse with stored answer
    bool get(const std::string &prompt, std::string &outResponse) const;

    // Stores prompt -> response in both B-Tree (for index) and HashTable (for lookup)
    void put(const std::string &prompt, const std::string &response);
};
