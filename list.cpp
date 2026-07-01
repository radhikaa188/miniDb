#include <iostream>
#include<fstream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include<set>
#include<memory>
using namespace std;

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

class RowNotFoundException : public exception {
public:
    const char* what() const noexcept override {
        return "Row not found at given index!";
    }
};

class InvalidQueryException : public exception {
public:
    const char* what() const noexcept override {
        return "Invalid query syntax!";
    }
};

class TypeMismatchException : public exception {
public:
    const char* what() const noexcept override {
        return "Type mismatch — expected a different data type!";
    }
};

namespace Storage{
class Column{
public:
    Column(){
        cout<<"column constructor\n";
    }
    virtual void print(string data)=0;
    virtual int size(string value)=0;
    virtual ~Column(){};
};
class IntColumn: public Column{
public:
    IntColumn(){
        cout<<"intcolumn constructor\n";
    }
    ~IntColumn(){
        cout<<"intcolumn destructor\n";
    }
    void print(string data) override{
        try{
            cout<< stoi(data);
        }catch(exception &e){
            throw TypeMismatchException();
        }
    }
    int size(string value) override{
        return sizeof(int);
    }
};
class TextColumn: public Column{
public:
    TextColumn(){
        cout<<"textcolumn constructor\n";
    }
    void print(string data) override{
        cout<<data;
    }
    int size(string value) override{
        return value.size();
    }
};
class DoubleColumn: public Column{
public:
    DoubleColumn(){
        cout<<"doublecolumn constructor\n";
    }
    void print(string data) override{
        cout<<data;
    }
    int size(string value) override{
        return sizeof(double);
    }
};
class Row {
private:
    vector<string> values;
    int currId;
    //static member decleration
    static int nextId;
public:
    explicit Row(vector<string> vals) {
        values = vals;
        currId=nextId;
        nextId=nextId+1;
    }

    string getValue(int idx) {
        return values[idx];
    }

    int size() {
        return values.size();
    }

    void print() {
        cout<<currId<<'|';
        for (int i = 0; i < values.size(); i++) {
            cout << values[i] ;
            if (i != values.size() - 1) {
            cout << "|";
        }
        }
    }
    void setValue(int idx, string val) {
        values[idx] = val;
    }
};

//static member defination
int Row::nextId=1;

int sortColumnIndex = 0;   // global — batata hai abhi kaunsa column sort key hai

bool compareRowsAscending(Row a, Row b) {
    return a.getValue(sortColumnIndex) > b.getValue(sortColumnIndex);
}

class Table {
private:
    map<string, unique_ptr<Column>> schema;
    vector<string> columnNames;
public:
    vector<Row> rows;
    // Constructor
    Table() {
        cout << "Table created.\n";
    }

    // Destructor
    ~Table() {
        cout << "Table destroyed.\n";
    }

    Table& insertRow(const Row &r) {
        rows.push_back(r);
        return *this;
    }

    Table& addColumn(string name, unique_ptr<Column> c){
        columnNames.push_back(name);
        schema[name] = move(c);
        return *this;
    }
    void printRow(int idx) {
        if (idx < 0 || idx >= rows.size()) {
            throw RowNotFoundException();
        }
        rows[idx].print();
    }

    int rowCount() {
        return rows.size();
    }
    void selectAll() {
        for (int i = 0; i < rows.size(); i++) {
            for(int j=0; j<schema.size(); j++){
                string colName = columnNames[j];
                schema[colName]->print(rows[i].getValue(j));
                if(j < columnNames.size()-1) cout << " | "; 
            }
            cout<<"\n";
        }
    }

    vector<Row> selectWhere(bool (*predicate)(Row)) {
        return findRows(rows, predicate);
    }
    
