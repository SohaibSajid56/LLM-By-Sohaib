// memory_engine.cpp
#include "memory_engine.h"
#include <iostream>
using namespace std;

MemoryEngine::MemoryEngine()
    : btree(2),
      hashtable(100)
{
    loadFromDisk();
}

string normalizePrompt(string s)
{
    // 1. lowercase
    transform(s.begin(), s.end(), s.begin(),
              [](unsigned char c)
              { return tolower(c); });

    // 2. remove punctuation
    s.erase(remove_if(s.begin(), s.end(),
                      [](unsigned char c)
                      { return ispunct(c); }),
            s.end());

    // 3. replace common phrases
    vector<string> patterns = {
        "i am",
        "im",
        "its",
        "its",
        "my name is",
        "this is",
    };

    for (const auto &p : patterns)
    {
        size_t pos;
        while ((pos = s.find(p)) != string::npos)
        {
            s.erase(pos, p.length());
        }
    }

    // 4. remove extra spaces
    string out;
    bool prevSpace = false;

    for (char c : s)
    {
        if (isspace(c))
        {
            if (!prevSpace)
                out += c;
            prevSpace = true;
        }
        else
        {
            out += c;
            prevSpace = false;
        }
    }

    // trim leading/trailing space
    if (!out.empty() && out.front() == ' ')
        out.erase(out.begin());
    if (!out.empty() && out.back() == ' ')
        out.pop_back();

    return out;
}

bool MemoryEngine::get(const string &key, string &value, string &source)
{
    // Level 1: HashTable (fast cache)
    string norm = normalizePrompt(key);

    if (hashtable.search(norm, value))
    {
        source = "hashtable";
        return true;
    }

    // Level 2: B-Tree (cold memory)
    if (btree.search(norm, value))
    {
        source = "btree";
        // Promote to hash table
        hashtable.insert(norm, value);
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
    string norm = normalizePrompt(key);

    hashtable.insert(norm, value);
    btree.insert(norm, value);
    saveToDisk(norm, value);
}

void MemoryEngine::loadFromDisk()
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cout << "[WARN] Could not open memory file: " << filename << endl;
        return;
    }

    cout << "[INFO] Loading memory from disk..." << endl;

    string line;
    while (getline(file, line))
    {
        size_t sep = line.find('|');
        if (sep == string::npos)
            continue;

        string key = line.substr(0, sep);
        string norm = normalizePrompt(key);
        string value = line.substr(sep + 1);

        // ONLY load into B-Tree
        btree.insert(norm, value);
    }
}

void MemoryEngine::saveToDisk(const string &key, const string &value)
{
    ofstream file(filename, ios::app);
    file << key << "|" << value << "\n";
}
