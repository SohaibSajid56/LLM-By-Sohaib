// btree.cpp
#include "btree.h"
#include <algorithm>

BTreeNode::BTreeNode(bool isLeaf)
{
    leaf = isLeaf;
}

BTree::BTree(int minDegree)
{
    root = new BTreeNode(true);
    t = minDegree;
}

bool BTree::searchNode(BTreeNode *node, const string &key) const
{
    int i = 0;
    while (i < node->keys.size() && key > node->keys[i])
        i++;

    if (i < node->keys.size() && key == node->keys[i])
        return true;

    if (node->leaf)
        return false;

    return searchNode(node->children[i], key);
}

bool BTree::search(const string &key) const
{
    return searchNode(root, key);
}

void BTree::insert(const string &key)
{
    if (search(key))
        return; // Prevent duplicate insert

    if (root->keys.size() == 2 * t - 1)
    {
        BTreeNode *newRoot = new BTreeNode(false);
        newRoot->children.push_back(root);
        splitChild(newRoot, 0, root);
        root = newRoot;
    }
    insertNonFull(root, key);
}

void BTree::splitChild(BTreeNode *parent, int i, BTreeNode *child)
{
    BTreeNode *newNode = new BTreeNode(child->leaf);
    int mid = t - 1;

    for (int j = 0; j < t - 1; j++)
        newNode->keys.push_back(child->keys[j + t]);

    if (!child->leaf)
    {
        for (int j = 0; j < t; j++)
            newNode->children.push_back(child->children[j + t]);
    }

    child->keys.resize(mid);
    if (!child->leaf)
        child->children.resize(t);

    parent->children.insert(parent->children.begin() + i + 1, newNode);
    parent->keys.insert(parent->keys.begin() + i, child->keys[mid]);
}

void BTree::insertNonFull(BTreeNode *node, const string &key)
{
    int i = node->keys.size() - 1;

    if (node->leaf)
    {
        node->keys.push_back("");
        while (i >= 0 && key < node->keys[i])
        {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
    }
    else
    {
        while (i >= 0 && key < node->keys[i])
            i--;
        i++;

        if (node->children[i]->keys.size() == 2 * t - 1)
        {
            splitChild(node, i, node->children[i]);
            if (key > node->keys[i])
                i++;
        }
        insertNonFull(node->children[i], key);
    }
}
