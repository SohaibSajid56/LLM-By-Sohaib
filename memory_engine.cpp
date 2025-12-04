// memory_engine.cpp
#include "memory_engine.h"

MemoryEngine::MemoryEngine() : btree(2), hashtable(100) {}

std::string MemoryEngine::handlePrompt(const std::string &prompt)
{
    std::string response;

    // Step 1: Search in Hash Table (fast lookup)
    if (hashtable.search(prompt, response))
    {
        return "FOUND (HashTable): " + response;
    }

    // Step 2: Search in B-Tree
    if (btree.search(prompt))
    {
        return "FOUND (B-Tree but no stored response).";
    }

    // Step 3: Not found → store
    btree.insert(prompt);

    std::string newResponse = "Stored new prompt: " + prompt;
    hashtable.insert(prompt, newResponse);

    return "NOT FOUND. Now saved.";
}
