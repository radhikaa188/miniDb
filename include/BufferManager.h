#pragma once
#include <map>
#include <list>
#include <vector>
#include <string>
#include "Row.h"
#include "Config.h"
using namespace std;

class BufferManager {
private:
    string dbFile;
    string idxFile;

    // LRU cache ke liye do structures saath kaam karte hain
    list<int> lruOrder;                        // front = most recently used page
    map<int, list<int>::iterator> pagePosition; // page number -> uski position list mein
    map<int, vector<Row>> cache;               // page number -> actual rows

    vector<long long> pageOffsets;  // .idx file se loaded offsets
    int numPages = 0;

    void loadOffsets();               // .idx file padho, offsets RAM mein lo
    vector<Row> loadPageFromDisk(int pageNum);  // disk se ek page padho
    void evict();                     // LRU page cache se hatao

public:
    BufferManager(string dbFileName);
    vector<Row> getPage(int pageNum);
    int getNumPages();
};