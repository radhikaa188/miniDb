#include "../include/Row.h"
#include<iostream>
#include<vector>
using namespace std;

int Row::nextId=1;

Row:: Row(vector<string> vals) {
        values = vals;
        currId=nextId;
        nextId=nextId+1;
    }
string Row::getValue(int idx) const{
        return values[idx];
    }

int Row::size() {
    return values.size();
}

void Row::print() {
    cout<<currId<<'|';
    for (int i = 0; i < values.size(); i++) {
        cout << values[i] ;
        if (i != values.size() - 1) {
            cout << "|";
        }
    }
}
void Row::setValue(int idx, string val) {
    values[idx] = val;
}
