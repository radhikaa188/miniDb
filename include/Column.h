#pragma once
#include <string>
using namespace std;
class Column{
public:
    Column();
    virtual void print(string data)=0;
    virtual int size(string value)=0;
    virtual ~Column(){};
};
class IntColumn: public Column{
public:
    IntColumn();
    void print(string data) override;
    int size(string value) override;
};
class TextColumn: public Column{
public:
    TextColumn();
    void print(string data) override;
    int size(string value) override;
};
class DoubleColumn: public Column{
public:
    DoubleColumn();
    void print(string data) override;
    int size(string value) override;
};
