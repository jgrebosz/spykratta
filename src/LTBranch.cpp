#include "LTBranch.h"

char LTBranch::signal[10] = "";

LTBranch LTBranch::FindVariable(const std::string & var)const{
    LTBranch branch;
        for(int i=0; i<Size(); i++){
            auto rec = GetElement(i);
//            rec.Print();
                if(rec.vars.find(var) != rec.vars.end()){
                    branch.Insert(rec);
                }
        }
    return branch;
}

LTBranch LTBranch::FindBoard(const std::string & board)const{
    LTBranch branch;
        for(int i=0; i<Size(); i++)    
        if(GetElement(i).board == board)
            branch.Insert(GetElement(i));
    return branch;
}

LTBranch LTBranch::FindChannel(unsigned int channel)const{
    LTBranch branch;
    for(int i=0; i<Size(); i++)    
        if(GetElement(i).channel == channel)
            branch.Insert(GetElement(i));
    return branch;
}

LTBranch LTBranch::FindID(int id)const{
    LTBranch branch;
    for(int i=0; i<Size(); i++)    
        if(GetElement(i).id == id)
            branch.Insert(GetElement(i));
    return branch;
}

LTBranch LTBranch::SortByID()const{
    LTBranch branch;
    std::map<int, LTRecord> map;
    for(int i=0; i<Size(); i++)    
        map[GetElement(i).id] = GetElement(i);
    
    for(auto p = map.begin(); p != map.end(); p++)
        branch.Insert(p->second);
    
    return branch;
}


LTBranch LTBranch::FindSignal(const std::string & signal)const{
    LTBranch branch;
    for(int i=0; i<Size(); i++)    
#ifdef LTFAST
        if(strcmp(GetElement(i).signal, signal.c_str()) != 0)
#else
        if(GetElement(i).signal == signal)
#endif
        branch.Insert(GetElement(i));
    return branch;
}

const LTRecord *const* LTBranch::GetArray()const{
    return vect.data();
}
const LTRecord * LTBranch::operator[](unsigned int n)const{
    return vect[n];
}

const LTRecord * LTBranch::operator()(const char * signal)const{
    auto p = Find(signal);
        return p != map.end() ? &GetElement(p) : nullptr;
}

const LTRecord * LTBranch::operator()(const char * board, int channel)const{
    LTRecord::form(board, channel, signal);
    return operator()(signal);
}

LTBranch & LTBranch::operator+=(const LTBranch & branch){
//    for(auto i=branch.begin(); i != branch.end(); i++)
    for(int i=0; i< branch.Size(); i++)
        Insert(*branch[i]);
    return *this;
}

void LTBranch::Print(std::ostream & str)const{
//     for(auto i=map.begin(); i != map.end(); i++)
//         GetElement(i).Print(str);
        
     for(int i=0; i<Size(); i++)
         GetElement(i).Print(str);
    }

