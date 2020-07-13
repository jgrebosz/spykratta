#include "calManager.h"
#include "calCoef.h"
#include "Tfile_helper.h"

//istream & zjedz(istream & plik);
//*************************************************************************
calManager::calManager(string filename, int dependent)
{
    if(dependent > 0)
        runDependent = 1;
    else
        runDependent = 0;

    gain   = 1.0;
    offset = 0.0;

    file_read = 1;
    maxDet = -1;

    lookup.clear();
    theCoeff.clear();
    if(this->ReadCoefficients(filename) < 0)
    {
        file_read = 0;
        cout << " Warning! Problem reading coefficients from file " << filename << endl;
    }
}
//*************************************************************************
calManager::calManager(ifstream & s, double how_many_detectors, int  dependent)
{
    if(dependent > 0)
        runDependent = 1;
    else
        runDependent = 0;

    gain   = 1.0;
    offset = 0.0;

    file_read = 1;
    maxDet = -1;

    lookup.clear();
    theCoeff.clear();
    if(ReadCoefficients(s, how_many_detectors) < 0)
    {
        file_read = 0;
        cout << " Warning! Problem reading coefficients " << endl;
    }
}
//*************************************************************************
calManager::~calManager()
{
    // cout << " Destroying ..." << endl;
    lookup.clear();
    theCoeff.clear();
}
//*************************************************************************
// error: negative number
// success: number of (good) lines
int calManager::ReadCoefficients(string filename)
{
    ifstream      inFile;  //> file with the coefficients
    istringstream sLine;   //> a string to handle I/O
    string        aLine;    //> a standard string

    inFile.open(filename.c_str(), ios::in);
    if(!inFile.is_open())
    {
        cout << " calManager::ReadCoefficients( string filename ):  -> Could not open file " << filename << " with the coefficients!" << endl;
        file_read = 0;
        //exit(1);
        return -1;
    }


    theCoeff.clear();

    int      run, det, order, ii;
    double   coeff;
    calCoef* aCoef;

    maxRun = -1;
    maxDet = -1;

    cout << " --> Reading coefficients from file " << filename << " ..." << endl;
    while(getline(inFile, aLine))
    {
        sLine.clear();
        sLine.str(aLine);  // now sLine contains aLine

        if(aLine.length() < 6) continue;    // essentially, an empty line
        //(4 numbers+3 spaces --> 7 is the minimum length)

        // check whether the first non-blank character is a comment sign "#"
        ii = aLine.find_first_not_of(" ");
        if(aLine.at(ii) == '#') continue;

        // now, decode!
        sLine >> run >> det >> order;
        if(run > maxRun) maxRun = run;
        if((runDependent > 0) && (run < 0)) continue;        //> run should be positive!

        if(det > maxDet) maxDet = det;
        if(det < 0) continue;                               //> det should be positive!

        // instance new calCoef object
        theCoeff.push_back(calCoef(det, run));
        aCoef = &theCoeff.back();
        // coefficients
        ii = 0;
        while(sLine >> coeff)
        {
            aCoef->addCoeff(coeff);
            ii++;
        }
        if(ii != order)
        {
            cout << " Found inconsistency in coefficients for run " << run << ", det " << det << endl;
            theCoeff.pop_back();
        }
        else
        {
            /*cout << aLine << endl;*/
        }
    }
    inFile.close();

    //   cout << " --> " << (int)(theCoeff.size()) << " lines of coefficients ";
    //   cout << "read successfully from file " << filename << endl;

    // now construct lookup table
    lookup.clear();
    lookup.resize(runDependent * maxRun + 1);

    for(int ii = 0; ii < (int)(lookup.size()); ii++)
    {
        lookup[ii].resize(maxDet + 1);
        for(int jj = 0; jj < maxDet + 1; jj++)
            lookup[ii][jj] = -1;
    }

    for(int ii = 0; ii < (int)(theCoeff.size()); ii++)
    {
        aCoef = &theCoeff[ii];

        run = runDependent * aCoef->getNRun();

        lookup[run][aCoef->getNDet()] = ii;
    }

    return (int)(theCoeff.size());
}
//************************************************************************
//*************************************************************************
// error: negative number
// success: number of (good) lines
int calManager::ReadCoefficients(ifstream & inFile, double how_many_detectors)
{
    //   istringstream sLine;   //> a string to handle I/O
    //   string        aLine;    //> a standard string

    theCoeff.clear();

    int      run, det, order, ii;
    double   coeff;
    calCoef* aCoef;

    maxRun = -1;
    maxDet = -1;

    for(int i = 0 ; i < how_many_detectors ; i++)
    {
        inFile >> zjedz >>  run ;
        //   if(run == -9999) break;    // end of many linies (originally there were many detectors (kolumns) in a separate file
        inFile >> zjedz >>  det ;
        inFile >>  zjedz >> order;

        //   cout << " run " << run
        //   << ", det "<<   det
        //   << ", order "<< order << endl;

        if(run > maxRun) maxRun = run;

        if((runDependent > 0) && (run < 0))
        {
            Treading_value_exception m;
            m.message = "Run nr should be positive!";
            throw m;
        }
        if(det > maxDet) maxDet = det;
        if(det < 0)
        {
            Treading_value_exception m;
            m.message = " detector nr should be positive!";
            throw m;
        }
        // instance new calCoef object

        theCoeff.push_back(calCoef(det, run));
        aCoef = &theCoeff.back();
        // coefficients
        ii = 0;
        for(int i = 0 ; i <  order ; i++)
        {
            inFile >> zjedz >> coeff ;
            if(! inFile)  break;
            aCoef->addCoeff(coeff);
            ii++;
        }

        if(ii != order)
        {
            cout << " Found inconsistency in coefficients for run " << run << ", det " << det << endl;
            theCoeff.pop_back();
        }
        else
        {
            /*cout << aLine << endl;*/
        }

    }
    //   cout << " --> " << (int)(theCoeff.size()) << " lines of coefficients ";
    //   cout << "read successfully from file " << filename << endl;

    // now construct lookup table
    lookup.clear();
    lookup.resize(runDependent * maxRun + 1);

    for(int ii = 0; ii < (int)(lookup.size()); ii++)
    {
        lookup[ii].resize(maxDet + 1);
        for(int jj = 0; jj < maxDet + 1; jj++)
            lookup[ii][jj] = -1;
    }

    for(int ii = 0; ii < (int)(theCoeff.size()); ii++)
    {
        aCoef = &theCoeff[ii];

        run = runDependent * aCoef->getNRun();

        lookup[run][aCoef->getNDet()] = ii;
    }

    return (int)(theCoeff.size());
}

