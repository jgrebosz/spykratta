#ifndef DataDecoder_h
#define DataDecoder_h

#include <vector>
#include <iostream>
#include <stdint.h>
#include "DataBoard.h"

class DataDecoder{
private:
    static std::vector<DataDecoder*> dbase;
// 
    static int push_back(DataDecoder* obj);
    static DataDecoder * find(const char * type);

protected:
    const char * type;
    DataDecoder(const char * type);
    
public:
    static int Size(){
        return dbase.size();
    }
    static DataDecoder * Get(const char * type){
        return find(type);
    }
    static void List(std::ostream & str = std::cout){
    for(int i = 0; i < Size(); i++)
        dbase[i]->ls(str);
//         str << dbase[i]->type << " -> " << dbase[i] << std::endl;
}
/// Pure virtual functions
    virtual int Load(void * buf) = 0; // Main decoding function
    virtual int GetNchan() = 0; // returns number of channels in the board
    virtual int GetDataSize(int i = 0) = 0;  // returns number of data per channel
    virtual DataType GetData(int channel, int ndata) = 0; // Returns data
//     virtual int GetXmin() = 0; // returns minimal value for data
//     virtual int GetXmax() = 0; // returns max value for data
//     virtual int GetNbin() = 0; // returns number of bins
    virtual void Dump() = 0;
    virtual bool Empty(int i) = 0;
    virtual void ls(std::ostream & str){
        str << type << " -> " << this << std::endl;
    }

};
#endif

