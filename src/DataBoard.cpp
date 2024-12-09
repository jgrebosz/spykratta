#include <iomanip>
#include <iostream>
#include <string.h>
#include "DataBoard.h"

using namespace std;

DataBoard::DataBoard(){}

DataBoard::~DataBoard(){
     for(auto  i=begin(); i != end(); i++)
         delete[] i->second.data;
    clear();
}

void DataBoard::Clear(){
    for(auto  i=begin(); i != end(); i++)
        memset(i->second.data, 0, i->second.size*sizeof(DataType));
}

void DataBoard::Reset(){
    for(auto  i=begin(); i != end(); i++)
        i->second.counts = 0;
}


DataType * DataBoard::AddEntry(const char * name, int size){
    DataType * data = new DataType[size];
    operator[](name)  = {data, size, 0};
    return data;
}

DataType * DataBoard::GetEntryData(const char * name, int i){
    DataType * data = 0;
    if(find(name) != end()){
        data = operator[](name).data;
        operator[](name).counts += i;
    }
//     std::cout << "DataBoard::GetEntryData: " << name <<" " << data << std::endl;
        return data;
}

int DataBoard::GetEntrySize(const char * name){
    if(find(name) != end())
        return operator[](name).size;
    else
        return 0;
}

void DataBoard::Print(std::ostream & str){
    str << setw(8) << "channel"    << "\t"
        << setw(8) <<  "address"   << "\t"
                   <<  "size"      << "\t"
                   <<  "counts"    << "\n";
      for(auto  i=begin(); i != end(); i++)
        str << setw(8) << i->first         << "\t"
            << i->second.data   <<"\t"
            << i->second.size   <<"\t"
            << i->second.counts <<"\t"
           
            << endl;
}
