// spectra  hierarchy

#include <memory>
#include "spectrum.h"
#include "TIFJAnalysis.h"

// #define COTO  cout<<"plik "<<__FILE__<<" linia "<<__LINE__<<endl ;


// static pointer to the descritor of the spectra
Cspectra_descriptor * spectrum_abstr::spectra_descriptor;

//*****************************************************************************

spectrum_1D::spectrum_1D ( string name_root, int nr_of_bins,
                           double first_channel,
                           double last_channel, string folder, string note,
                           const string list_incrementers )

{
    //cout << "Constructor for Spectrum: " << name_root << endl ;
    //last_channel_paper = 66 ; //ast_channel;
    max_bin = 0 ;
    //   cout << "name_root=" << name_root
    //   << ", nr_of_bins="  << nr_of_bins
    //   << ", first_channel=" << first_channel
    //   << ", last_channel=" << last_channel
    //   << ", folder=" << folder
    //   << ", note=" << note
    //   << ", list_incrementers=" << list_incrementers << endl;

    create ( name_root, nr_of_bins,
             first_channel,
             last_channel,
             folder,
             note,
             list_incrementers ) ;
}
//*******************************************************************************
spectrum_1D::~spectrum_1D()
{
    //     cout << "Spectrum destructor running " << endl ;
    //save_to_disk() ;

    // if we use the following instruction, the spectrum will not be saved
    // into xxxASF.root file (auto save file)

    // TGo4Analysis::Instance() ->  RemoveHistogram(spectrum_name);

    //cout << "Spectrum: " << give_name() << " incremented "
    //     << give_statistic() << " times " << endl ;


    // the root part of spectrum was registered, so we do not have to delete it ?????????????
    // As there is no root here - we must delete it  !!!!!
    //delete ptr_root_spectrum;
}
//**************************************************************************
/** No descriptions */
spectrum_1D::spectrum_1D()
{}
//**************************************************************************
/** No descriptions */
//void spectrum_1D::create(char * name_root, char * title, int nr_of_bins,
//                         double first_channel,
//                         double last_channel, const char * folder, const char * note)
void spectrum_1D::create ( string name_root, int nr_of_bins,
                           double first_channel,
                           double last_channel, string folder, string note,
                           string list_of_incrementers )
{
    //     cout << "Constructor for Spectrum: " << name_root << endl ;

    // perhaps such a spectrum already exist in the memory (made by the previous
    // session) ?

    //     cout << "Does it already exist ? " << endl ;
    TH1 * wskaznik = RisingAnalysis_ptr -> GetHistogram ( name_root.c_str() ) ;

    if ( wskaznik != 0 )
    {
        // such a root spectrum already exist in root memory,
        // so we have to delete it

        //         cout << "Yes, removing it ." << endl ;
        RisingAnalysis_ptr -> RemoveHistogram ( name_root ) ;

        //         cout << "Spectrum already existed, so deleting it "    << endl ;
    }

    //     COTO;
    // here we could check if there are special wishes about the binning
    {
        // local
        string fname = string ( "./my_binnings/" ) + name_root + ".spc.binning" ;
        ifstream plik ( fname.c_str() );
        if ( plik )
        {
            plik >> zjedz >> nr_of_bins
                    >> zjedz >> first_channel
                    >> zjedz >> last_channel ;
        }
        plik.close();
    } // local

    // creating the spectrum
    //     cout << "Spectrum new TH1D..." << endl ;

    // S is Short, D is Double  I (long int)
    // ptr_root_spectrum  = new TH1S (name_root, title, nr_of_bins, first_channel,  last_channel);
    ptr_root_spectrum  = new TH1I ( name_root.c_str(),
                                    nr_of_bins, first_channel,  last_channel );
    //         cout << "Spectrum created brand-new " << endl ;

    reset_incrementers() ;

    statistics_of_increments = 0 ;
    spectrum_length = nr_of_bins ;
    min_bin = first_channel;

    ptr_condition_flag_is_true = 0 ;
    // COTO;
    RisingAnalysis_ptr ->  AddHistogram ( ptr_root_spectrum, folder.c_str() );
    //--------------------------- for 1D spectra
    //  notice = note ;
    //   COTO;
    //   cout << "before adding entry" << endl;
    spectra_descriptor->add_entry ( name_root + ".spc",
                                    nr_of_bins, first_channel,  last_channel,
                                    note.c_str() , list_of_incrementers );

    // COTO;
    // if somebody wants to analyse the specra

    int continuation = 1 ;
    if ( continuation )
    {
        // look at the disk if if does not exist
        cout << ":" << flush ;
        read_from_disk() ;
    }
    else
    {
        save_to_disk() ;
        cout << "." << flush ;
    }
    // COTO;
    max_bin = last_channel;

}
//********************************************************* ********************
void spectrum_1D::increment_yourself()
{

    //cout << "Function incerment yourself, (how many incrementers="
    //     << how_many_incrementers << endl ;

    if ( can_we_increment() == false )
    {
        //cout << "INcrement not possible" << endl ;
        return ;
    }


    // look at the right data
    for ( unsigned int i = 0 ; i < int_incrementers_vector.size() ; i++ )
    {
        //cout << "Checking the candidate nr " << i << endl ;

        int value = * ( int_incrementers_vector[i] );
        //cout << "it has a value  " << value << endl ;

        //can be nagative !!!
        {
            //cout << "just before incrementation " << value << endl ;
            ptr_root_spectrum->Fill ( value );
            //statistics();

            // cout << "                       YES, incremented " << value << endl ;
        }

    }


    for ( unsigned int i = 0 ; i < double_incrementers_vector.size() ; i++ )
    {
        //cout << "Checking the candidate nr " << i << endl ;

        double value = * ( double_incrementers_vector[i] );
        //cout << "it has a value  " << value << endl ;

        //can be nagative !!!
        {
            //cout << "just before incrementation " << value << endl ;
            ptr_root_spectrum->Fill ( value );
            //statistics();

            // cout << "                       YES, incremented " << value << endl ;
        }

    }


}
//*****************************************************************************
void spectrum_1D::int_incrementer_X ( int * candidate )
{

    //cout << "F. : incrementer_X" << endl;
    int_incrementers_vector.push_back ( candidate );

}


