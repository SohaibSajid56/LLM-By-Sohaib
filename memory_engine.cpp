// memory_engine.cpp
#include "memory_engine.h"

MemoryEngine::MemoryEngine()
    : btree(2),      // B-Tree min degree
      hashtable(100) // Hash table size
{
}

bool MemoryEngine::get(const std::string &prompt, std::string &outResponse) const
{
    // Use hash table for fast lookup
    if (hashtable.search(prompt, outResponse))
    {
        return true;
    }
    // If you want, you could also check B-Tree only for existence,
    // but main value storage is in HashTable.
    return false;
}

void MemoryEngine::put(const std::string &prompt, const std::string &response)
{
    // Insert key into B-Tree index (if not already present)
    if (!btree.search(prompt))
    {
        btree.insert(prompt);
    }
    // Store full prompt->response mapping in HashTable
    hashtable.insert(prompt, response);
}
