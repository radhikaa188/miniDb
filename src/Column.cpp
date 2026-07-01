#include "../include/Exceptions.h"
#include <string>
#include "../include/Column.h"
#include<iostream>
using namespace std;

Column::Column(){
    cout<<"column constructor\n";
}

IntColumn::IntColumn(){
    cout<<"intcolumn constructor\n";
}
void IntColumn:: print(string data){
    try{
        cout<< stoi(data);
    }catch(exception &e){
        throw TypeMismatchException();
    }
}
int IntColumn ::size(string value){
    return sizeof(int);
}
    
    TextColumn::TextColumn(){
        cout<<"textcolumn constructor\n";
    }
    void TextColumn:: print(string data){
        cout<<data;
    }
    int TextColumn::size(string value) {
        return value.size();
    }

    DoubleColumn:: DoubleColumn(){
        cout<<"doublecolumn constructor\n";
    }
    void DoubleColumn::print(string data){
        cout<<data;
    }
    int DoubleColumn::size(string value) {
        return sizeof(double);
    }
