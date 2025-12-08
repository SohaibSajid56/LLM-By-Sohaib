// btree.h
#pragma once
#include <string>
#include <vector>
#include <iostream>
using namespace std;

struct BTreeNode
{
    bool leaf;
    vector<string> keys;
    vector<BTreeNode *> children;

    BTreeNode(bool isLeaf);
};

class BTree
{
private:
    BTreeNode *root;
    int t;

    void splitChild(BTreeNode *parent, int i, BTreeNode *child);
    void insertNonFull(BTreeNode *node, const string &key);
    bool searchNode(BTreeNode *node, const string &key) const;

public:
    BTree(int minDegree = 2);
    bool search(const string &key) const;
    void insert(const string &key);
};
