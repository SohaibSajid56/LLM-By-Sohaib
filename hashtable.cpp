// hashtable.cpp
#include "hashtable.h"

HashTable::HashTable(int s) : size(s)
{
    table.resize(size, nullptr);
}

int HashTable::hash(const string &key) const
{
    unsigned long h = 0;
    for (char c : key)
        h = h * 31 + c;
    return h % size;
}

vector<pair<string, string>> HashTable::getAll() const
{
    vector<pair<string, string>> result;

    for (auto bucket : table)
    {
        HashNode *curr = bucket;
        while (curr)
        {
            result.push_back({curr->key, curr->value});
            curr = curr->next;
        }
    }
    return result;
}

void HashTable::insert(const string &key, const string &value)
{
    int idx = hash(key);

    HashNode *node = table[idx];
    while (node)
    {
        if (node->key == key)
        {
            node->value = value;
            return;
        }
        node = node->next;
    }

    HashNode *newNode = new HashNode(key, value);
    newNode->next = table[idx];
    table[idx] = newNode;
}

bool HashTable::search(const string &key, string &outValue) const
{
    int idx = hash(key);
    HashNode *node = table[idx];

    while (node)
    {
        if (node->key == key)
        {
            outValue = node->value;
            return true;
        }
        node = node->next;
    }
    return false;
}
