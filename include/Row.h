#pragma once
#include<vector>
#include<string>
using namespace std;
class Row {
private:
    vector<string> values;
    int currId;
    static int nextId;
public:
    explicit Row(vector<string> vals);

    string getValue(int idx);

    int size();

    void print();
    
    void setValue(int idx, string val);
};
