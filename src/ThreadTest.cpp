#include <thread>
#include <iostream>
#include "../include/Table.h"
#include "../include/Column.h"
using namespace std;

void insertRows(Table& t, int start, int end) {
    for (int i = start; i < end; i++) {
        Row r({to_string(i), "User" + to_string(i)});
        t.insertRow(r);
    }
}

int main() {
    Table t;
    t.addColumn("id", make_unique<IntColumn>());
    t.addColumn("name", make_unique<TextColumn>());

    // do threads simultaneously insert karein
    thread t1(insertRows, ref(t), 1, 6);   // rows 1-5
    thread t2(insertRows, ref(t), 6, 11);  // rows 6-10

    t1.join();  // wait karo t1 khatam ho
    t2.join();  // wait karo t2 khatam ho

    cout << "Total rows: " << t.rowCount() << "\n";
    t.selectAll();
    return 0;
}