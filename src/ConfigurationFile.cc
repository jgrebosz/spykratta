#include <cstdarg>

#include "ConfigurationFile.h"
//*********************************************************************************
ConfigurationFile::ConfigurationFile(const std::string file_name, char *token1, ...)  // sets the defaults
{
    va_list ap;

    va_start(ap, token1);

    for(; token1 != NULL; token1 = va_arg(ap, char *))
        set_string(token1, va_arg(ap, char *));

    va_end(ap);

    read_file(file_name);
}
//*********************************************************************************
void ConfigurationFile::read_file(std::string file_name)
{
    std::ifstream file(file_name.c_str());

    if(file)
    {
        file >> *this;
        file.close();
    }
}
//*********************************************************************************
void ConfigurationFile::write_file(const std::string file_name) const
{
    std::ofstream file(file_name.c_str());

    if(file)
    {
        file << *this;
        file.close();
    }
}
//*********************************************************************************
void ConfigurationFile::set_defaults(char *token1, ...)
{
    va_list ap;

    va_start(ap, token1);

    for(; token1 != NULL; token1 = va_arg(ap, char *))
    {
        set_string(token1, va_arg(ap, char *));
    }

    va_end(ap);
}
//*********************************************************************************
//const char *ConfigurationFile::get_string(std::string token, const char *_default ) const
const char *ConfigurationFile::get_string(std::string token, std::string _default) const
{
    std::list<_item>::const_iterator i;

    std::transform(token.begin(), token.end(), token.begin(), ::tolower);

    i = std::find(items.begin(), items.end(), token);

    if(i != items.end())
        return i->value.c_str();

    return _default.c_str() ;
}
//*********************************************************************************
double ConfigurationFile::get_double(std::string token, double _default) const
{
    std::list<_item>::const_iterator i;

    std::transform(token.begin(), token.end(), token.begin(), ::tolower);

    i = std::find(items.begin(), items.end(), token);

    if(i != items.end()) return atof(i->value.c_str());

    return _default;
}
//*********************************************************************************
int ConfigurationFile::get_int(std::string token, int _default) const
{
    std::list<_item>::const_iterator i;

    std::transform(token.begin(), token.end(), token.begin(), ::tolower);

    i = std::find(items.begin(), items.end(), token);

    if(i != items.end()) return atoi(i->value.c_str());

    return _default;
}
//*********************************************************************************
bool ConfigurationFile::get_bool(std::string token, bool _default) const
{
    std::list<_item>::const_iterator i;

    std::transform(token.begin(), token.end(), token.begin(), ::tolower);

    i = std::find(items.begin(), items.end(), token);

    if(i == items.end()) return _default;

    if(i->value == "0")  return false;
    if(i->value == "1")  return true;

    std::string _value = i->value;

    std::transform(_value.begin(), _value.end(), _value.begin(), ::tolower);

    if(i->value == "true")  return true;
    if(i->value == "false") return false;

    return _default;
}
//*********************************************************************************
std::ostream &operator << (std::ostream &stream, const ConfigurationFile &cfg)
{
    std::list<ConfigurationFile::_item>::const_iterator i;

    for(i = cfg.items.begin(); i != cfg.items.end(); i++)
    {
        switch(i->type)
        {
        case ConfigurationFile::_item::VALUE:
            stream << i->token << " = " << i->value << '\n';
            break;
        case ConfigurationFile::_item::COMMENT:
            stream << "# " << i->value << '\n';
            break;
        case ConfigurationFile::_item::BLANK:
            stream << '\n';
            break;
        }
    }
    return stream;
}
//*********************************************************************************
std::istream &operator >> (std::istream &stream, ConfigurationFile &cfg)
{
    if(stream)
    {
        while(!stream.eof())
        {
            char buffer[256];

            stream.getline(buffer, sizeof(buffer) / sizeof(char));

            std::string line = buffer;

            if(line != "")     // parsing: token = value [ # | // | [bla bla] are considered as remarks]
            {
                std::string::size_type pos;

                if((pos = line.find("#"))  != std::string::npos)   line.erase(pos);
                if((pos = line.find("//")) != std::string::npos)   line.erase(pos);
                if((pos = line.find("[")) != std::string::npos)    line.erase(pos);

                line.erase(remove_if(line.begin(), line.end(), isspace), line.end());

                if((pos = line.find("=")) != std::string::npos)
                {
                    std::string token = line.substr(0, pos);
                    std::string value = line.substr(pos + 1, std::string::npos);

                    cfg.set_string(token, value);
                }
            }
        }
    }
    return stream;
}
//*********************************************************************************
