// memory_engine.cpp
#include "memory_engine.h"

MemoryEngine::MemoryEngine()
    : btree(2),
      hashtable(100)
{
    loadFromDisk();
}

bool MemoryEngine::get(const string &prompt, string &outResponse) const
{
    if (hashtable.search(prompt, outResponse))
    {
        return true;
    }

    return false;
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
    while (std::getline(file, line))
    {
        size_t sep = line.find('|');
        if (sep == string::npos)
            continue;

        string key = line.substr(0, sep);
        string value = line.substr(sep + 1);

        hashtable.insert(key, value);
        btree.insert(key, value);
    }
}

void MemoryEngine::saveToDisk(const string &key, const string &value)
{
    ofstream file(filename, ios::app);
    file << key << "|" << value << "\n";
}
