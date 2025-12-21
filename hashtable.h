// hashtable.h
#pragma once
#include <string>
#include <vector>
using namespace std;

struct HashNode
{
    string key;
    string value;
    HashNode *next;

    HashNode(string k, string v) : key(k), value(v), next(nullptr) {}
};

class HashTable
{
private:
    vector<HashNode *> table;
    int size;

    int hash(const string &key) const;

public:
    HashTable(int s = 100);
    vector<pair<string, string>> getAll() const;
    void insert(const string &key, const string &value);
    bool search(const string &key, string &outValue) const;
};
