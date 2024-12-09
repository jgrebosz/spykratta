#include "LTRecord.h"
#include <sstream>
#include <iomanip>

static std::ostream & operator<<(std::ostream & str, const std::vector<double> & list){
    for(unsigned i=0; i< list.size(); i++)
        str << " " << list[i];
    return str;
}

// void LTRecord::form(const std::string & name, int i, char * s){
//     sprintf(s, "%s_%02d", name.c_str(), i);
// }

void LTRecord::form(const char * name, int i, char * s){
    sprintf(s, "%s_%02d", name, i);
}

void LTRecord::form(const std::string & name, int i, std::string & s){
    std::stringstream ss;
    ss << name << "_" << std::setw(2) << std::setfill('0') << i;
    s = ss.str();
}

void LTRecord::Read(const std::string & line){
    std::stringstream str(line);
    std::string var, lastvar = "";
    double value;
    char * end;
    str >> board >> channel >> branch >> id;
#ifdef LTFAST
     form(board.c_str(), channel, signal);
#else
     form(board, channel, signal);
#endif     
     while(str){
         str >> var;
         if(str) {
             value = std::strtod(var.c_str(), &end);
             if(*end){
                AddVariable(var);
                lastvar = var;
             }
             else
                 AddParameter(lastvar, value);
         }
     }
}
    
void LTRecord::Print(std::ostream & str)const{
     str << signal << "\t" << board << "\t" << channel  << "\t" << branch<< "\t" << id;
     for(unsigned i = 0; i < var.size(); i++){
         str << "\t" << var[i];
         auto list =  vars.find(var[i]);
         str << list->second;
     }
     str << std::endl;
}

