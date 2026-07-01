#include "../include/Table.h"
#include<iostream>
#include "../include/Utils.h"
#include "../include/Exceptions.h"
#include <fstream>
using namespace std;

// Constructor
Table::Table() {
    cout << "Table created.\n";
}

// Destructor
Table::~Table() {
    cout << "Table destroyed.\n";
}

string Table::getColumnType(string colName) {
    IntColumn* i = dynamic_cast<IntColumn*>(schema[colName].get());
    if (i != nullptr) return "INT";

    DoubleColumn* d = dynamic_cast<DoubleColumn*>(schema[colName].get());
    if (d != nullptr) return "DOUBLE";

    return "TEXT";
}

vector<string> Table::getColumnNames() {
    return columnNames;
}

void Table::saveSchema(string fileName) {
    ofstream outFile(fileName, ios::binary);

    int colCount = columnNames.size();
    outFile.write((char*)&colCount, sizeof(int));

    for (int i = 0; i < colCount; i++) {
        string name = columnNames[i];
        string type = getColumnType(name);

        int nameLen = name.size();
        outFile.write((char*)&nameLen, sizeof(int));
        outFile.write(name.c_str(), nameLen);

        int typeLen = type.size();
        outFile.write((char*)&typeLen, sizeof(int));
        outFile.write(type.c_str(), typeLen);
    }
    outFile.close();
}

void Table::loadSchema(string fileName) {
    ifstream inFile(fileName, ios::binary);
    if (!inFile) return;   // schema file nahi hai, koi baat nahi

    int colCount;
    inFile.read((char*)&colCount, sizeof(int));

    for (int i = 0; i < colCount; i++) {
        int nameLen;
        inFile.read((char*)&nameLen, sizeof(int));
        char* nameBuf = new char[nameLen];
        inFile.read(nameBuf, nameLen);
        string name(nameBuf, nameLen);
        delete[] nameBuf;

        int typeLen;
        inFile.read((char*)&typeLen, sizeof(int));
        char* typeBuf = new char[typeLen];
        inFile.read(typeBuf, typeLen);
        string type(typeBuf, typeLen);
        delete[] typeBuf;

        if (type == "INT") addColumn(name, make_unique<IntColumn>());
        else if (type == "DOUBLE") addColumn(name, make_unique<DoubleColumn>());
        else addColumn(name, make_unique<TextColumn>());
    }
    inFile.close();
}

Table& Table::insertRow(const Row &r) {
    rows.push_back(r);
    return *this;
}

Table& Table::addColumn(string name, unique_ptr<Column> c){
    columnNames.push_back(name);
    schema[name] = move(c);
    return *this;
}
void Table::printRow(int idx) {
    if (idx < 0 || idx >= rows.size()) {
        throw RowNotFoundException();
    }
    rows[idx].print();
}

int Table::rowCount() {
    return rows.size();
}
void Table::selectAll() {
    for (int i = 0; i < rows.size(); i++) {
        for(int j=0; j<schema.size(); j++){
            string colName = columnNames[j];
            schema[colName]->print(rows[i].getValue(j));
            if(j < columnNames.size()-1) cout << " | "; 
        }
        cout<<"\n";
    }
}

vector<Row> Table::selectWhere(bool (*predicate)(Row)) {
    return findRows(rows, predicate);
}

void Table::selectWhereCondition(string colName, string op, string value) {
    int colIndex = -1;
    for (int i = 0; i < columnNames.size(); i++) {
        if (columnNames[i] == colName) {
            colIndex = i;
            break;
        }
    }
    if (colIndex == -1) throw InvalidQueryException();

    for (int i = 0; i < rows.size(); i++) {
        string actual = rows[i].getValue(colIndex);
        bool matches = false;

        if (op == "=") {
            matches = (actual == value);
        } else if (op == ">") {
            matches = (stod(actual) > stod(value));
        } else if (op == "<") {
            matches = (stod(actual) < stod(value));
        }

        if (matches) {
            for (int j = 0; j < columnNames.size(); j++) {
                schema[columnNames[j]]->print(rows[i].getValue(j));
                if (j < columnNames.size() - 1) cout << " | ";
            }
            cout << "\n";
        }
    }
}

