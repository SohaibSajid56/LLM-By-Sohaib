// hashtable.cpp
#include "hashtable.h"

HashTable::HashTable(int s) : size(s)
{
    table.resize(size, nullptr);
}

int HashTable::hash(const std::string &key) const
{
    unsigned long h = 0;
    for (char c : key)
        h = h * 31 + c;
    return h % size;
}

void HashTable::insert(const std::string &key, const std::string &value)
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

bool HashTable::search(const std::string &key, std::string &outValue) const
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
