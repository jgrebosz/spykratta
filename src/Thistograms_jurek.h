//////////////////////////////////////////////////////////////////
// Jurek Grebosz, 14 June 2002
//
// spectra hierarchy
/////////////////////////////////////////////////////////////////

#ifndef HISTOGRAMS_JUREK_H
#define HISTOGRAMS_JUREK_H

#include <vector>
#include <string>
#include <cstring>   // for memset
using namespace std ;
#include <iostream>
#include <fstream>
#include <cmath>
//***************************************

typedef double Stat_t;
typedef int Int_t;
typedef double  Double_t;

extern bool flag_talking_histograms;
///////////////////////////////////////////////////////////////////
class TAxis
{
    double fXmin;
    double fXmax;
    int fNbins;

public:
    double GetBinLowEdge(int bin) const
    {
        return fXmin + ((bin - 1) * ((fXmax - fXmin) / fNbins));
    }

    double GetBinUpEdge(int bin) const
    {
        return fXmin + ((bin - 1) * (fXmax - fXmin) / fNbins);
    }

    void Set(int  nbins, double  xlow, double xup)
    {
        // Initialize axis with fix bins
        fNbins   = nbins;
        fXmin    = xlow;
        fXmax    = xup;
        // if (!fParent) SetDefaults();
    }
    //---------------
    int FindBin(double x);
    int GetNbins() const
    {
        return fNbins;
    }

};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
class TH1
{
protected:
    string name;
    vector <int> widmo;

    TAxis  fXaxis;
    TAxis  fYaxis;
    TAxis  fZaxis;
    int fDimension;

    int fNcells; // nr of bins (1D) cells (2D) + U/Overflows
    //double * fBuffer;
    double * fArray; // real place to store spectrum


public:
    TH1(const char *name_, const char *title, int nbins, double xlow, double xup)
        : name(name_)
    {
        if(nbins <= 0)
            nbins = 1;
        fXaxis.Set(nbins, xlow, xup);
        fNcells = fXaxis.GetNbins() + 2;
    }

    virtual   ~TH1()
    {
        // cout << "destructor ~TH1 " << endl;
        // delete [] fArray ;  <---- deleting is made in derived classes
    }

    string GetName()
    {
        return name;
    }

    virtual int Fill(double value);     // incrementing

    //        virtual int Fill ( int x_value, int y_value );
    virtual int Fill(double x_value, double weight);
    //     virtual int SetBinContent ( int x, int y, int co );
    virtual  void Reset()
    {
//         cout << "Empty f. TH1::Reset()   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
    }

    virtual int GetBinContent(int i) = 0;
    //     {
    //         cout << "PUSTA babstrakcyjna f. TH1::GetBinContent(int i = " << i << ") dla widma "
    //         << name
    //         << endl;
    //         return 0;
    //
    //         //return GetBinContent(i, 0);
    //     }

    virtual int GetBinContent(int binx, int biny)      // for 2D spectra only
    {
        //   -*-*-*-*-*Return content of bin number binx, biny
        //             =======================================
        // NB: Function to be called for 2-D histograms only
        // see convention for numbering bins in TH1::GetBin

//                 cout << "TH1::GetBinContent ( int " << binx << ", int " << biny
//                 << ") dla widma " << GetName() << endl;

        Int_t bin = GetBin(binx, biny);
        return GetBinContent(bin);
    }

    virtual TAxis* GetXaxis()
    {
        return &fXaxis;
    }
    virtual TAxis* GetYaxis()
    {
        return &fYaxis;
    }

    virtual void AddBinContent(int) = 0;
    //     {
    //         cout << "abstrakcyjna f. AddBinContent(int) - trzeba implementowac. Widmo: "
    //         << name << endl;
    //     }

    virtual void AddBinContent(int, double w) = 0;
    //     {
    //         cout << "abstrakcyjna f. AddBinContent(int, double) - trzeba implementowac.  Widmo: "
    //         << name
    //         << endl;
    //     }

    typedef double content_type ;
    virtual void SetBinContent(int binx, int biny, int binz, content_type content);
    virtual void SetBinContent(int binx, int biny, content_type content)
    {
//        cout << " virtual int SetBinContent ( int binx, int biny, content_type content )  -> binx=" << binx
//        << ", biny = " << biny
//        << ", content=" << content << endl;
        SetBinContent(binx, biny, 0, content);
    }
    virtual void SetBinContent(int binx, double val)
    {
//       cout << " virtual void SetBinContent ( int binx, double val )  -> " << binx << ", val=" << val << endl;
        //SetBinContent ( binx, 0, 0, val );  // ??????????????????????????????????
        fArray[binx] = val;
    }

    int GetBin(int binx, int biny = 0, int binz = 0) const;

    int GetDimension() const
    {
        return fDimension;
    }

    int GetNbinsX() const
    {
        return fXaxis.GetNbins();
    }
    int GetNbinsY() const
    {
        return fYaxis.GetNbins();
    }
    int GetNbinsZ() const
    {
        return fZaxis.GetNbins();
    }
    Int_t BufferEmpty(Int_t action = 0);
};

////////////////////////////////////////////////////////////////////////////////////////////////////
class  TH1I : public TH1
{
public:
    TH1I(const char *name_, const char *title, int nbins, double xlow, double xup)
        :
        TH1(name_, title, nbins, xlow, xup)
    {
        fDimension   = 1;
        fArray = new double[fNcells];
        // zeroing
        Reset();  //       memset(fArray, 0, fNcells * sizeof(double) );
    }
    ~TH1I()
    {
//       cout << "destructor ~TH1I " << endl;
        delete [] fArray;
    }

