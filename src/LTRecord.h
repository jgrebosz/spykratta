#ifndef LTRecord_h
#define LTRecord_h

// #define LTFAST

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <string.h>

typedef std::map<std::string, std::vector<double> > LTVariableList;
typedef std::pair<std::string, std::vector<double> > LTVariable;
class LTRecord{
friend class LTBranch;
friend class LTable;

private:
#ifdef LTFAST
    char signal[10];
#else
    std::string signal;
#endif
    std::string branch;
    std::string board;
    unsigned int channel;
    int id;
    std::vector<std::string> var;
    LTVariableList vars;
    
//     static void form(const std::string & board, int i, char * s);
    static void form(const char *  board, int i, char * s);
    static void form(const std::string & board, int i, std::string & s);
//     static void form(const char * board, int i, std::string & s);

//     void AddVariable(const std::string & str){var.push_back(str);}
    void AddVariable(const std::string & str){
        vars[str];
        var.push_back(str);
    }
    void AddParameter(const std::string & str, double value){
        vars[str].push_back(value);
    }
    void SetID(int i){id = i;}
    void SetBranch(const std::string & br){branch = br;}
//     LTRecord & operator=(const LTRecord & rec){
// #ifdef LTFAST
//         strcpy(signal, rec.signal);
// #else
//         signal = rec.signal;
// #endif
//         branch = rec.branch;
//         board = rec.board;
//         channel = rec.channel;
//         id = rec.id;
//         for(auto p = rec.vars.begin(); p != rec.vars.end(); p++){
//          if(vars.find(p->first))   
//                 vars[p->first] = p->second;
//          else
//         }
//         return *this;
//     }
public:
    
    LTRecord(){signal[0] = 0;}
    int GetID()const{return id;}
    int GetChannel()const{return channel;}
    const char * GetBoard()const{return board.c_str();}
#ifdef LTFAST
    const char * GetSignal()const{return signal;}
#else 
    const char * GetSignal()const{return signal.c_str();}
#endif
    const char * GetBranch()const{return branch.c_str();}
    const char * GetVariable(unsigned int i)const{
         return var.at(i).c_str();
//         if(i < vars.size()){
//             auto p = vars.begin();
//             for(unsigned k=0; k != i; k++) p++;
//             return p->first.c_str();
//        }
//        return nullptr;
    }

    unsigned GetVarNumber()const{return var.size();}
    
    unsigned GetParNumber(const std::string & name)const{
        auto p = vars.find(name);
        return (p != vars.end()) ? p->second.size() : 0;
    }
    double GetParameter(const std::string & name, unsigned i)const{
        auto p = vars.find(name);
        return (p != vars.end()) ? p->second.at(i) : 0;
    }
    
    const double * GetParameters(const std::string & name)const{
        auto p = vars.find(name);
        return (p != vars.end()) ? p->second.data() : 0;
    }
    
    void Print(std::ostream & str = std::cout)const;
    void Read(const std::string & line);
};

#endif
