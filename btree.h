// btree.h
#pragma once
#include <string>
#include <vector>
using namespace std;

struct BTreeNode
{
    bool leaf;

    vector<pair<string, string>> entries;

    vector<BTreeNode *> children;

    BTreeNode(bool isLeaf);
};

class BTree
{
private:
    BTreeNode *root;
    int t;

    void splitChild(BTreeNode *parent, int idx, BTreeNode *child);
    void insertNonFull(BTreeNode *node, const string &key, const string &value);

public:
    BTree(int minDegree);

    bool search(const string &key, string &valueOut) const;

    void insert(const string &key, const string &value);
};
