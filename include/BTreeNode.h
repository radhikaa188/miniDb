#pragma once
#include <vector>
#include <string>
#include <memory>
using namespace std;

const int T = 2;  // minimum degree
                  // ek node mein min T-1 keys (1), max 2T-1 keys (3)
                  // ek node mein min T children (2), max 2T children (4)

class BTreeNode {
public:
    vector<string> keys;               // sorted column values
    vector<int> rowIndices;            // keys[i] → rowIndices[i] (row ka position)
    vector<unique_ptr<BTreeNode>> children;  // child nodes
    bool isLeaf;                       // leaf hai to children empty hai

    BTreeNode(bool isLeaf);

    friend class BTree;  // BTree ko private internals access karne do
};