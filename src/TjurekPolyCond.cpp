#include "TjurekPolyCond.h"

TjurekPolyCond::TjurekPolyCond()
{
}


TjurekPolyCond::~TjurekPolyCond()
{
}
//********************************************************
bool TjurekPolyCond::Test(double x, double y)
{

    bool outside = (IsInside(x, y) == 0);
// if we want some statistics ?
    if(outside) return false;
    return true;
}
//********************************************************
bool TjurekPolyCond::IsInside(double xp, double yp)
{
    // Function which returns kTRUE if point xp,yp lies inside the
    // polygon defined by the how_many_points points in arrays x and y, kFALSE otherwise
    // NOTE that the polygon must be a closed polygon (1st and last point
    // must be identical).

// searching nr of intersection of the vertical line with the edges of polygon
    double xint;
    int i;
    int inter = 0;
    for(i = 0; i < how_many_points - 1; i++)
    {
        if(y_points[i] == y_points[i + 1]) continue;
        if(yp <= y_points[i] && yp <= y_points[i + 1]) continue;
        if(y_points[i] < yp && y_points[i + 1] < yp) continue;
        xint = x_points[i] + (yp - y_points[i]) * (x_points[i + 1] - x_points[i]) / (y_points[i + 1] - y_points[i]);
        if((double)xp < xint) inter++;
    }
    // impari nr of intersections i the point is inside the polygon
    if(inter % 2) return true;
    return false;
}