void Table::selectOrderBy(string colName) {
    int colIndex = -1;
    for (int i = 0; i < columnNames.size(); i++) {
        if (columnNames[i] == colName) {
            colIndex = i;
            break;
        }
    }
    if (colIndex == -1) throw InvalidQueryException();

    orderBy(colIndex);   // tumhara already-bana hua orderBy() reuse ho raha hai
    selectAll();
}

void Table::saveToFile(string fileName){
    ofstream outFile(fileName, ios::binary);

    int rowCount = rows.size();
    outFile.write((char*)&rowCount, sizeof(int));   //total rows phle

    for(int i=0; i<rowCount; i++){
        int valCount=rows[i].size();
        outFile.write((char*)&valCount, sizeof(int));

        for(int j=0; j<valCount; j++){
            string val = rows[i].getValue(j);
            int len = val.size();
            outFile.write((char*)&len, sizeof(int));
            outFile.write(val.c_str() , len);
        }
    }
    outFile.close();
}
void Table::loadFromFile(string fileName){
    try{
        ifstream inFile(fileName, ios::binary);
        if(!inFile) throw InvalidQueryException();
        rows.clear();

        int rowCount;
        inFile.read((char*)&rowCount, sizeof(int));

        for(int i=0; i<rowCount; i++){
            int valCount;
            inFile.read((char*)&valCount, sizeof(int));
            vector<string>rowValues;

            for(int j=0; j<valCount;j++){
                int len;
                inFile.read((char*)&len, sizeof(int));
                char*buffer = new char[len];
                inFile.read(buffer, len);
                string readName(buffer, len);
                delete[]buffer;
                rowValues.push_back(readName);
            }
            Row r(rowValues);
            rows.push_back(r);
        }
        inFile.close();
    }catch(exception &e){
        cout<<"error loading file: "<<e.what()<< "\n";
    }
}
// Table class ke andar
void Table::printColumnTypes() {
    for (int i = 0; i < schema.size(); i++) {
        string colName=columnNames[i];
        IntColumn* check = dynamic_cast<IntColumn*>(schema[colName].get());
        if (check != nullptr) {
            cout << "Column " << colName << " is IntColumn\n";
        } else {
            cout << "Column " << colName << " is NOT IntColumn\n";
        }
    }
}
void Table::orderBy(int colIndex) {
    sortColumnIndex = colIndex;          // pehle batao kaunsa column
    simpleSort(rows, compareRowsAscending);   // phir sort karo
}
void Table::deleteWhere(string colName, string value) {
    int colIndex = -1;
    for (int i = 0; i < columnNames.size(); i++) {
        if (columnNames[i] == colName) {
            colIndex = i;
            break;
        }
    }
    if (colIndex == -1) throw InvalidQueryException();

    vector<Row> newRows;
    for (int i = 0; i < rows.size(); i++) {
        if (rows[i].getValue(colIndex) != value) {
            newRows.push_back(rows[i]);   // jo match nahi karta, usko rakho
        }
    }
    rows = newRows;
}
void Table::updateWhere(string setCol, string newValue, string whereCol, string whereValue) {
    int setIdx = -1, whereIdx = -1;
    for (int i = 0; i < columnNames.size(); i++) {
        if (columnNames[i] == setCol) setIdx = i;
        if (columnNames[i] == whereCol) whereIdx = i;
    }
    if (setIdx == -1 || whereIdx == -1) throw InvalidQueryException();

    for (int i = 0; i < rows.size(); i++) {
        if (rows[i].getValue(whereIdx) == whereValue) {
            rows[i].setValue(setIdx, newValue);
        }
    }
}