//*****************************************************************************
void spectrum_1D::double_incrementer_X ( double * candidate )
{

    //cout << "F. : incrementer_X" << endl;
    double_incrementers_vector.push_back ( candidate );

}



//**************************************************************************
void spectrum_1D::int_incrementer_Y ( int * /*candidate*/ )
{
    /* empty */
    cout << "Incrementer Y has no sense for 1D spectrum, any key... " << endl ;
    int liczba ;
    cin >> liczba ;
}
//**************************************************************************
void spectrum_1D::double_incrementer_Y ( double * /*candidate*/ )
{
    /* empty */
    cout << "Incrementer Y has no sense for 1D spectrum, any key... " << endl ;
    int liczba ;
    cin >> liczba ;
}
//***********************************************************************
// char * spectrum_1D::give_name()
// {
//   return ptr_root_spectrum->GetName(); ;
// }
//************************************************************************
void spectrum_1D::save_to_disk()
{
    //#define  SPECTRA_IN_ASCII
#ifdef SPECTRA_IN_ASCII
    // this is 1D spectrum, so we save it in ASCII
    string name_s = string ( "spectra/" ) + ptr_root_spectrum->GetName();
    name_s += ".asc";

    //cout << "Saving 1D spectrum " << name_s << endl ;
    ofstream plik ( name_s.c_str(), ios::trunc );

    if ( !plik )
    {
        cout << "Error while saving spectrum "
             << name_s
             << endl ;
    }

    // we get value from the bin, not from "channel" so we do not
    // care about range of channels, for ex. -200 +200.

    // warning: in ROOT the bin nr 0 contains: underflow
    //                  the bin nr 1 contains: first bin of spectrum
    //                  the bin nr nbin+1 contains: overflow
    for ( int i = 1 ; i <= spectrum_length ; i ++ ) // see warning above
    {
        // int value =    (int)  ptr_root_spectrum->GetBinContent(i);
        // double bin_position = ptr_root_spectrum->GetBinLowEdge(i) ;

        plik
                << ptr_root_spectrum->GetBinLowEdge ( i )       // bin_position
                << "\t\t"
                << ( ( int )  ptr_root_spectrum->GetBinContent ( i ) ) // value
                << "\n" ;
    }
    plik << flush ;
    plik.close();
    //cout << "Succes " << endl;
#else

    // we save it in binary
    string name_s = string ( "spectra/" ) + ptr_root_spectrum->GetName();
    name_s += ".spc";

    //     cout << "Saving 1D spectrum " << name_s << endl ;
    ofstream plik ( name_s.c_str(), ios::trunc | ios::binary );

    if ( !plik )
    {
        cout << "Error while saving spectrum - cant open file "
             << name_s << " for writing"
             << endl ;
    }

    // we get value from the bin, not from "channel" so we do not
    // care about range of channels, for ex. -200 +200.

    // warning: in ROOT the bin nr 0 contains: underflow
    //                  the bin nr 1 contains: first bin of spectrum
    //                  the bin nr nbin+1 contains: overflow


    // the system which I am using now is
    // 1. double word - bins
    // 2. double word - left ede of the first bin
    // 3. double word - right ede of the last bin

    //  double left_edge = ptr_root_spectrum->GetBinLowEdge(1);
    //  double waga = ptr_root_spectrum->GetBinLowEdge(2) -
    //                    ptr_root_spectrum->GetBinLowEdge(1);;
    //
    //
    //  plik.write((char*)&left_edge, sizeof( left_edge));
    //  plik.write((char*)&waga, sizeof( waga));


    double bins = ptr_root_spectrum->GetNbinsX() ;
    double left_edge = ptr_root_spectrum->GetXaxis()->GetBinLowEdge ( 1 );
    double right_edge = ptr_root_spectrum->GetXaxis()->GetBinUpEdge ( ( int ) bins + 1 );

    plik.write ( ( char* ) &bins, sizeof ( bins ) );
    plik.write ( ( char* ) &left_edge, sizeof ( left_edge ) );
    plik.write ( ( char* ) &right_edge, sizeof ( right_edge ) );

#ifdef NIGDY

    cout
            << "F. spectrum_1D::save_to_disk()  "
            << "spectrum " << name_s
            << ", bins = " << bins
            << ", left edge = " << left_edge
            << ", right edge = " << right_edge
            << endl;
#endif

    for ( int i = 1 ; i <= spectrum_length ; i ++ ) // see warning above
    {

        //double bin_position = ptr_root_spectrum->GetBinLowEdge(i) ;
        //plik.write((char*)&bin_position, sizeof(bin_position));

        int value = ( int )  ptr_root_spectrum->GetBinContent ( i );
        //        if(i < 15)
        //         cout << "For channel "  << i << "  saving value = " << value << endl;
        plik.write ( ( char* ) &value, sizeof ( value ) );
    }
    //plik << flush ;
    plik.close();
    //cout << "Succes " << endl;


#endif
}
//************************************************************************
void spectrum_1D::manually_inc_by ( long int chan, int value )
{
    ptr_root_spectrum->Fill ( chan, value );
    //statistics(value);

}
//************************************************************************
//************************************************************************
spectrum_2D::spectrum_2D ( string  name_root,
                           int nr_of_bins_x, double first_channel_x, double last_channel_x,
                           int nr_of_bins_y, double first_channel_y, double last_channel_y,
                           const string  folder, const string  note, const string  list_of_incrementers )
{

    //     cout << "Constructor for 2D Spectrum: " << name_root << endl ;

    // perhaps such a spectrum already exist in the memory (made by the previous
    // session) ?

    //cout << "Does " << name_root << " already exist ? " << endl ;
    TH1 * wskaznik = RisingAnalysis_ptr -> GetHistogram ( name_root.c_str() ) ;

    if ( wskaznik != 0 )
    {
        // such a root spectrum already exist in root memory,
        // so we have to delete it

        //         cout << "Yes, removing it ." << endl ;
        RisingAnalysis_ptr -> RemoveHistogram ( name_root.c_str() ) ;

        //         cout << "Spectrum already existed, so deleting it "    << endl ;
    }
    //     if(flag_talking_histograms)
    //     {
    //         cout <<  "breakpoint" << endl;
    //     }
    string fname = string ( "./my_binnings/" ) + name_root + ".mat.binning" ;
    ifstream plik ( fname.c_str() );
    if ( plik )
    {
        plik >> zjedz >> nr_of_bins_x
                >> zjedz >> first_channel_x
                >> zjedz >> last_channel_x ;
        plik >> zjedz >> nr_of_bins_y
                >> zjedz >> first_channel_y
                >> zjedz >> last_channel_y ;
    }
    plik.close();


    // creating the spectrum
    //cout << "Spectrum new TH2I..." << endl ;
    ptr_root_spectrum  = new TH2I ( name_root.c_str(),
                                    nr_of_bins_x,  first_channel_x,  last_channel_x,
                                    nr_of_bins_y,  first_channel_y,  last_channel_y
                                    );
    //cout << "Spectrum created brand-new " << endl ;



    statistics_of_increments = 0 ;
    x_spectrum_length = nr_of_bins_x ;
    x_min_bin = first_channel_x;

    y_spectrum_length = nr_of_bins_y ;
    y_min_bin = first_channel_y;


    RisingAnalysis_ptr ->  AddHistogram ( ptr_root_spectrum, folder.c_str() );

    //---------------------------  for 2D spectra

    spectra_descriptor-> add_entry ( name_root + ".mat",
                                     nr_of_bins_x,  first_channel_x,  last_channel_x,
                                     nr_of_bins_y,  first_channel_y,  last_channel_y,
                                     note.c_str(), list_of_incrementers.c_str() );


    // look at the disk if if does not exist
    int continuation = 1 ;
    if ( continuation )
    {
        //         cout << "Readinfg from disk - for cxontitunation" << endl;
        cout << "[#]" << flush ;
        read_from_disk();


    }
    else
    {
        save_to_disk() ;
        cout << "[.]" << flush ;
    }

}
//*******************************************************************************
spectrum_2D::~spectrum_2D()
{
    // perhaps the analysis class will save it earlier ?
    //cout << "Spectrum destructor running " << endl ;

    // ptr_root_spectrum ->Write();     -< error, no file open

    //save_to_disk() ;

    // if we use the following instruction, the spectrum will not be saved
    // into xxxASF.root file (auto save file)

    // TGo4Analysis::Instance() ->  RemoveHistogram(spectrum_name);


    //  cout << "Spectrum: " << give_name() << " incremented "
    //       << give_statistic() << " times " << endl ;



    //     the root part of spectrum was registered, so we do not have to delete it ?????????????
}
//**********************************************************************************

