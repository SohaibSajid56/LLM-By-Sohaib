// btree.cpp
#include "btree.h"
#include <algorithm>
using namespace std;

BTreeNode::BTreeNode(bool isLeaf)
{
    leaf = isLeaf;
}

BTree::BTree(int minDegree)
{
    root = new BTreeNode(true);
    t = minDegree;
}

bool BTree::search(const string &key, string &valueOut) const
{
    BTreeNode *node = root;

    while (true)
    {
        int i = 0;

        while (i < node->entries.size() && key > node->entries[i].first)
            i++;

        if (i < node->entries.size() && node->entries[i].first == key)
        {
            valueOut = node->entries[i].second;
            return true;
        }

        if (node->leaf)
            return false;

        node = node->children[i];
    }
}

void BTree::insert(const string &key, const string &value)
{
    string existingValue;

    if (search(key, existingValue))
        return;

    if (root->entries.size() == 2 * t - 1)
    {
        BTreeNode *newRoot = new BTreeNode(false);
        newRoot->children.push_back(root);
        splitChild(newRoot, 0, root);
        root = newRoot;
    }

    insertNonFull(root, key, value);
}

void BTree::splitChild(BTreeNode *parent, int idx, BTreeNode *child)
{
    BTreeNode *newNode = new BTreeNode(child->leaf);

    for (int j = 0; j < t - 1; j++)
        newNode->entries.push_back(child->entries[j + t]);

    if (!child->leaf)
    {
        for (int j = 0; j < t; j++)
            newNode->children.push_back(child->children[j + t]);
    }

    pair<string, string> middle = child->entries[t - 1];

    child->entries.resize(t - 1);
    if (!child->leaf)
        child->children.resize(t);

    parent->children.insert(parent->children.begin() + idx + 1, newNode);

    parent->entries.insert(parent->entries.begin() + idx, middle);
}

void BTree::insertNonFull(BTreeNode *node, const string &key, const string &value)
{
    int i = node->entries.size() - 1;

    if (node->leaf)
    {
        node->entries.push_back({"", ""});

        while (i >= 0 && key < node->entries[i].first)
        {
            node->entries[i + 1] = node->entries[i];
            i--;
        }

        node->entries[i + 1] = {key, value};
    }
    else
    {
        while (i >= 0 && key < node->entries[i].first)
            i--;

        i++;

        if (node->children[i]->entries.size() == 2 * t - 1)
        {
            splitChild(node, i, node->children[i]);

            if (key > node->entries[i].first)
                i++;
        }

        insertNonFull(node->children[i], key, value);
    }
}