//******************************************************************
int calManager::setCoeffFile(string filename)
{
    return this->ReadCoefficients(filename);
}
//*************************************************************************
/////////////////////////////////////////////////
/// If coefficients are missing, returns zero
/// (missing file or missing coefficients)
/////////////////////////////////////////////////
double calManager::getCalValue(int run, int det, double raw)
{

//   cout << "calManager::getCalValue ---- maxDet=" << maxDet
//   << ", file_read= " << file_read << ", det= " << det
//   << endl;

    if((det > maxDet) || (det < 0) || (file_read < 1))
    {
        return 0.;
    }
    int index = run * runDependent;
    if(runDependent > 0)
    {
        if((index > maxRun) || (index < 0))
        {
            return 0.;
        }
    }

    index = lookup[index][det];
    if(index < 0)
        return 0.;

    calCoef* aCoef = &theCoeff[index];

    return offset + gain * aCoef->getCalValue(raw);
}
//*************************************************************************
double calManager::getCoeff(int run, int det, int idx)
{
    if((det > maxDet) || (det < 0) || (file_read < 1))
    {
        return 0.;
    }
    int index = run * runDependent;
    if(runDependent > 0)
    {
        if((index > maxRun) || (index < 0))
        {
            return 0.;
        }
    }

    index = lookup[index][det];
    if(index < 0)
        return 0.;

    calCoef* aCoef = &theCoeff[index];

    return aCoef->getCoeff(idx);
}
//*************************************************************************
