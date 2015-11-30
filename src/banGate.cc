#include "banGate.h"
//*****************************************************************
banGate::banGate(int rX, int rY, int det, int pid, double wei)
{
    //   maxBan = NULL;
    //   minBan = NULL;

    resX = rX;
    resY = rY;

    cout << "Reserving " << resX << " arrays for max and for min " << endl;

    try
    {
        //     cout << "Reserving 1 " << resX << " arrays for min " << endl;
        //     minBan = new double[resX];
        //     cout << "Reserving 2 " << resX << " arrays for max " << endl;
        //     maxBan = new double[resX];

//     cout << "Before Resize 1 " << resX << " arrays for max " << endl;
        minBan.resize(resX + 30);
//     cout << "Before Resize 2 " << resX << " arrays for max " << endl;
        maxBan.resize(resX + 30);
//     cout << "After Resize 2 " << resX << " arrays for max " << endl;

    }
    catch(...)
    {
        cout << "Exception? banGate::banGate( int rX, int rY, int det, int pid, double wei ) " << endl;
        exit(4);
    }

    p_id   = pid;
    weight = wei;
    ndet = det;
    thePoints.clear();
}
//*****************************************************************
banGate::~banGate()
{
// cout << " ~banGate() :  - clear the Points - size = " << thePoints.size() << endl;
    thePoints.clear();
    // delete [] minBan; // jg
    // delete []  maxBan; //jg
//cout << " ~banGate() :  - clear minBan "<< endl;
    //minBan.clear();
//cout << " ~banGate() :  - clear maxBan" << endl;
    //maxBan.clear();

// cout << " ~banGate() :  - finished" << endl;
}
//*****************************************************************
void banGate::addPoint(double x, double y)
{
    thePoints.push_back(Vector2D(x, y));
}
//*****************************************************************
int banGate::inside(double x, double y)
{
    if(x < 0. || x > resX || y < 0. || y > resY)
        return 0;

    unsigned int x_i = (int)floor(x);

    if(x_i >= minBan.size()) exit(78);
    if(x_i >= maxBan.size()) exit(79);
    if(x_i >= maxBan.size()) exit(79);

    if(minBan[x_i] < 0.) return 0;

    if(y < minBan[x_i] || y > maxBan[x_i])
        return 0;
    else
        return 1;
}
//*****************************************************************
int banGate::mapBanana()
{
    cout << " --> Mapping banana for detector " << ndet << "..." << endl;
#define COTO cout << "banGate::mapBanana()   line " << __LINE__ << endl;

    Vector2D first = thePoints.front();
    Vector2D last  = thePoints.back();

    // last point should be different than the first one
    // (banana is closed automatically)
    double tolerance = 0.01;
    if((first - last).rho() < tolerance)
    {
        thePoints.pop_back();
        last = thePoints.back();
    }
    // at least 3 points to make a banana!
    int size = (int)(thePoints.size());
    if(size < 3) return -1;

    // start filling the lower and upper limits with nonsense values
    for(unsigned int ii = 0; ii < (unsigned int) resX; ++ii)
    {

        if(  ii >= minBan.size()) exit(80);
        if(ii >= maxBan.size()) exit(81);

        minBan[ii] = -1;
        maxBan[ii] = -1;
    }
    double yy, deltay, dummy;
    int    x_i, x_f, n_steps, step;

    for(int ii = 0; ii < size; ii++)
    {
//     cout << "Loop index over points of banana ii = " << ii << " till size = " << size << endl;



        x_i = (int)floor(thePoints.at(ii).X());
        x_f = (int)floor(thePoints.at((ii + 1) % size).X());

        // same x coordinate --> take y coordinates as lower and upper limits
        if(x_f == x_i)
        {

            if((unsigned int) x_i >= minBan.size()) exit(84);
            if((unsigned int) x_i >= maxBan.size()) exit(85);

            if(thePoints[(ii + 1) % size].Y() > thePoints[ii].Y())
            {
                minBan[x_i] = thePoints.at(ii).Y();
                maxBan[x_i] = thePoints.at((ii + 1) % size).Y();
            }
            else
            {
                maxBan[x_i] = thePoints.at(ii).Y();
                minBan[x_i] = thePoints.at((ii + 1) % size).Y();
            }
            continue;
        }
        // different x coordinates
        n_steps = (int)floor(abs(thePoints[(ii + 1) % size].X() - thePoints[ii].X())) + 1;
        if(thePoints.at((ii + 1) % size).X() > thePoints.at(ii).X())
            step = 1;
        else
            step = -1;
        deltay  = (double)step * (thePoints.at((ii + 1) % size).Y() - thePoints.at(ii).Y()) / ((double)(x_f - x_i));

        // start the cycle to calculate the lower and upper limits
//     cout << "For loop , n_steps " << n_steps << endl;
        for(int jj = 0, xx = x_i; jj < n_steps; xx += step, jj++)
        {
            //    COTO
            //    cout <<  "jj = " << jj << " xx = " << xx << endl;
            yy = thePoints.at(ii).Y() + jj * deltay;
            if(yy < 0.) return -1;

            x_i = xx;
            //cout << "x_i = " << x_i << endl;
            if((unsigned int) x_i >= minBan.size())
            {
                cout << "Alarm the value of x in this bannana is  x_i = " << x_i
                     << " while it should be less than " << minBan.size() << endl;
                exit(5);
            }

            if((unsigned int)  x_i >= maxBan.size())
            {
                cout << "Alarm the value of x in this bannana is  x_i = " << x_i
                     << " while it should be less than " << minBan.size() << endl;
                exit(6);
            }

            if(minBan[x_i] < 0.)
            {
                minBan[x_i] = yy;
            }
            else if(maxBan[x_i] < 0.)
            {
                maxBan[x_i] = yy;
            }
            else
            {
                if(yy < minBan[x_i]) minBan[x_i] = yy;
                if(yy > maxBan[x_i]) maxBan[x_i] = yy;
            }
            if(minBan[x_i] > maxBan[x_i])
            {
                dummy       = maxBan[x_i];
                maxBan[x_i] = minBan[x_i];
                minBan[x_i] = dummy;
            }
        }

    }
    //cout << " Banana map for detector " << ndet << ": " << endl;
    //for(int ii=0; ii<resX; ii++ ) {
    //  if( minBan[ii] >= 0. )
    //    cout << ii << " " << minBan[ii] << " " << maxBan[ii] << endl;
    //}
    return 0;
}
//*****************************************************************
double banGate::getMaxBan(int idx)
{
    if((idx < 0) || (idx > resX))
        return -1.;

    return maxBan.at(idx);

}
//*****************************************************************
double banGate::getMinBan(int idx)
{
    if((idx < 0) || (idx > resX))
        return -1.;

    return minBan.at(idx);

}
//*****************************************************************
// bananas overlap when one of the points of second one is inside first one
int banGate::overlapBan(banGate* otherBan)
{
    for(int ii = 0; ii < resX; ii++)
    {
        if((this->inside(ii, otherBan->getMinBan(ii))) || (this->inside(ii, otherBan->getMaxBan(ii))))
            return 1;
    }

    return 0;
}
//*****************************************************************
