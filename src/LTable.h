#ifndef LTable_h
#define LTable_h

#include "LTBranch.h"
#include <fstream>

/// class LTable

class LTable: private std::map<std::string, LTBranch>{
private:

const static LTBranch EmptyBranch;
typedef std::vector<std::string> FileList;

LTBranch & GetBranch(const std::string & branch){
    return std::map<std::string, LTBranch>::operator[](branch);
}

static FileList ls(const std::string & path);
// static std::string GetPath(const char * filename_in);


public:
std::string name;

void ReadFile(std::istream & file);
void ReadFile(const char * filename);
void ReadTables(const char * filename_in);

LTable(const char * filename = 0);
    
const LTBranch & Branch(const char * branch)const;

LTBranch FindVariable(const char * var)const;

LTBranch FindBoard(const char * board)const;

LTBranch FindChannel(unsigned int channel)const;

LTBranch FindID(int id)const;

LTBranch FindSignal(const char * signal)const;

int Size()const;

const LTRecord * operator()(const char * board, int channel)const;

const LTRecord * operator()(const char * signal)const;

const LTRecord * operator[](int n)const;


void Print(std::ostream & str = std::cout)const;
};

#endif
