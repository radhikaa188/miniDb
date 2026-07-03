#include "../include/BufferManager.h"
#include <fstream>
#include <iostream>
using namespace std;

BufferManager::BufferManager(string dbFileName) {
    dbFile = dbFileName;
    idxFile = dbFileName.substr(0, dbFileName.size() - 3) + ".idx";
    loadOffsets();
}

void BufferManager::loadOffsets() {
    ifstream idx(idxFile, ios::binary);
    if (!idx) {
        numPages = 0;
        return;
    }
    idx.read((char*)&numPages, sizeof(int));
    pageOffsets.resize(numPages);
    for (int i = 0; i < numPages; i++) {
        idx.read((char*)&pageOffsets[i], sizeof(long long));
    }
    idx.close();
}

vector<Row> BufferManager::loadPageFromDisk(int pageNum) {
    ifstream db(dbFile, ios::binary);
    if (!db) return {};

    db.seekg(pageOffsets[pageNum]);  // directly jump — .idx ka fayda yahan hai

    vector<Row> pageRows;
    for (int i = 0; i < PAGE_SIZE; i++) {
        int valCount;
        if (!db.read((char*)&valCount, sizeof(int))) break;  // last page kam rows ho sakti hai

        vector<string> rowValues;
        for (int j = 0; j < valCount; j++) {
            int len;
            db.read((char*)&len, sizeof(int));
            char* buffer = new char[len];
            db.read(buffer, len);
            string val(buffer, len);
            delete[] buffer;
            rowValues.push_back(val);
        }
        pageRows.push_back(Row(rowValues));
    }
    db.close();
    return pageRows;
}

void BufferManager::evict() {
    int lruPage = lruOrder.back();   // sabse puraana page
    lruOrder.pop_back();
    pagePosition.erase(lruPage);
    cache.erase(lruPage);
}

vector<Row> BufferManager::getPage(int pageNum) {
    if (cache.count(pageNum) > 0) {
        // cache hit — sirf order update karo
        lruOrder.erase(pagePosition[pageNum]);  // purani position hatao
        lruOrder.push_front(pageNum);           // front pe lo
        pagePosition[pageNum] = lruOrder.begin();
        return cache[pageNum];
    }

    // cache miss
    if ((int)cache.size() >= MAX_CACHE_SIZE) {
        evict();
    }

    vector<Row> loaded = loadPageFromDisk(pageNum);
    cache[pageNum] = loaded;
    lruOrder.push_front(pageNum);
    pagePosition[pageNum] = lruOrder.begin();
    return cache[pageNum];
}

int BufferManager::getNumPages() {
    return numPages;
}