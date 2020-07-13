#include "DataDecoder.h"
#include <iostream>
#include <string.h>

std::vector<DataDecoder*> DataDecoder::dbase;

DataDecoder * DataDecoder::find(const char * type){
        for(unsigned int i = 0; i <dbase.size(); i++)
             if(strcmp(dbase[i]->type, type) == 0) 
               return dbase[i];
        return 0;
    }
    
DataDecoder::DataDecoder(const char * t):type(t){
    dbase.push_back(this);
}
