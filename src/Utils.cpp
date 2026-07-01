#include<map>
#include "../include/Row.h"
using namespace std;

int sortColumnIndex = 0;

bool compareRowsAscending(Row a, Row b) {
    return a.getValue(sortColumnIndex) > b.getValue(sortColumnIndex);
}

bool gpaAbove7(Row r){
    return stod(r.getValue(2))>7.0;
}