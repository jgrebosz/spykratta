#ifndef TJUREKPOLYCOND_H
#define TJUREKPOLYCOND_H

/**
    @author Jerzy Grebosz <jerzy.grebosz@ifj.edu.pl>
*/

#include <string>
#include <sstream>
#include <vector>
using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////
class TjurekPolyCond
{
    string name;
    int how_many_points;
    vector<double> x_points;
    vector<double> y_points;

    bool enabled;
public:
    TjurekPolyCond();
    TjurekPolyCond(string name_, string title = "Go4 polygon condition")
        : name(name_)
    { }



    ~TjurekPolyCond();

    string printout_points()
    {
        ostringstream  output ;
        for(unsigned int i = 0 ; i < x_points.size() ; i++)
        {
        output << i << " ---> [" << x_points[i] << " " << y_points[i] <<"]\n";
        }
        return output.str();
        
    }

    /** Test if X,Y are inside. */
    //virtual
    bool Test(double x, double y);

    /** Delete old cut and create a new cut with X,Y values. */
    //virtual
    void SetValues(double * x, double * y, int len)
    {
        x_points.clear();
        y_points.clear();
        for(int i = 0; i < len ; i++)
        {
            x_points.push_back(x[i]);
            y_points.push_back(y[i]);
        }
        how_many_points = len;
    }
    //----------------
    void Enable()
    {
        enabled = true;
    }
    string give_name()
    {
        return name;
    }


private:
    bool IsInside(double xp, double yp) ;
};

#endif
