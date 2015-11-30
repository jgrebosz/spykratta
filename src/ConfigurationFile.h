///////////////////////////////////////////////////////
//// Taken from A.Latina                          /////
///////////////////////////////////////////////////////
#ifndef CONFIGURATION_FILE_CLASS
#define CONFIGURATION_FILE_CLASS

#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>

#include <sys/types.h>
#include <sys/stat.h>
#include <cstdlib>
#include <cctype>
/////////////////////////////////////////////////////////////////////////////////////////////
class ConfigurationFile
{
    struct _item
    {
        std::string token;
        std::string value;

        enum _type
        {
            VALUE,
            COMMENT,
            BLANK
        } type;

        _item() {};
        _item(std::string _token, std::string _value, _type __type)
        {
            std::transform(_token.begin(), _token.end(), _token.begin(), ::tolower);

            token = _token;
            value = _value;
            type = __type;
        }

        bool operator == (std::string _token) const
        {
            return token == _token;
        };
        bool operator != (std::string _token) const
        {
            return token != _token;
        };

    };

    std::list<_item> items;

    typedef std::list<_item>::const_iterator  item_const_iterator;
    typedef std::list<_item>::iterator        item_iterator;

public:

//  ConfigurationFile(const char *file_name ) { read_file(file_name); };
    ConfigurationFile(const std::string file_name)
    {
        read_file(file_name);
    };
    ConfigurationFile(const std::string file_name, char *token1, ...);
    ConfigurationFile() {};

    void read_file(const char *file_name);
    void write_file(const char *file_name) const;

    void read_file(std::string file_name);
    void write_file(const std::string file_name) const;

    void clear()
    {
        items.clear();
    };

    void set_defaults(char *token1, ...);

    std::string operator [](std::string token) const
    {
        item_const_iterator i;

        std::transform(token.begin(), token.end(), token.begin(), ::tolower);

        if((i = std::find(items.begin(), items.end(), token)) == items.end())
        {
            _item item(token, "", _item::VALUE);
            return "";
        }
        return i->value;
    }

    std::string &operator [](std::string token)
    {
        item_iterator i;

        std::transform(token.begin(), token.end(), token.begin(), ::tolower);

        if((i = std::find(items.begin(), items.end(), token)) == items.end())
        {
            _item item(token, "", _item::VALUE);
            items.push_back(item);
            return items.back().value;
        }
        return i->value;
    }

    //const char  *get_string(std::string token, const char *_default = NULL ) const;
    const char    *get_string(std::string token, std::string _default = "") const;


    double     get_double(std::string token, double _default = 0.0) const;
    int    get_int(std::string token, int _default = 0) const;
    bool   get_bool(std::string token, bool _default = false) const;

    void set_string(std::string token, const std::string &value)
    {
        item_iterator i;

        std::transform(token.begin(), token.end(), token.begin(), ::tolower);

        i = std::find(items.begin(), items.end(), token);

        if(i != items.end())
        {
            i->value = value;
            i->type = _item::VALUE;
        }
        else
        {
            _item item(token, value, _item::VALUE);
            items.push_back(item);
        }
    }

    void set_double(const std::string &token, double value)
    {
        std::ostringstream str;
        str << value;
        set_string(token, str.str());
    }

    void set_int(const std::string &token, int value)
    {
        std::ostringstream str;
        str << value;
        set_string(token, str.str());
    }

    void set_bool(const std::string &token, bool value)
    {
        set_string(token, value ? "true" : "false");
    }

    void set_comment(const std::string &comment)
    {
        _item item;

        item.token = "";
        item.value = comment;
        item.type = _item::COMMENT;
        items.push_back(item);
    }

    void add_blank_line()
    {
        _item item;

        item.token = "";
        item.type = _item::BLANK;
        items.push_back(item);
    }

    friend std::ostream &operator << (std::ostream &stream, const ConfigurationFile &cfg);
    friend std::istream &operator >> (std::istream &stream, ConfigurationFile &cfg);

};

#endif /* CONFIGURATION_FILE_CLASS */
