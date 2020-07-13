#include "detData.h"
//************************************************************************
detData::detData(int ndet, int npar)
{
    n_det = ndet;
    n_par = npar;
    valid = 1;

    det_data.resize(n_par);
}
//************************************************************************
detData::~detData()
{
    det_data.clear();
}
//************************************************************************
double& detData::operator [](size_t index)
{
    return det_data[index];
}
//************************************************************************
double& detData::operator [](int index)
{
    return det_data[index];
}
//************************************************************************
std::ostream &operator << (std::ostream &stream, const detData &orig)
{
    std::ostringstream str;
    str << '(' << orig.n_det << ')';
    for(int ii = 0; ii < orig.n_par; ii++)
        str << " " << orig.det_data[ii];
    str << endl;

    return stream << str.str();
}
//************************************************************************
void detData::reset_detData(int ndet, int npar)
{
    n_det = ndet;
    n_par = npar;
    valid = 1;

    det_data.resize(n_par);
}
//******************************************************************************