    // virtual
    void Reset()
    {
//         cout << "f. TH1I::Reset()" << endl;
        // zeroing
        memset(fArray, 0, fNcells * sizeof(double));
    }
    //______________________________________________________________________________
    int GetBinContent(int bin)    // const
    {
        // see convention for numbering bins in TH1::GetBin
        //        if (fBuffer) ((TH1I*)this)->BufferEmpty();

        if(bin < 0)
            bin = 0;

        if(bin >= fNcells)
            bin = fNcells - 1;

        if(!fArray)
            return 0;

        return int (fArray[bin]);   // why this casting is done?
    }

    void AddBinContent(int bin, double waga)
    {
        //         cout << "f. TH1I::AddBinContent(int, double) - trzeba implementowac.  Widmo: "
        //         << name
        //         << endl;
        if(bin > 0 && bin < fNcells - 1)
            if(fArray[bin] < 2147483647)
                fArray[bin] += waga;
    }
    void AddBinContent(int bin)
    {
        // cout << "abstrakcyjna f. AddBinContent(int) - trzeba implementowac. Widmo: "
        //<< name << endl;
        //   -*-*-*-*-*-*-*-*Increment bin content by 1*-*-*-*-*-*-*-*-*-*-*-*-*-*
        //                   ==========================

        if(fArray[bin] < 2147483647)
            fArray[bin]++;

    }

};
class TAxis;
///////////////////////////////////////////////////////////////////
class  TH2I : public TH1
{
public:
    //_______________________
    TH2I(const char *name, const char *title,
         int nbinsx, double xlow, double xup,
         int nbinsy, double ylow, double yup)
        : TH1(name, title, nbinsx, xlow, xup)
    {
        // see comments in the TH1 base class constructors
//         cout << "Ctor  TH2I " << endl;
        fDimension   = 2;
        //   fScalefactor = 1;
        //   fTsumwy      = fTsumwy2 = fTsumwxy = 0;
        if(nbinsy <= 0)
            nbinsy = 1;
        fYaxis.Set(nbinsy, ylow, yup);
        fNcells      = fNcells * (nbinsy + 2); // fNCells is set in the TH1 constructor

        fArray = new double[fNcells];
//         cout << "zeroing inside Ctor  TH2I " << endl;
        memset(fArray, 0, sizeof(double) * fNcells);
//  Reset();   <--- non existing
    }

    void Reset()
    {
//         cout << "f. TH2I::Reset()" << endl;
        // zeroing
        memset(fArray, 0, fNcells * sizeof(double));
    }
    ~TH2I()
    {
//       cout << "destructor ~TH2I " << endl;
        delete [] fArray;
    }

    int GetBinContent(int bin)
    {
        //         cout << "PUSTA babstrakcyjna f. TH1::GetBinContent(int i = " << i << ") dla widma "
        //         << name
        //         << endl;
        //         return 0;


        if(bin < 0)
            bin = 0;
        if(bin >= fNcells)
            bin = fNcells - 1;
        if(!fArray)
            return 0;
        return int (fArray[bin]);

    }

    //__________________________________________________________________
    void AddBinContent(Int_t bin)
    {
        //*-*-*-*-*-*-*-*-*-*Increment bin content by 1*-*-*-*-*-*-*-*-*-*-*-*-*-*
        //*-*                ==========================

        if(fArray[bin] < 2147483647)
            fArray[bin]++;
    }
    void AddBinContent(Int_t bin, double weight)
    {
        //*-*-*-*-*-*-*-*-*-*Increment bin content by 1*-*-*-*-*-*-*-*-*-*-*-*-*-*
        //*-*                ==========================

        if(fArray[bin] < 2147483647)
            fArray[bin] += weight;
    }

    //********************************************************************
    int  Fill(double x, double y)
    {
        //cout << "2::Fill(double x, double y) " << endl;

        // recalculate x and y to know which bin it is

        //*-*-*-*-*-*-*-*-*-*-*Increment cell defined by x,y by 1*-*-*-*-*-*-*-*-*-*
        //*-*                  ==================================
        //*-*
        //*-* if x or/and y is less than the low-edge of the corresponding axis first bin,
        //*-*   the Underflow cell is incremented.
        //*-* if x or/and y is greater than the upper edge of corresponding axis last bin,
        //*-*   the Overflow cell is incremented.
        //*-*
        //*-* If the storage of the sum of squares of weights has been triggered,
        //*-* via the function Sumw2, then the sum of the squares of weights is incremented
        //*-* by 1 in the cell corresponding to x,y.
        //*-*
        //*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

        //if (fBuffer) return BufferFill(x,y,1);

        //    Int_t binx, biny, bin;
        //    fEntries++;
        int binx = fXaxis.FindBin(x);
        int biny = fYaxis.FindBin(y);
        int bin  = biny * (fXaxis.GetNbins() + 2) + binx;
        if(flag_talking_histograms)
        {
            cout << "Before incrementation , binx = "
                 << binx
                 << ", biny = " << biny
                 << ", bin = " << bin << endl;
        }
        AddBinContent(bin);
        //    if (fSumw2.fN) ++fSumw2.fArray[bin];
        if(binx == 0 || binx > fXaxis.GetNbins())
        {
            //       if (!fgStatOverflows)
            return -1;
        }
        if(biny == 0 || biny > fYaxis.GetNbins())
        {
            //       if (!fgStatOverflows)
            return -1;
        }
        //    ++fTsumw;
        //    ++fTsumw2;
        //    fTsumwx  += x;
        //    fTsumwx2 += x*x;
        //    fTsumwy  += y;
        //    fTsumwy2 += y*y;
        //    fTsumwxy += x*y;
        return bin;

    }
};

///////////////////////////////////////////////////////////////////////////////
#endif // HISTOGRAMS_JUREK_H


