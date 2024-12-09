#ifndef DataBoard_h
#define DataBoard_h

#include <map>
#include <string>
#include <iostream>

typedef double DataType;

struct MyData{
    DataType * data;
    int size;
    unsigned long counts;
};

class DataBoard:private std::map<std::string, MyData >{
public:

DataBoard();

~DataBoard();
void Clear();

DataType * AddEntry(const char * name, int size);

DataType * GetEntryData(const char * name, int i = 0);

int GetEntrySize(const char * name);
void Print(std::ostream & str = std::cout);
void Reset();
};

#endif
