#include "../include/TableManager.h"
#include "../include/Exceptions.h"
#include<iostream>

TableManager::TableManager() = default;

bool TableManager::  createTable(string name) {
    if (tableNames.count(name) > 0) {
        cout << "Error: table '" << name << "' already exists\n";
        return false;
    }
    tableNames.insert(name);
    tables[name] = make_unique<Table>();
    return true;
}

Table& TableManager::getTable(string name) {
    if(tables.count(name) == 0){
        throw InvalidQueryException();
    }
    return *tables[name];
}

void TableManager::beginTransaction() {
    if (inTransaction) {
        cout << "Error: transaction already in progress\n";
        return;
    }
    inTransaction = true;
    backups.clear();
}

void TableManager::touchForTransaction(string tableName) {
    if (!inTransaction) return;            // koi transaction active hi nahi, kuch mat karo
    if (backups.count(tableName) > 0) return;  // pehle se backup le chuke is table ka

    Table& t = getTable(tableName);
    backups[tableName] = t.rows;           // snapshot le liya, transaction se pehle wali state
}

void TableManager::commitTransaction() {
    if (!inTransaction) {
        cout << "Error: no transaction in progress\n";
        return;
    }
    for (auto& entry : backups) {
        string tableName = entry.first;
        Table& t = getTable(tableName);
        t.saveToFile(tableName + ".db");
    }
    backups.clear();
    inTransaction = false;
    cout << "Transaction committed.\n";
}

void TableManager::rollbackTransaction() {
    if (!inTransaction) {
        cout << "Error: no transaction in progress\n";
        return;
    }
    for (auto& entry : backups) {
        string tableName = entry.first;
        Table& t = getTable(tableName);
        t.rows = entry.second;             // backup se restore, disk untouched
    }
    backups.clear();
    inTransaction = false;
    cout << "Transaction rolled back.\n";
}

bool TableManager::isInTransaction() {
    return inTransaction;
}