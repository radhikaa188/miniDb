#pragma once
#include <map>
#include <memory>
#include <string>
#include "Table.h"
#include<set>
using namespace std;

class TableManager {
private:
    map<string, unique_ptr<Table>> tables;
    set<string> tableNames;
    bool inTransaction = false;
    map<string, vector<Row>> backups;

    public:
    TableManager();
    bool createTable(string name);

    Table& getTable(string name);

    void beginTransaction();
    void commitTransaction();
    void rollbackTransaction();
    void touchForTransaction(string tableName);   // called before any write, takes backup if needed
    bool isInTransaction();
    void dropTable(string name);
};