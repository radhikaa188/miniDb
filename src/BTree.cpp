#include "../include/BTree.h"
#include <iostream>
using namespace std;

// ─── Constructor ───────────────────────────────────────────────
BTree::BTree() {
    root = make_unique<BTreeNode>(true);  // pehle root ek khaali leaf hai
}

// ─── Search ────────────────────────────────────────────────────
int BTree::searchNode(BTreeNode* node, string key) {
    int i = 0;

    // pehli key dhundho jo >= key ho
    while (i < (int)node->keys.size() && key > node->keys[i]) {
        i++;
    }

    // key mil gayi
    if (i < (int)node->keys.size() && node->keys[i] == key) {
        return node->rowIndices[i];
    }

    // leaf pe aa gaye aur mili nahi → exist nahi karti
    if (node->isLeaf) return -1;

    // appropriate child mein jao
    return searchNode(node->children[i].get(), key);
}

int BTree::search(string key) {
    return searchNode(root.get(), key);
}

// ─── Split ─────────────────────────────────────────────────────
void BTree::splitChild(BTreeNode* parent, int i) {
    BTreeNode* child = parent->children[i].get();  // jo node split hogi
    auto newNode = make_unique<BTreeNode>(child->isLeaf);

    int mid = T - 1;  // middle key index

    // middle key parent mein jaati hai
    parent->keys.insert(parent->keys.begin() + i, child->keys[mid]);
    parent->rowIndices.insert(parent->rowIndices.begin() + i, child->rowIndices[mid]);

    // right half newNode mein jaati hai
    newNode->keys.assign(child->keys.begin() + mid + 1, child->keys.end());
    newNode->rowIndices.assign(child->rowIndices.begin() + mid + 1, child->rowIndices.end());

    // agar internal node hai to children bhi split karo
    if (!child->isLeaf) {
        newNode->children = vector<unique_ptr<BTreeNode>>(
            make_move_iterator(child->children.begin() + T),
            make_move_iterator(child->children.end())
        );
        child->children.erase(child->children.begin() + T, child->children.end());
    }

    // left half child mein rakho (mid aur right hata do)
    child->keys.erase(child->keys.begin() + mid, child->keys.end());
    child->rowIndices.erase(child->rowIndices.begin() + mid, child->rowIndices.end());

    // newNode ko parent ke children mein daalo
    parent->children.insert(parent->children.begin() + i + 1, move(newNode));
}

// ─── Insert Non Full ───────────────────────────────────────────
void BTree::insertNonFull(BTreeNode* node, string key, int rowIndex) {
    int i = (int)node->keys.size() - 1;

    if (node->isLeaf) {
        // sorted position pe insert karo
        node->keys.push_back("");
        node->rowIndices.push_back(0);

        while (i >= 0 && key < node->keys[i]) {
            node->keys[i + 1] = node->keys[i];
            node->rowIndices[i + 1] = node->rowIndices[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->rowIndices[i + 1] = rowIndex;

    } else {
        // sahi child dhundho
        while (i >= 0 && key < node->keys[i]) i--;
        i++;

        // agar child full hai → pehle split
        if ((int)node->children[i]->keys.size() == 2 * T - 1) {
            splitChild(node, i);
            if (key > node->keys[i]) i++;
        }
        insertNonFull(node->children[i].get(), key, rowIndex);
    }
}

// ─── Insert ────────────────────────────────────────────────────
void BTree::insert(string key, int rowIndex) {
    BTreeNode* r = root.get();

    // root full hai → pehle root split karo
    if ((int)r->keys.size() == 2 * T - 1) {
        auto newRoot = make_unique<BTreeNode>(false);
        newRoot->children.push_back(move(root));
        splitChild(newRoot.get(), 0);
        root = move(newRoot);
    }

    insertNonFull(root.get(), key, rowIndex);
}