//************************************************************************
void spectrum_2D::save_to_disk()
{
    // this is 2D spectrum, so we save it in binary
    string  name_s = "spectra/" ;
    name_s += ptr_root_spectrum->GetName();
    name_s += ".mat";
    // cout << "Saving 2D spectrum " << name_s << endl ;
    ofstream plik ( name_s.c_str(), ios::binary );

    if ( !plik )
    {
        cout << "Error while saving spectrum "
             << name_s
             << endl ;
    }


//    cout << "is saving in short int = "
//         <<  (2*y_spectrum_length * x_spectrum_length )/1024
//          << " KB  " << endl ;


    // we get value from the bin, not from "channel" so we do not
    // care about range of channels, for ex. -200 +200.

    // warning: in ROOT the bin nr 0 contains: underflow
    //                  the bin nr 1 contains: first bin of spectrum
    //                  the bin nr nbin+1 contains: overflow




    // NOTE: TRICK   we check if the matrix has less than 32000 couts in every cell.
    // If yes, we can save this matrix in 16 bit formats
    // If NO, we save it in 32 bit format, and we signalise this by negative value
    // in the  tab[0]

    bool flag_must_be_32_bit = false;
    for ( int y = 1 ; y <= y_spectrum_length && !flag_must_be_32_bit ; y ++ ) // see note above
    {
        for ( int x = 1 ; x <= x_spectrum_length ; x ++ ) // see note above
        {
            if ( ptr_root_spectrum->GetBinContent ( x, y ) > 32000 )
            {
                flag_must_be_32_bit = true;
                //cout << "Must be saving 32 bit spectrum " << name_s << endl;
                break;
            }
        }
    }


    // the system which I am using now is
    // 0. double word - left ede of the first bin X
    // 1. double word - waga X
    // 2. double word - bins X

    // 3. double word - left ede of the first bin Y
    // 4. double word - waga Y
    // 5. double word - bins Y

    double tab[6] ;


    //ptr_root_spectrum)->GetXaxis();

    //------- X
    tab[0] = ptr_root_spectrum->GetNbinsX() ;
    tab[1] = ptr_root_spectrum->GetXaxis()->GetBinLowEdge ( 1 );
    tab[2] = ptr_root_spectrum->GetXaxis()->GetBinUpEdge ( ( int ) tab[0] + 1 ); // here was a bug, because we had to skip the underflow bin !!! (+1)


    //     cout << "  NBins X = "  << tab[0]
    //          << "  BinLowEdge[1] =" << tab[1]
    //          << "  BinUpEdge[" << tab[0+1] << "] =  " << tab[2] << endl;

    // ------ Y
    tab[3] = ptr_root_spectrum->GetNbinsY() ;
    tab[4] = ptr_root_spectrum->GetYaxis()->GetBinLowEdge ( 1 );
    tab[5] = ptr_root_spectrum->GetYaxis()->GetBinUpEdge ( ( int ) tab[3] + 1 ); // here was a bug, because we had to skip the underflow bin !!! (+1)


    //      cout << "  NBins Y = "  << tab[3]
    //          << "  BinLowEdge[1] =" << tab[4]
    //          << "  BinUpEdge[" << tab[3]+1 << "] =  " << tab[5] << endl;

    tab[0] *= ( flag_must_be_32_bit? -1 : 1 ); // moment of puttng a negative sign (TRICK above)

    //    cout << "This matrix has xbins = "
    //          << ptr_root_spectrum->GetNbinsX()
    //         << " ybins = "
    //          << ptr_root_spectrum->GetNbinsY() << endl;


    plik.write ( ( char* ) tab, sizeof ( tab ) );


    // warning: in ROOT the bin nr 0 contains: underflow
    //                  the bin nr 1 contains: first bin of spectrum
    //                  the bin nr nbin+1 contains: overflow

    if ( flag_must_be_32_bit == false ) // +++++++++++++++++++++++++++++++
    {
        using slowo = int16_t;        // short int
        vector<slowo> linijka(x_spectrum_length);

        for ( int y = 1 ; y <= y_spectrum_length ; y ++ ) // see note above
        {
            for ( int x = 1 ; x <= x_spectrum_length ; x ++ ) // see note above
            {
                linijka[x - 1] = ( slowo )  ptr_root_spectrum->GetBinContent ( x, y );
            }
            plik.write ( ( char* ) linijka.data(), linijka.size() * sizeof(slowo)  );
        }
    }
    else    // so flag_must_be_32_bit !!!   //++++++++++++++++++++++++++
    {
        using slowo = int32_t;
        vector<slowo> linijka(x_spectrum_length);
        for ( int y = 1 ; y <= y_spectrum_length ; y ++ ) // see note above
        {
            for ( int x = 1 ; x <= x_spectrum_length ; x ++ ) // see note above
            {
                linijka[x - 1] = ( slowo )  ptr_root_spectrum->GetBinContent ( x, y );
            }
            plik.write ( ( char* ) linijka.data(), linijka.size() * sizeof(slowo)  );
        }
    }
    plik.close();
    //   cout << "Succes " << endl;
}
//*****************************************************************************
void spectrum_2D::int_incrementer_X ( int * candidate )
{

    //cout << "F. : incrementer_X" << endl;
    x_int_incrementers_vector.push_back ( candidate );

}

