#ifndef LTBranch_h
#define LTBranch_h

#include "LTRecord.h"
#include <map>

#ifdef LTFAST
typedef uint64_t KeyType;
#else
typedef std::string KeyType;
#endif

typedef std::map<KeyType, LTRecord> BranchType;
typedef BranchType::const_iterator BranchIterator;

class LTBranch{
    friend class LTable;
private:
BranchType map;
std::vector<const LTRecord*> vect;
static char signal[10];

#ifdef LTFAST
static KeyType & GetKey(const char * s){ return *(KeyType*)s;}
#else
static KeyType GetKey(const std::string & s){return s;}
#endif
void Insert(const LTRecord & record){
    KeyType key = GetKey(record.signal);
    vect.push_back( &(map[key] = record) );
}

BranchIterator Find(const char * signal)const{
    KeyType key = GetKey(signal);
    return map.find(key);
}

const LTRecord & GetElement(BranchIterator & p)const{
    return p->second;
}

const LTRecord & GetElement(unsigned i)const{
    return *vect[i];
}


public:
    
LTBranch FindVariable(const std::string & var)const;

LTBranch FindBoard(const std::string & board)const;

LTBranch FindChannel(unsigned int channel)const;

LTBranch FindID(int id)const;

LTBranch FindSignal(const std::string & signal)const;

int Size()const{return map.size();}

LTBranch SortByID()const;

const LTRecord *const* GetArray()const;

const LTRecord * operator[](unsigned int n)const;

const LTRecord * operator()(const char * signal)const;

const LTRecord * operator()(const char * board, int channel)const;

LTBranch & operator+=(const LTBranch & branch);

void Print(std::ostream & str = std::cout)const;
};

#endif
