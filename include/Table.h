#pragma once
#include<string>
#include<map>
#include<vector>
#include <memory>
#include "Row.h"
#include "Column.h"
#include "Config.h"
#include "BufferManager.h"
#include "BTree.h"

using namespace std;

class Table {
private:
    map<string, unique_ptr<Column>> schema;
    vector<string> columnNames;
    unique_ptr<BufferManager> bufferManager = nullptr;
    unique_ptr<BTree> btree;
public:
    vector<Row> rows;

    // Constructor
    Table();

    // Destructor
    ~Table();

    void saveSchema(string fileName);
    void loadSchema(string fileName);
    vector<string> getColumnNames();   // taaki Table ke bahar se naam/type pata kar sakein
    string getColumnType(string colName);

    Table& insertRow(const Row &r);

    Table& addColumn(string name, unique_ptr<Column> c);

    void printRow(int idx);

    int rowCount();

    void selectAll();

    vector<Row> selectWhere(bool (*predicate)(Row));
    
    void selectWhereCondition(string colName, string op, string value);

    void selectOrderBy(string colName);

    void saveToFile(string fileName);
    
    void loadFromFile(string fileName);
    
    // Table class ke andar
    void printColumnTypes();
    
    void orderBy(int colIndex);
    
    void deleteWhere(string colName, string value);
    
    void updateWhere(string setCol, string newValue, string whereCol, string whereValue);

    void initBufferManager(string fileName);
};
