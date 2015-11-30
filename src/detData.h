////////////////////////////////////////////////////////////
/// Event by event, data from each detector should be
/// stored into detData structures for on-line processing
///////////////////////////////////////////////////////////
#ifndef DET_DATA
#define DET_DATA

#include <vector>
#include <iostream>
#include <sstream>

using namespace std;

class detData
{
public:
    detData(int ndet, int npar = 1);
    ~detData();

private:
    int valid;

private:
    int n_det;

private:
    int n_par;
    std::vector<double> det_data;

public:
    void reset_detData(int ndet, int npar);

public:
    inline int is_valid()
    {
        return valid;
    };
    inline int num_param()
    {
        return n_par;
    };
    inline int num_det()
    {
        return n_det;
    };

public:
    //inline
    void   set(int, double);
    inline double get(int);
    //const  double operator [] ( int );  // to access det_data[ii] as detData[ii]
    double& operator [](size_t);
    double& operator [](int);

public:
    inline void set_valid(int);

public:
    friend std::ostream &operator << (std::ostream &stream, const detData &orig);
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void detData::set(int index, double value)
{
    if(index < 0 || index >= (int)det_data.size())
    {
        cout << "detData::set      Warning! Outside vector boundaries ... " << endl;
        return;
    }
    det_data[index] = value;
}
//************************************************************************
inline void detData::set_valid(int value)
{
    if(value > 0)
        valid = 1;
    else
        valid = 0;
}
//************************************************************************
inline double detData::get(int index)
{
    if(index < 0 || index >= (int)det_data.size())
    {
        cout << "detData::get    Warning! Outside vector boundaries ... " << endl;
        return -1.;
    }
    return det_data[index];
}
//************************************************************************
#endif
