#pragma once
#include "BTreeNode.h"

class BTree {
private:
    unique_ptr<BTreeNode> root;

    void splitChild(BTreeNode* parent, int i);
    // parent ka i-th child overflow ho gaya → split karo

    void insertNonFull(BTreeNode* node, string key, int rowIndex);
    // node mein guaranteed space hai → seedha insert karo

    int searchNode(BTreeNode* node, string key);
    // recursive search

public:
    BTree();
    void insert(string key, int rowIndex);
    int search(string key);   // rowIndex return karo, -1 agar nahi mila
};