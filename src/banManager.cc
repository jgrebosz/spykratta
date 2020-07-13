#include "banManager.h"
#include "banGate.h"
//*************************************************************************
banManager::banManager(string filename, int rX, int rY, int pid, double wei)
{
    resX = rX;
    resY = rY;

    p_id   = pid;
    weight = wei;

    file_read = 1;
    detDependent = 1; // by default, multiBananas

    theGates.clear();
    if(this->ReadBananas(filename) < 0)
    {
        file_read = 0;
        cout << " Warning! Problem reading banana gates from file " << filename << "." << endl;
    }
}
//*************************************************************************
banManager::~banManager()
{
    /* for( int ii=0; ii< (int)theGates.size(); ii++ )
        delete  &theGates[ii];*/



    theGates.clear();
}
//*************************************************************************
// error: negative number
// success: number of (good) bananas
int banManager::ReadBananas(string filename)
{
    cout << "F. banManager::ReadBananas(" << filename << ")" << endl;
    ifstream      inFile;  //> file with the coefficients
    istringstream sLine;   //> a string to handle I/O
    string        aLine;    //> a standard string

    inFile.open(filename.c_str(), ios::in);
    if(!inFile.is_open())
    {
        cout << " Could not open file [" << filename << "] with the bananas!" << endl;
        exit(2);
        return -1;
    }
    theGates.clear();

    int      det, ii, index = 0;
    double   px, py;
    banGate* aGate;

    int firstline = 1;

    maxDet = -1;
    cout << " --> Reading gates from file " << filename << " ..." << endl;
    while(getline(inFile, aLine))
    {
        //     cout << "Top of the while loop =================" << endl;
        sLine.clear();
        sLine.str(aLine);  // now sLine contains aLine

        if(aLine.length() < 3) continue;    // essentially, an empty line
        //(2 numbers+1 space --> 3 is the minimum length)

        // check whether the first non-blank character is a comment sign "#"
        ii = aLine.find_first_not_of(" ");
        if(aLine.at(ii) == '#') continue;

        // must recognize from first "good" line multibanana or single banana format
        if(firstline > 0)
        {
            firstline = 0;

            // multibanana
            if(aLine.find("ADC") != string::npos)
            {
                index = aLine.find_last_of("ADC");
                aLine.erase(0, ++index);
                sLine.clear();
                sLine.str(aLine);  // now sLine contains aLine
                sLine >> det;
                if(det > maxDet) maxDet = det;
                if(det < 0) continue;                               //> det should be positive!
                theGates.push_back(banGate(resX, resY, det, p_id, weight));
                continue;
            }
            else
            {
                // single banana
                detDependent = 0;
                maxDet = 1000000;
                theGates.push_back(banGate(resX, resY, index, p_id, weight));
                sLine >> px >> py;
                aGate = &theGates.back();
                aGate->addPoint(px, py);
                continue;
            }
        }

        // regular behaviour for following lines

        // finds new banana
        if(aLine.find("ADC") != string::npos)
        {
            index = aLine.find_last_of("ADC");
            aLine.erase(0, ++index);
            sLine.clear();
            sLine.str(aLine);  // now sLine contains aLine
            sLine >> det;
            if(det > maxDet) maxDet = det;
            if(det < 0) continue;                               //> det should be positive!
            theGates.push_back(banGate(resX, resY, det, p_id, weight));
        }
        else
        {
            aGate = &theGates.back();
            // normal behaviour: find new point
            sLine >> px >> py;
            aGate->addPoint(px, py);
        }
//     cout << "Bottom of while loop " << endl;
    }
    inFile.close();
    cout << " --> " << (int)(theGates.size()) << " bananas read successfully from file " << filename << endl;

    // maps bananas
    for(int ig = 0; ig < (int)(theGates.size()); ig++)
    {
        aGate = &theGates[ig];
        if(aGate->mapBanana() < 0)
        {
            cout << " --> Could not map banana for detector " << aGate->getNDet() << ", aborting ..." << endl;
            return -1;
        }
    }

    // now construct lookup table
    lookup.clear();
    lookup.resize(detDependent * maxDet + 1);

    for(int i = 0; i < (int)(lookup.size()); i++)
    {
        lookup[i] = -1;
    }

    for(int i = 0; i < (int)(theGates.size()); i++)
    {
        aGate = &theGates[i];
        det = detDependent * aGate->getNDet();

        lookup[det] = i;
    }

    return (int)(theGates.size());
}
//*************************************************************************
int banManager::setBananasFile(string filename)
{
    return this->ReadBananas(filename);
}
//*************************************************************************
int banManager::inside(int det, double x, double y)
{
    if((det < 0) || (file_read < 1))
    {
        return 0;
    }
    //if( (det < 0) || (file_read <1) ){
    //  return 0;
    //}
    if((detDependent > 1) && (det > maxDet))
    {
        return 0;
    }
    int index = det * detDependent;

    index = lookup[index];
    if(index < 0)
        return 0;

    banGate* aGate = &theGates[index];

    return aGate->inside(x, y);
}
//*************************************************************************
int banManager::getP_Id(int det)
{
    if((det < 0) || (file_read < 1))
    {
        return -1;
    }
    if((detDependent > 1) && (det > maxDet))
    {
        return -1;
    }
    int index = det * detDependent;

    banGate* aGate = &theGates[index];

    return aGate->getP_Id();
}
//*************************************************************************
double banManager::getWeight(int det)
{
    if((det < 0) || (file_read < 1))
    {
        return -1.;
    }
    if((detDependent > 1) && (det > maxDet))
    {
        return -1.;
    }
    int index = det * detDependent;

    banGate* aGate = &theGates[index];

    return aGate->getWeight();
}
//*************************************************************************
banGate* banManager::getBanGate(int idx)
{
    if((idx < 0) || (idx >= (int)theGates.size()))
        return 0;

    return &theGates[idx];
}
//*************************************************************************

