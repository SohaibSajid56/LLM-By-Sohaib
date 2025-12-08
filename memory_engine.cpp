// memory_engine.cpp
#include "memory_engine.h"

MemoryEngine::MemoryEngine()
    : btree(2),
      hashtable(100)
{
}

bool MemoryEngine::get(const string &prompt, string &outResponse) const
{
    if (hashtable.search(prompt, outResponse))
    {
        return true;
    }

    return false;
}

void MemoryEngine::put(const string &prompt, const string &response)
{
    if (!btree.search(prompt))
    {
        btree.insert(prompt);
    }
    hashtable.insert(prompt, response);
}
