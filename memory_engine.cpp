// memory_engine.cpp
#include "memory_engine.h"
using namespace std;

MemoryEngine::MemoryEngine()
    : btree(2),
      hashtable(100)
{
    loadFromDisk();
}

bool MemoryEngine::get(const string &key, string &value, string &source)
{
    // Level 1: HashTable (fast cache)
    if (hashtable.search(key, value))
    {
        source = "hashtable";
        return true;
    }

    // Level 2: B-Tree (cold memory)
    if (btree.search(key, value))
    {
        source = "btree";
        // Promote to hash table
        hashtable.insert(key, value);
        return true;
    }

    return false;
}

vector<pair<string, string>> MemoryEngine::getHashTableSnapshot() const
{
    return hashtable.getAll();
}

void MemoryEngine::put(const string &key, const string &value)
{
    hashtable.insert(key, value);
    btree.insert(key, value);
    saveToDisk(key, value);
}

void MemoryEngine::loadFromDisk()
{
    ifstream file(filename);
    if (!file.is_open())
        return;

    string line;
    while (getline(file, line))
    {
        size_t sep = line.find('|');
        if (sep == string::npos)
            continue;

        string key = line.substr(0, sep);
        string value = line.substr(sep + 1);

        // ONLY load into B-Tree
        btree.insert(key, value);
    }
}

void MemoryEngine::saveToDisk(const string &key, const string &value)
{
    ofstream file(filename, ios::app);
    file << key << "|" << value << "\n";
}
