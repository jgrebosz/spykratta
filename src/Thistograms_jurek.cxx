
//////////////////////////////////////////////////////////////////
// Jurek Grebosz, 6 April  2009
//
// spectra hierarchy - my version
/////////////////////////////////////////////////////////////////

#include "Thistograms_jurek.h"
bool flag_talking_histograms;
//______________________________________________________________________________
int TAxis::FindBin(double x)
{
    // Find bin number corresponding to abscissa x
    //
    // If x is underflow or overflow, attempt to rebin histogram

    int bin;
    if(x < fXmin)                 //*-* underflow
    {
        bin = 0;   // we are going to increment the bin 0 in case of underflow
        return bin;
    }
    else  if(!(x < fXmax))         //*-* overflow  (note the way to catch NaN
    {
        bin = fNbins + 1;
        return bin; // last bin means : overflow
    }
    else   // Normal case
    {
        bin = 1 + int (fNbins * (x - fXmin) / (fXmax - fXmin));
        if(flag_talking_histograms)
        {
            cout << "here" << endl;
        }
    }
    return bin;
}

//***************************************
// incrementing a channel given by double nr. return the bin nr
int TH1::Fill(double value)
{
    int bin = fXaxis.FindBin(value);
    if(bin == 0 || bin > fXaxis.GetNbins())
        return -1;   // error

    // if success
    AddBinContent(bin) ;// increment by one
    return  bin;
}
//***************************************
// incrementing a channel given by double nr. return the bin nr
int TH1::Fill(double value, double weight)
{
    int bin = fXaxis.FindBin(value);
    if(bin == 0 || bin > fXaxis.GetNbins())
        return -1;   // error

    // if success
    AddBinContent(bin, weight) ;// increment by weight
    return  bin;
}
//******************************************************************
void TH1::SetBinContent(int binx, int biny, int binz, content_type content)
{
//   cout << "TH1::SetBinContent(int binx, int biny, int binz, content_type content) " ; //<< endl;
//      cout << "  binx=" << binx
//        << ", biny = " << biny
//        << ", binz = " << binz
//        << ", content=" << content << endl;

    // see convention for numbering bins in TH1::GetBin
    if(binx < 0 || binx > fXaxis.GetNbins() + 1)
        return;
    if(biny < 0 || biny > fYaxis.GetNbins() + 1)
        return;
    if(binz < 0 || binz > fXaxis.GetNbins() + 1)
        return;

    int bin = GetBin(binx, biny, binz);
//     cout << "Before call, bin do takich wsolrzednych =" << bin << endl;
    SetBinContent(bin, content);
//     cout << "After call" << endl;
}
//*****************************************************************
//______________________________________________________________________________
int TH1::GetBin(int binx, int biny, int binz) const
{
    //   -*-*-*-*Return Global bin number corresponding to binx,y,z*-*-*-*-*-*-*
    //           ==================================================
    //
    //      2-D and 3-D histograms are represented with a one dimensional
    //      structure.
    //      This has the advantage that all existing functions, such as
    //        GetBinContent, GetBinError, GetBinFunction work for all dimensions.
    //
    //     In case of a TH1x, returns binx directly.
    //     see TH1::GetBinXYZ for the inverse transformation.
    //
    //      Convention for numbering bins
    //      =============================
    //      For all histogram types: nbins, xlow, xup
    //        bin = 0;       underflow bin
    //        bin = 1;       first bin with low-edge xlow INCLUDED
    //        bin = nbins;   last bin with upper-edge xup EXCLUDED
    //        bin = nbins+1; overflow bin
    //      In case of 2-D or 3-D histograms, a "global bin" number is defined.
    //      For example, assuming a 3-D histogram with binx,biny,binz, the function
    //        int bin = h->GetBin(binx,biny,binz);
    //      returns a global/linearized bin number. This global bin is useful
    //      to access the bin information independently of the dimension.
    //   -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
    int nx, ny, nz;
    if(GetDimension() < 2)
    {
        nx  = fXaxis.GetNbins() + 2;
        if(binx < 0)
            binx = 0;
        if(binx >= nx)
            binx = nx - 1;
        return binx;
    }

    if(GetDimension() < 3)
    {
        nx  = fXaxis.GetNbins() + 2;
        if(binx < 0)
            binx = 0;
        if(binx >= nx)
            binx = nx - 1;
        ny  = fYaxis.GetNbins() + 2;
        if(biny < 0)
            biny = 0;
        if(biny >= ny)
            biny = ny - 1;
        return  binx + nx * biny;
    }

    if(GetDimension() < 4)
    {
        nx  = fXaxis.GetNbins() + 2;
        if(binx < 0)
            binx = 0;
        if(binx >= nx)
            binx = nx - 1;
        ny  = fYaxis.GetNbins() + 2;
        if(biny < 0)
            biny = 0;
        if(biny >= ny)
            biny = ny - 1;
        nz  = fZaxis.GetNbins() + 2;
        if(binz < 0)
            binz = 0;
        if(binz >= nz)
            binz = nz - 1;
        return  binx + nx * (biny + ny * binz);
    }
    return -1;
}
//**********************************************************************************


//_____________________________________________________________________________
Int_t TH1::BufferEmpty(Int_t action)
{
    // Fill histogram with all entries in the buffer.
    // action = -1 histogram is reset and refilled from the buffer (called by THistPainter::Paint)
    // action =  0 histogram is filled from the buffer
    // action =  1 histogram is filled and buffer is deleted
    //             The buffer is automatically deleted when the number of entries
    //             in the buffer is greater than the number of entries in the histogram


#ifdef NIGDY
    // do we need to compute the bin size?
    if(!fBuffer)
        return 0;
    Int_t nbentries = (Int_t)fBuffer[0];
    if(!nbentries)
        return 0;
    Double_t *buffer = fBuffer;
    if(nbentries < 0)
    {
        if(action == 0)
            return 0;
        nbentries  = -nbentries;
        fBuffer = 0;
        Reset();
        fBuffer = buffer;
    }
    if(TestBit(kCanRebin) || (fXaxis.GetXmax() <= fXaxis.GetXmin()))
    {
        //find min, max of entries in buffer
        Double_t xmin = fBuffer[2];
        Double_t xmax = xmin;
        for(Int_t i = 1; i < nbentries; i++)
        {
            Double_t x = fBuffer[2 * i + 2];
            if(x < xmin)
                xmin = x;
            if(x > xmax)
                xmax = x;
        }
        if(fXaxis.GetXmax() <= fXaxis.GetXmin())
        {
            THLimitsFinder::GetLimitsFinder()->FindGoodLimits(this, xmin, xmax);
        }
        else
        {
            fBuffer = 0;
            Int_t keep = fBufferSize;
            fBufferSize = 0;
            if(xmin <  fXaxis.GetXmin())
                RebinAxis(xmin, &fXaxis);
            if(xmax >= fXaxis.GetXmax())
                RebinAxis(xmax, &fXaxis);
            fBuffer = buffer;
            fBufferSize = keep;
        }
    }

    FillN(nbentries, &fBuffer[2], &fBuffer[1], 2);

    if(action > 0)
    {
        delete [] fBuffer;
        fBuffer = 0;
        fBufferSize = 0;
    }
    else
    {
        if(nbentries == (Int_t)fEntries)
            fBuffer[0] = -nbentries;
        else
            fBuffer[0] = 0;
    }
    return nbentries;
#endif

    return 0; // as fake

}
//*****************************************************
//  int TH1I::GetBinContent ( int binx, int biny )
//     {
//         cout << "abstrakcyjna f. TH1::GetBinContent(int, int) " << endl;
//         return 0;
//     }