    void saveToFile(string fileName){
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
    void loadFromFile(string fileName){
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
    void printColumnTypes() {
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
    void orderBy(int colIndex) {
        sortColumnIndex = colIndex;          // pehle batao kaunsa column
        simpleSort(rows, compareRowsAscending);   // phir sort karo
    }
    void deleteWhere(string colName, string value) {
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
    void updateWhere(string setCol, string newValue, string whereCol, string whereValue) {
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
};

class TableManager {
private:
    map<string, unique_ptr<Table>> tables;
    set<string> tableNames;

public:
    bool createTable(string name) {
        if (tableNames.count(name) > 0) {
            cout << "Error: table '" << name << "' already exists\n";
            return false;
        }
        tableNames.insert(name);
        tables[name] = make_unique<Table>();
        return true;
    }

    Table& getTable(string name) {
        if(tables.count(name) == 0){
            throw InvalidQueryException();
        }
        return *tables[name];
    }
};

}


namespace Query{
    vector<string> tokenize(string query){
        vector<string> tokens;
        string current = "";
        for(int i=0; i<query.size(); i++){
            char c = query[i];
            if(c== ' '){
                if(current!=""){
                    tokens.push_back(current);
                    current="";
                }
            }else if(c == ',' || c == ')' || c == '('){
                if(current!=""){
                    tokens.push_back(current);
                    current="";
                }
                tokens.push_back(string(1,c));
            }else{
                current+=c;
            }
        }
        if(current!=""){
            tokens.push_back(current);
        }
        return tokens;
    }
}
using namespace Storage;
using namespace Query;

    template <typename T>
    map<T, vector<int>> buildIndex(vector<Row>& rows, int colIndex) {
        map<T, vector<int>> index;
        for (int i = 0; i < rows.size(); i++) {
            T key = rows[i].getValue(colIndex);   // column value le lo (abhi string hi rahega)
            index[key].push_back(i);              // us value ke saath row position store karo
        }
        return index;
    }

bool gpaAbove7(Row r){
    return stod(r.getValue(2))>7.0;
}

int main(){

    TableManager manager;
    cout<<"MiniDB started. Type a cmd(or EXIT to quit):\n";

    string line;
    while(true){
        cout<<"minidb> ";
        getline(cin, line);
        if(line == "EXIT") break;
        vector<string> tokens= tokenize(line);
        if(tokens.empty()) continue;
        string command = tokens[0];
        if(command=="CREATE") {
            string tableName=tokens[2];
            bool created = manager.createTable(tableName);
            if (!created) continue;

            Table&t=manager.getTable(tableName);
            for (int i = 4; i + 1 < tokens.size() && tokens[i] != ")"; i += 3) {
                string colName = tokens[i];
                string colType = tokens[i + 1];

                if (colType == "INT") {
                    t.addColumn(colName, make_unique<IntColumn>());
                } else if (colType == "TEXT") {
                    t.addColumn(colName, make_unique<TextColumn>());
                } else if (colType == "DOUBLE") {
                    t.addColumn(colName, make_unique<DoubleColumn>());
                } else {
                    cout << "Unknown type: " << colType << "\n";
                }
            }


            string filename = tableName + ".db";
            ifstream check(filename);
            if (check.good()) {
                check.close();
                t.loadFromFile(filename);   
                cout << "Existing data loaded for '" << tableName << "'\n";
            }
        
        }else if(command=="INSERT"){
            string tableName=tokens[2];
            try{
                Table&t=manager.getTable(tableName);

                vector<string> values;
                for(int i=4; i<tokens.size(); i++){
                    if(tokens[i] == "(" || tokens[i]==")" || tokens[i]==",") continue;
                    values.push_back(tokens[i]);
                }
                Row r(values);
                t.insertRow(r);
                t.saveToFile(tableName + ".db");
                cout<<"1 row inserted. Total row: "<<t.rowCount()<<"\n";
            }catch(exception&e){
                cout<<"Error: "<<e.what()<<"\n";
            }
        }else if(command == "SELECT"){
            string tableName = tokens[3];
            try{
                Table&t = manager.getTable(tableName);
                t.selectAll();
            }catch(exception&e){
                cout<<"Error: "<<e.what()<<"\n";
            }
        }else if (command == "DELETE") {
            string tableName = tokens[2];   // DELETE(0) FROM(1) students(2)
            string colName = tokens[4];     // WHERE(3) id(4) =(5) 1(6)
            string value = tokens[6];

            try {
                Table& t = manager.getTable(tableName);
                t.deleteWhere(colName, value);
                t.saveToFile(tableName + ".db");
                cout << "Rows deleted.\n";
            } catch (exception& e) {
                cout << "Error: " << e.what() << "\n";
            }
        }else if (command == "UPDATE") {
            string tableName = tokens[1];
            string setCol = tokens[3];
            string newValue = tokens[5];
            string whereCol = tokens[7];
            string whereValue = tokens[9];

            try {
                Table& t = manager.getTable(tableName);
                t.updateWhere(setCol, newValue, whereCol, whereValue);
                t.saveToFile(tableName + ".db");
                cout << "Rows updated.\n";
            } catch (exception& e) {
                cout << "Error: " << e.what() << "\n";
            }
        }
        else{
            cout<<"Unknown command\n";
        }
    }

//    TableManager manager;
//    manager.createTable("students");
//    manager.createTable("teachers");
//    manager.createTable("students");
// //    vector<string> tokens = tokenize("INSERT INTO students VALUES Amol 21");
// //     for(string t: tokens){
// //         cout<<t<<"\n";
// //     }
//     Table&t=manager.getTable("students");
//     t.addColumn("id", make_unique<IntColumn>());
//     Table table;
//     table.addColumn("id", make_unique<IntColumn>());
//     table.addColumn("name",make_unique<TextColumn>());
//     table.addColumn("gpa", make_unique<DoubleColumn>());

//     table.printColumnTypes();

//     ifstream check("test.db");
//     bool fileExists = check.good();
//     check.close();
//     if(fileExists){
//          table.loadFromFile("test.db");
//          table.selectAll();  
//         char choice;
//     cout << "Add new rows? (y/n): ";
//     cin >> choice;

//     if (choice == 'y') {
//         int newRows;
//         cout << "How many new rows? ";
//         cin >> newRows;

//         for (int i = 0; i < newRows; i++) {
//             string id, name, gpa;
//             cin >> id >> name >> gpa;

//             vector<string> rowData;
//             rowData.push_back(id);
//             rowData.push_back(name);
//             rowData.push_back(gpa);

//             Row r(rowData);
//             table.insertRow(r);
//         }

//         table.saveToFile("test.db");   // poori table dobara save, purani + nayi
//         table.selectAll();
//     }
//     }else{
//         int tableRows;
//         cout<<"Input rows number: ";
//         cin >> tableRows;

//         for(int i = 0; i < tableRows; i++){
//             string id, name, gpa;
//             cin >> id >> name >> gpa;

//             vector<string> rowData;
//             rowData.push_back(id);
//             rowData.push_back(name);
//             rowData.push_back(gpa);
//             //object banaya row class ka and row vector me values daldi
//             Row r(rowData);
//             //vector me push krdi upr wali row
//             table.insertRow(r);
//         }
//         table.saveToFile("test.db");
//         table.selectAll();
//     }
//     try{
//         table.printRow(999);
//     }catch(RowNotFoundException &e){
//         cout<<"error: "<<e.what()<<"\n"; 
//     }
//     vector<Row> highGpaRows = table.selectWhere(gpaAbove7);
//     cout<<"students wuth gpa > 7"<<"\n";
//     for(int i=0; i<highGpaRows.size(); i++){
//         highGpaRows[i].print();
//         cout<<"\n";
//     }
//     cout<<"order"<<"\n";
//     table.orderBy(2);
//     table.selectAll();
//     map<string, vector<int>> gpaIndex = buildIndex<string>(table.rows, 2);   // column 2 = gpa

//     for (auto& pair : gpaIndex) {
//         cout << pair.first << " -> rows: ";
//         for (int pos : pair.second) {
//             cout << pos << " ";
//         }
//         cout << "\n";
//     }
    return 0;
}