#include<iostream>
#include<fstream>
#include "../include/TableManager.h"
#include "../include/Query.h"
#include "../include/Column.h"
#include <vector>
#include <string>
using namespace std;
using namespace Query;


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
        
        if (command == "BEGIN") {
            manager.beginTransaction();
            continue;
        }else if (command == "COMMIT") {
            manager.commitTransaction();
            continue;
        }else if (command == "ROLLBACK") {
            manager.rollbackTransaction();
            continue;
        }else if(command=="CREATE") {
            if (tokens.size() < 3) {
                cout << "Error: Invalid CREATE syntax. Usage: CREATE TABLE name (col TYPE, ...)\n";
                continue;
            }
            string tableName=tokens[2];
            bool created = manager.createTable(tableName);
            if (!created) continue;

            Table&t=manager.getTable(tableName);
            string schemaFile = tableName + ".schema";
            if (tokens.size() > 4 && tokens[3] == "(") {
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
                t.saveSchema(schemaFile);
            }else{
                t.loadSchema(schemaFile);
                cout<<"Schema loaded from file for '"<< tableName << "'\n";
            }

            string filename = tableName + ".db";
            ifstream check(filename);
            if (check.good()) {
                check.close();
                t.loadFromFile(filename);   
                cout << "Existing data loaded for '" << tableName << "'\n";
            }
        
        }else if(command=="INSERT"){
            if (tokens.size() < 5) {
                cout << "Error: Invalid INSERT syntax. Usage: INSERT INTO table VALUES (...)\n";
                continue;
            }
            string tableName=tokens[2];
            try{
                Table&t=manager.getTable(tableName);

                vector<string> values;
                for(int i=4; i<tokens.size(); i++){
                    if(tokens[i] == "(" || tokens[i]==")" || tokens[i]==",") continue;
                    values.push_back(tokens[i]);
                }
                Row r(values);
                manager.touchForTransaction(tableName);
                t.insertRow(r);
                if (!manager.isInTransaction()) {
                    t.saveToFile(tableName + ".db");
                }
                cout<<"1 row inserted. Total row: "<<t.rowCount()<<"\n";
            }catch(exception&e){
                cout<<"Error: "<<e.what()<<"\n";
            }
        }else if(command == "SELECT"){
            if (tokens.size() < 4) {
                cout << "Error: Invalid SELECT syntax. Usage: SELECT * FROM table\n";
                continue;
            }
            string tableName = tokens[3];
            try{
                Table&t = manager.getTable(tableName);
                if (tokens.size() > 4 && tokens[4] == "WHERE") {
                    string colName = tokens[5];
                    string op = tokens[6];
                    string value = tokens[7];
                    t.selectWhereCondition(colName, op, value);
                }else if (tokens.size() > 4 && tokens[4] == "ORDER") {
                    string colName = tokens[6];
                    t.selectOrderBy(colName);
                } else {
                    t.selectAll();
                }
            }catch(exception&e){
                cout<<"Error: "<<e.what()<<"\n";
            }
        }else if (command == "DELETE") {
            if (tokens.size() < 7) {
                cout << "Error: Invalid DELETE syntax. Usage: DELETE FROM table WHERE col = value\n";
                continue;
            }
            string tableName = tokens[2];   // DELETE(0) FROM(1) students(2)
            string colName = tokens[4];     // WHERE(3) id(4) =(5) 1(6)
            string value = tokens[6];

            try {
                Table& t = manager.getTable(tableName);
                manager.touchForTransaction(tableName);
                t.deleteWhere(colName, value);
                if (!manager.isInTransaction()) {
                    t.saveToFile(tableName + ".db");
                }
                cout << "Rows deleted.\n";
            } catch (exception& e) {
                cout << "Error: " << e.what() << "\n";
            }
        }else if (command == "UPDATE") {
            if (tokens.size() < 10) {
                cout << "Error: Invalid UPDATE syntax. Usage: UPDATE table SET col = val WHERE col = val\n";
                continue;
            }
            string tableName = tokens[1];
            string setCol = tokens[3];
            string newValue = tokens[5];
            string whereCol = tokens[7];
            string whereValue = tokens[9];

            try {
                Table& t = manager.getTable(tableName);
                manager.touchForTransaction(tableName);
                t.updateWhere(setCol, newValue, whereCol, whereValue);
                if (!manager.isInTransaction()) {
                    t.saveToFile(tableName + ".db");
                }
                cout << "Rows updated.\n";
            } catch (exception& e) {
                cout << "Error: " << e.what() << "\n";
            }
        }
        else{
            cout<<"Unknown command\n";
        }
    }
    return 0;
}