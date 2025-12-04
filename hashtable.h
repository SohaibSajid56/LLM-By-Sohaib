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

    HashNode(string k, std::string v) : key(k), value(v), next(nullptr) {}
};

class HashTable
{
private:
    std::vector<HashNode *> table;
    int size;

    int hash(const std::string &key) const;

public:
    HashTable(int s = 100);
    void insert(const std::string &key, const std::string &value);
    bool search(const std::string &key, std::string &outValue) const;
};