//*****************************************************************************
void spectrum_2D::double_incrementer_X ( double * candidate )
{

    //cout << "F. : incrementer_X" << endl;
    x_double_incrementers_vector.push_back ( candidate );
}
//**************************************************************************
void spectrum_2D::int_incrementer_Y ( int * candidate )
{
    //cout << "F. : incrementer_Y" << endl;
    y_int_incrementers_vector.push_back ( candidate );
}
//**************************************************************************
void spectrum_2D::double_incrementer_Y ( double * candidate )
{

    //cout << "F. : incrementer_Y" << endl;
    y_double_incrementers_vector.push_back ( candidate );

}
//***********************************************************************
void spectrum_2D::increment_yourself()
{
    //cout << "Function incerment yourself, (how many incrementers="
    //     << how_many_incrementers << endl ;


    if ( can_we_increment() == false )
        return ;

    // look at the right integer data

    // all Y from intgeger list -------------
    for ( unsigned int y = 0 ; y < y_int_incrementers_vector.size()  ;  y++ )
    {

        // x integer list first ----------
        for ( unsigned int x = 0 ; x < x_int_incrementers_vector.size() ; x++ )
        {
            //cout << "Checking the candidate nr " << i << endl ;

            // test if this not the same data
            if ( x_int_incrementers_vector[x] == y_int_incrementers_vector[y] )
                continue ;


            int x_value = * ( x_int_incrementers_vector[x] );
            int y_value = * ( y_int_incrementers_vector[y] );
            //cout << "it has a value  " << x_value << endl ;

            //can be nagative !!!
            {
                //cout << "just before incrementation " << value << endl ;
                ptr_root_spectrum->Fill ( x_value, y_value );
                statistics();

                // cout << "                       YES, incremented " << value << endl ;
            }

        }

        // x double list now ----------
        for ( unsigned int x = 0 ; x < x_double_incrementers_vector.size() ; x++ )
        {
            //cout << "Checking the candidate nr " << i << endl ;

            // test if this not the same data
            //  if(x_incrementers_list[x] == y_incrementers_list[y] )continue ;


            double x_value = * ( x_double_incrementers_vector[x] );
            int y_value = * ( y_int_incrementers_vector[y] );
            //cout << "it has a value  " << x_value << endl ;

            //can be nagative !!!
            {
                //cout << "just before incrementation " << value << endl ;
                ptr_root_spectrum->Fill ( x_value, y_value );
                statistics();

                // cout << "                       YES, incremented " << value << endl ;
            }

        }



    }

    // now the same for Y double ###############################
    for ( unsigned int y = 0 ; y < y_double_incrementers_vector.size(); y++ )
    {

        // x integer list first ----------
        for ( unsigned int x = 0 ; x < x_int_incrementers_vector.size() ; x++ )
        {
            //cout << "Checking the candidate nr " << i << endl ;

            // test if this not the same data
            //if(x_int_incrementers_list[x] == y_double_incrementers_list[y] )continue ;


            int x_value = * ( x_int_incrementers_vector[x] );
            double y_value = * ( y_double_incrementers_vector[y] );
            //cout << "it has a value  " << x_value << endl ;

            //can be nagative !!!
            {
                //cout << "just before incrementation " << value << endl ;
                ptr_root_spectrum->Fill ( x_value, y_value );
                statistics();

                // cout << "                       YES, incremented " << value << endl ;
            }

        }

        // x double list now ----------
        for ( unsigned int x = 0 ; x < x_double_incrementers_vector.size() ; x++ )
        {
            //cout << "Checking the candidate nr " << i << endl ;

            // test if this not the same data
            //  if(x_incrementers_list[x] == y_incrementers_list[y] )continue ;


            double x_value = * ( x_double_incrementers_vector[x] );
            double y_value = * ( y_double_incrementers_vector[y] );
            //cout << "it has a value  " << x_value << endl ;

            //can be nagative !!!
            {
                //cout << "just before incrementation " << value << endl ;
                ptr_root_spectrum->Fill ( x_value, y_value );
                statistics();

                // cout << "                       YES, incremented " << value << endl ;
            }

        }



    }


}
//*****************************************************************************
void spectrum_1D::manually_increment ( int x_value, int y_value )
{
    cout << "just before MATIX incrementation " << x_value << endl ;
    ptr_root_spectrum->Fill ( x_value, y_value );
    statistics();

    cout << "            YES, incremented x " << x_value << " y " << y_value << endl ;

}
//****************************************************************************
/** this function is meant only for scalers "spectra". They are scrolled like
pen writing on the paper band */
void spectrum_1D::scroll_left_by_n_bins ( int bins_to_scroll )
{

    //   cout << "Scrolling spectrum " << give_name() << endl;

    for ( int i = 0 ; i < spectrum_length ; i ++ )
    {

        if ( i < ( spectrum_length - bins_to_scroll ) )
        {

            //    below Stat_t is a root typedef (meaning: int ?)
            Stat_t ttt = ptr_root_spectrum->GetBinContent ( i + bins_to_scroll )  ;

            ptr_root_spectrum->SetBinContent ( i,    // channel nr
                                               ttt );

            //          ptr_root_spectrum->Fill(i, value);
            //      ptr_root_spectrum->SetBinContent(
            //          i,  // channel nr
            //          ptr_root_spectrum->GetBinContent(i+bins_to_scroll)  // value
            //          );
        }
        else
        {
            // put zero
            ptr_root_spectrum->SetBinContent ( i, 0 ) ;
            ptr_root_spectrum->Fill ( i, 0 );
        }

    } // end of for

}
//*****************************************************************************
/** for continuation option */
void spectrum_1D::read_from_disk()
{

    // this is 1D spectrum, so we save it in ASCII
    string name_s = string ( "spectra/" ) + ptr_root_spectrum->GetName();
    name_s += ".spc";

    //   cout << "spectrum_1D::read_from_disk()    Restoring 1D spectrum " << name_s << endl ;
    ifstream plik ( name_s.c_str(), ios::binary );

    if ( !plik )
    {
        cout << "Can't open file " << name_s << " to restore spectrum"
             << "\nmost probably spectrum was not stored on the disk before"
             << endl ;
        save_to_disk() ;
        return;
    }

    // we get value from the bin, not from "channel" so we do not
    // care about range of channels, for ex. -200 +200.

    // warning: in ROOT the bin nr 0 contains: underflow
    //                  the bin nr 1 contains: first bin of spectrum
    //                  the bin nr nbin+1 contains: overflow


    // the system which I am using now is
    // 1. double word - left ede of the first bin
    // 2. double word - waga

    //  double left_edge = ptr_root_spectrum->GetBinLowEdge(1);
    //  double waga = ptr_root_spectrum->GetBinLowEdge(2) -
    //                    ptr_root_spectrum->GetBinLowEdge(1);;
    //
    //
    //  plik.read((char*)&left_edge, sizeof( left_edge));
    //  plik.read((char*)&waga, sizeof( waga));


    double bins, left_edge, right_edge ;

    plik.read ( ( char* ) &bins, sizeof ( bins ) );
    plik.read ( ( char* ) &left_edge, sizeof ( left_edge ) );
    plik.read ( ( char* ) &right_edge, sizeof ( right_edge ) );

    double waga = ( right_edge - left_edge ) / bins ;

#ifdef NIGDY

    bool flag_test = false ;
    string nnn = ptr_root_spectrum->GetName();
    if ( nnn == "test_frame_signal_raw" )
    {
        flag_test = true;
        cout << "\nread bins = " << bins
             << ", left_edge = " << left_edge
             << ", right_edge = " << right_edge << endl;
    }
#endif // NIGDY


    int value ;
    for ( int i = 0 /*1*/ ; i < ptr_root_spectrum->GetNbinsX() && plik  ; i ++ ) // see warning above
    {
        double chan = ( i * waga ) + left_edge + ( waga / 2.0 ); // - 1;  // (without -1 the spectrum was shifted +1 channel during reading

        plik.read ( ( char* ) &value, sizeof ( value ) );
        if ( plik.eof() )
            break ;
        ptr_root_spectrum->Fill ( chan, value );

#ifdef NIGDY

        if ( flag_test && i < 20 )
        {
            cout << "read value = " << value
                 << ", is put to the channel  = " << chan  << endl;
        }
#endif // NIGDY

        //cout << "Chan " << chan << " value " << value << endl;
    }
    plik.close();



}
//***********************************************************************
//*****************************************************************************
/** for continuation option */
void spectrum_2D::read_from_disk()
{

    // this is 1D spectrum, so we save it in ASCII
    string name_s = string ( "spectra/" ) + ptr_root_spectrum->GetName();
    name_s += ".mat";

    //     cout << "Restoring 2D spectrum " << name_s << endl ;
    ifstream plik ( name_s.c_str(), ios::binary );

    if ( !plik )
    {
        cout << "Can't open file " << name_s << " to restore spectrum"
             << "\nmost probably spectrum was not stored on the disk before"
             << endl ;
        save_to_disk();
        return;
    }


    // we get value from the bin, not from "channel" so we do not
    // care about range of channels, for ex. -200 +200.

    // warning: in ROOT the bin nr 0 contains: underflow
    //                  the bin nr 1 contains: first bin of spectrum
    //                  the bin nr nbin+1 contains: overflow


    // the system which I am using now is
    // 0. double word - bins X
    // 1. double word - left edge of the first bin X
    // 2. double word - right edge of the first bin X

    // 3. double word - bins Y
    // 4. double word - left edge of the first bin Y
    // 5. double word - right edge of the first bin y

    double def_tab[6] ;

    //------- X

    def_tab[0] = ptr_root_spectrum->GetNbinsX() ;
    def_tab[1] = ptr_root_spectrum->GetXaxis()->GetBinLowEdge ( 1 ); // this is the first, real bin, because the bin 0 is reserved for underflow

    // the last real bin underflow bin + nr of bins
    def_tab[2] = ptr_root_spectrum->GetXaxis()->GetBinUpEdge ( ( int ) ( 1 + def_tab[0] ) );

    // ------ Y
    def_tab[3] = ptr_root_spectrum->GetNbinsY() ;
    def_tab[4] = ptr_root_spectrum->GetYaxis()->GetBinLowEdge ( 1 );
    def_tab[5] = ptr_root_spectrum->GetYaxis()->GetBinUpEdge ( ( int ) ( 1 + def_tab[3] ) );


    //  cout << "This matrix has xbins = "
    //        << ptr_root_spectrum->GetNbinsX()
    //       << " ybins = "
    //        << ptr_root_spectrum->GetNbinsY() << endl;


    double retab[6] ;
    plik.read ( ( char* ) retab, sizeof ( retab ) );

    // NOTE: TRICK If the first word (tab[0] is negative, this means that the coding is 32 bit word(int)
    // otherwise it is standard 16 bit word (short int)
    bool flag_must_be_32_bit = false;
    if ( retab[0] < 0 )
    {
        flag_must_be_32_bit = true;
        retab[0] *= -1;  // we change sign into positive
    }


    for ( int i = 0 ; i < 6 ; i++ )
    {
        if ( def_tab[i] != retab[i] )
        {
            //             cout << " // <different binning - we do not read, matrix should be zero " << endl;
            return ; // <different binning - we do not read, matrix should be zero
        }
    }

    // warning: in ROOT the bin nr 0 contains: underflow
    //                  the bin nr 1 contains: first bin of spectrum
    //                  the bin nr nbin+1 contains: overflow

    //     bool gadaj = false;
    //
    //     if (name_s == "spectra/user_psa_front.mat")
    //     {
    //         gadaj = 1;
    //         cout << name_s << endl;
    //     }

    //typedef short int slowo;
    if ( flag_must_be_32_bit == false )
    {
        typedef short int slowo;
        slowo *linijka = new slowo[ ( int ) retab[0]] ; // bins x
        for ( int y = 1 ; y <= retab[3] ; y ++ ) // bins y
        {
            plik.read ( ( char* ) linijka, sizeof ( slowo ) * ( ( int ) retab[0] ) ) ;
            for ( int x = 1 ; x <= retab[0] ; x ++ ) // see note above
            {
                ptr_root_spectrum->SetBinContent ( x, y, linijka[x - 1] );
            }
        }
        delete [] linijka ;
    }
    else      // flag_must_be_32_bit == false
    {
        typedef int slowo;
        slowo *linijka = new slowo[ ( int ) retab[0]] ; // bins x
        for ( int y = 1 ; y <= retab[3] ; y ++ ) // bins y
        {
            plik.read ( ( char* ) linijka, sizeof ( slowo ) * ( ( int ) retab[0] ) ) ;
            for ( int x = 1 ; x <= retab[0] ; x ++ ) // see note above
            {
                ptr_root_spectrum->SetBinContent ( x, y, linijka[x - 1] );
            }
        }
        delete [] linijka ;

    }
    plik.close();

    //     for (int i = -100 ; i < 100 ; i++)
    //     {
    //         manually_increment (i,-40 );
    //     }
    //             save_to_disk() ;
    //         if (gadaj)
    //  {
    //   exit(999);
    //  }


    //      cout << "Succes " << endl;
}

//***********************************************************************
/** No descriptions */
void spectrum_2D::manually_increment_by ( double x, double y, int value )
{
    // cout << "Empty function spectrum_2D::manually_increment_by" << endl;
    ptr_root_spectrum->Fill ( x, y, value );
}
/** No descriptions */
double spectrum_1D::give_max_chan_paper()
{
    return  max_bin;
}
