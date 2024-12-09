#include "Run.h"

Run::Run(const char * filename_in){
     std::string str(filename_in);
        int last_slash = str.find_last_of(delim) + 1;
        path = str.substr(0, last_slash);
        name = str.substr(last_slash);
        num = GetNumber(name.c_str(), 0);
        type = GetType(name.c_str());
}

std::string Run::GetPath(const char * filename_in){
     std::string path(filename_in);
    int last_slash = path.find_last_of(delim) + 1;
    return path.substr(0, last_slash);
}

std::string Run::GetName(const char * filename_in){
     std::string path(filename_in);
    int last_slash = path.find_last_of("/") + 1;
    return path.substr(last_slash);
}

std::string Run::GetType(const char * filename_in){
     std::string path(filename_in);
    int last_slash = path.find_last_of(".") + 1;
    return path.substr(last_slash);
}

int Run::GetNumber(const char * filename_in, bool cut){
    std::string filename = cut ? GetName(filename_in) : filename_in;
    auto n1 = filename.find_first_of(digits);
    if(n1 == std::string::npos)
        return -1;
    auto n2 = filename.find_last_of(digits) + 1;
    int len = n2 - n1;
//     printf("%u %u\n",n1,n2);
    return std::stoi(filename.substr(n1, len));
}
