#include<vector>
#include<map>
#include "Row.h"
using namespace std ;
template <typename T>
void simpleSort(vector<T>& items, bool (*comparator)(T, T)) {
    for (int i = 0; i < items.size(); i++) {
        for (int j = 0; j < items.size() - 1 - i; j++) {
            if (comparator(items[j], items[j+1])) {
                swap(items[j], items[j+1]);
            }
        }
    }
}

template <typename T>
vector<T> findRows(vector<T>&items, bool(*predicate)(T)){
    vector<T> result;
    for(int i=0; i<items.size(); i++){
        if(predicate(items[i])){
            result.push_back(items[i]);
        }
    }
    return result;
}

bool compareRowsAscending(Row a, Row b);

template <typename T>
map<T, vector<int>> buildIndex(vector<Row>& rows, int colIndex) {
    map<T, vector<int>> index;
    for (int i = 0; i < rows.size(); i++) {
        T key = rows[i].getValue(colIndex);   // column value le lo (abhi string hi rahega)
        index[key].push_back(i);              // us value ke saath row position store karo
    }
    return index;
}

bool gpaAbove7(Row r);

extern int sortColumnIndex;