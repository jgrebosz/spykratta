#include "LTable.h"
#include "Run.h"
#include <glob.h>

//
// Class LTable
//

const LTBranch LTable::EmptyBranch;

void LTable::ReadFile(std::istream & file){
    std::string line;
        while(file){
                getline(file, line);
                if(file){
                    if(line.empty()) continue;
                    if(line[0] == '#') continue;
                    LTRecord record;
                    record.Read(line);
                    GetBranch(record.branch).Insert(record);
                }
        }
    
}

void LTable::ReadFile(const char * filename){
    std::ifstream file(filename);
    if(!file){
        throw std::runtime_error("File " + (std::string)filename + " is inaccessible");
    }
    else{
        ReadFile(file);
        name = filename;
    }
}

LTable::LTable(const char * filename){
    if(filename)
        ReadFile(filename);
}
    
const LTBranch & LTable::Branch(const char * branch)const{
    auto i = find(branch);
    return i != end() ? i->second : EmptyBranch;
}

LTBranch LTable::FindVariable(const char * var)const{
    LTBranch branch;
    for(auto i=begin(); i != end(); i++)
        branch += i->second.FindVariable(var);
    return branch;
}

LTBranch LTable::FindBoard(const char * board)const{
    LTBranch branch;
    for(auto i=begin(); i != end(); i++)
        branch += i->second.FindBoard(board);
    return branch;
}

LTBranch LTable::FindChannel(unsigned int channel)const{
    LTBranch branch;
    for(auto i=begin(); i != end(); i++)
        branch += i->second.FindChannel(channel);
    return branch;
}

LTBranch LTable::FindID(int id)const{
    LTBranch branch;
    for(auto i=begin(); i != end(); i++)
        branch += i->second.FindID(id);
    return branch;
}

LTBranch LTable::FindSignal(const char * signal)const{
    LTBranch branch;
    for(auto i=begin(); i != end(); i++)
        branch += i->second.FindSignal(signal);
    return branch;
}

const LTRecord * LTable::operator()(const char * board, int channel)const{
    const LTRecord * record = 0;
    for(auto i=begin(); i != end(); i++)
        if((record = i->second(board, channel)))
            return record;
    return 0;
}

const LTRecord * LTable::operator()(const char * signal)const{
    const LTRecord * record = 0;
    for(auto i=begin(); i != end(); i++){
        if((record = i->second(signal)))
            return record;
    }
    return 0;
}

const LTRecord * LTable::operator[](int n)const{
    int offset = 0;
    for(auto i=begin(); i != end(); i++){
        if(n-offset < i->second.Size())
            return i->second[n-offset];
        offset += i->second.Size();
    }
    return 0;
}

int LTable::Size()const{
    int size = 0;
    for(auto i=begin(); i != end(); i++)
        size += i->second.Size();
    return size;
}

void LTable::Print(std::ostream & str)const{
    for(auto i=begin(); i != end(); i++){
        str << "Branch: " << i->first << std::endl; 
        i->second.Print(str);
    }
}

LTable::FileList LTable::ls(const std::string & path){
    FileList list;
    glob_t globbuf;
    if (!glob(path.c_str(), 0, NULL, &globbuf)) {
        for (unsigned i=0;  i <globbuf.gl_pathc; i++)
            list.push_back(globbuf.gl_pathv[i]);
        globfree(&globbuf);
    }
    return list;
}

void LTable::ReadTables(const char * filename_in){
    Run run(filename_in);
    std::string path = run.GetPath();
    int run_number   = run.GetNumber();
    
    FileList list = ls(run.GetPath() + "*.ltb");
    std::cout << "Reading lookup tables from " << path << ":" << std::endl;
    for(unsigned i=0; i<list.size(); i++){
        int num = Run::GetNumber(list[i].c_str());
//        std::cout << num << " " << run_number << std::endl;
        if(num <= run_number){
             std::cout << list[i] << std::endl;
            ReadFile(list[i].c_str());
        }
    }
}
