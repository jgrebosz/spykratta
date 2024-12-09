#ifndef Run_h
#define Run_h
#include <string>

class Run{
private:
static const char delim = '/';
static constexpr const char * digits = "0123456789";

std::string path;
std::string name;
std::string type;
int num;

public:
static std::string GetPath(const char * filename_in);
static std::string GetName(const char * filename_in);
static int GetNumber(const char * filename_in, bool cut = 1);
std::string GetType(const char * filename_in);

std::string GetPath(){return path;}
std::string GetName(){return name;}
int GetNumber(){return num;}
std::string GetType(){return type;}

Run(const char * filename_in);

};

#endif
