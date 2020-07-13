//////////////////////////////////////////////////////////////////
// Jurek Grebosz, 14 June 2002
//
// spectra hierarchy
/////////////////////////////////////////////////////////////////

#ifndef SPECTRUM_H
#define SPECTRUM_H

#include "experiment_def.h"
//#include "TH1.h"

#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
using namespace std ;
#include <iostream>
#include <fstream>
#include <cmath>
////////////////////////////////////////////////////////////////////
// Descriptor stores on the disk the list of all currently collected spectra
// This information isused by the cracow
//////////////////////////////////////////////////////////////////
class Cspectra_descriptor
{
    string name ;
    string notice_txt;
public:

    Cspectra_descriptor(const string fname = "spectra/descriptions.txt")
    {
        name = fname ;
        //
        ofstream plik(name.c_str());     // deleting the previous contents
        plik << "version_with_incrementers\n";  // to distinguish from the old style
        plik.close();
    }
    //--------------------------- for 1D spectra
#define WHERE   cout << __FILE__<< ", line " << __LINE__ << endl;

    void add_entry(const string spec_name, double bin_x, double first_x, double last_x,
                   const string notice_arg, const string list_of_incrementers)
    {
//  cout << "linia " << __LINE__ << endl;;
        notice_txt = notice_arg;
        if(list_of_incrementers == "no incrementers known"
                || list_of_incrementers.empty())
        {
            cout << "Widmo " << spec_name << " nie ma opisu inkrementorow"
                 << endl;
            cout << spec_name << endl;
        }
        ofstream plik(name.c_str(), ios::app);

        plik << spec_name << "\n" << bin_x
             << "\t" << first_x << "\t" << last_x
             << "\n{\n" << notice_arg << "\n}"
             << "\n{\n"
             << list_of_incrementers
             << "\n}" << endl;
        plik.close();
    }
    //---------------------------  for 2D spectra
    void add_entry(const string spec_name,
                   int bin_x, double first_x, double last_x,
                   int bin_y, double first_y, double last_y, const string notice,
                   const string list_of_incrementers)
    {
        if(list_of_incrementers == "2D incrementers"
                || list_of_incrementers.empty() )
        {
            cout << "Widmo " << spec_name << " nie ma opisu inkrementorow"
                 << endl;
            cout << spec_name << endl;
        }

        ofstream plik(name.c_str(), ios::app);
        plik << spec_name << "\n"
             << bin_x << "\t" << first_x << "\t" << last_x
             << "\n"
             << bin_y << "\t" << first_y << "\t" << last_y ;

        plik << "\n{\n" << notice << "\n}"
             << "\n{\n" << list_of_incrementers << "\n}"
             << endl;

        plik.close();
    }
    void add_notice(const string txt)
    {
        notice_txt += txt ;
    }

};
//***************************************

// including my version of Cern Root histograms - without using Root library
#include "Thistograms_jurek.h"


//////////////////////////////////////////////////////////////////
class spectrum_abstr
{

protected:
    TH1 * ptr_root_spectrum;
    // char spectrum_name[100] ;
    int statistics_of_increments ;

    bool * ptr_condition_flag_is_true ;

    static  Cspectra_descriptor *spectra_descriptor;

public:

    spectrum_abstr()
    {
        ptr_condition_flag_is_true = 0 ;

        if(spectra_descriptor == 0)
            spectra_descriptor = new Cspectra_descriptor;
    }
    //------------------------

    virtual void int_incrementer_X(int * candidate) = 0 ;
    virtual void int_incrementer_Y(int * candidate) = 0 ;

    virtual void double_incrementer_X(double * candidate) = 0 ;
    virtual void double_incrementer_Y(double * candidate) = 0 ;

    void set_adres_of_condition_flag(bool * adres)
    {
        ptr_condition_flag_is_true = adres;
    }


    bool can_we_increment()
    {
        if(ptr_condition_flag_is_true == 0)
        {
            //cout << " There was 0 in spectrum condiotion flag" << endl ;
            return true;  // there was no condition
        }
        else
        {
            //cout << " There was reall adres in spectrum condiotion flag (and *adres is " <<*(ptr_condition_flag_is_true)  << endl ;
            return * (ptr_condition_flag_is_true) ;   //there was, and it is true
        }
    }


    virtual void reset_incrementers() = 0 ;

    virtual void increment_yourself()
    {}   // = 0;
    TH1* give_root_spectrum_pointer()
    {
        return  ptr_root_spectrum;
    }

    //------------------------------
    const string give_name()
    {
        return ptr_root_spectrum->GetName();
    }


    void statistics(int nr = 1)
    {
        statistics_of_increments += nr ;
    }
    virtual int give_statistic()
    {
        return statistics_of_increments;
    }

    virtual ~spectrum_abstr()
    {
        delete spectra_descriptor ;
        spectra_descriptor = 0 ; // null
    } ;

    /** No descriptions */
    void add_notice(const string txt);    // why it is here  ?

    virtual void save_to_disk() = 0 ;
    virtual void manually_increment(int value) = 0 ;
    virtual void manually_increment(double value) = 0 ;
    void zeroing()
    {
        ptr_root_spectrum->Reset();
    }
};

///////////////////////////////////////////////////////////////////////////////
class spectrum_1D : public spectrum_abstr
{

protected:
    //  int * *int_incrementers_list ; // pointer to table of pointers
    vector<int*> int_incrementers_vector ;

    // int how_many_int_incrementers;

    //double * *double_incrementers_list ; // pointer to table of pointers
    //int how_many_double_incrementers;
    vector<double*> double_incrementers_vector ;

    int spectrum_length ; // so many "bins"
    double min_bin ;
    double max_bin ;

public:

    //-------------------------------------------
    spectrum_1D(const string name_root, int h_one, double h_two, double h_three,
                const string folder = "",
                const string note = "",
                const string list_incrementers = "no incrementers known");

    spectrum_1D() ;  // default for contitional spectra

    // D'TOR-------------------------------------
    ~spectrum_1D();

    //-------------------------------------------
    void increment_yourself();

    //------------------------------------------
    void int_incrementer_X(int * candidate);
    void int_incrementer_Y(int * candidate);

    void double_incrementer_X(double * candidate);
    void double_incrementer_Y(double * candidate);


    void reset_incrementers()
    {
        int_incrementers_vector.resize(0) ;
        // same for doubles
        double_incrementers_vector.resize(0) ;
    }
    //------------------------------------------
    void save_to_disk() ;

    //    void manually_increment(int value)  ;
    //    void manually_increment(double value)  ;
    //----------------------
    void manually_increment(int value)
    {
        //              cout << "just before incrementation " << ptr_root_spectrum->GetName()
        //                    << " with " << value << endl ;
        ptr_root_spectrum->Fill(value);     //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        //statistics();
    }
    void manually_increment(long long value)
    {
        //              cout << "just before incrementation " << ptr_root_spectrum->GetName()
        //                    << " with " << value << endl ;
        ptr_root_spectrum->Fill(value);     //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        //statistics();
    }
    void manually_increment(long value)
    {
        //              cout << "just before incrementation " << ptr_root_spectrum->GetName()
        //                    << " with " << value << endl ;
        ptr_root_spectrum->Fill(value);     //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        //statistics();
    }
    // -------------------------------------------
    void manually_increment(double value)
    {
        // cout << "just before incrementation " << value << endl ;
        ptr_root_spectrum->Fill(value);      //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        //statistics();
    }

    void zeroing()
    {
        // cout << "just before zeroing " << value << endl ;
        ptr_root_spectrum->Reset();
    }

    void manually_increment(int, int)  ;
    void manually_inc_by(long int chan, int value);    // increment channel nr chan, by value
    /** this function is meant only for scalers "spectra". They are scrolled like
    pen writing on the paper band */
    void scroll_left_by_n_bins(int bins);
    /** No descriptions */
    void create(const string name_root, int nr_of_bins,
                double first_channel,
                double last_channel, const string folder, const string  note,
                const string list_incrementers);
    /** for continuation option */
    void read_from_disk();
    /** No descriptions */
    double give_max_chan_paper();


};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class spectrum_2D : public spectrum_abstr
{

protected:

    vector<int*> x_int_incrementers_vector ;
    vector<double*> x_double_incrementers_vector ;


    int x_spectrum_length ; // so many "bins"
    double x_min_bin ;

    vector<int*> y_int_incrementers_vector ;
    vector<double*> y_double_incrementers_vector ;

    int y_spectrum_length ; // so many "bins"
    double y_min_bin ;

public:

    //-------------------------------------------
    spectrum_2D(const string  name_root,
                int nr_of_bins_x, double first_channel_x, double last_channel_x,
                int nr_of_bins_y, double first_channel_y, double last_channel_y,
                const string  folder = "", const string  note = "",
                const string list_incrementers = "2D incrementers");

    // D'TOR-------------------------------------
    ~spectrum_2D();


    //-------------------------------------------
    void increment_yourself();

    //------------------------------------------
    void int_incrementer_X(int * candidate);
    void int_incrementer_Y(int * candidate);

    void double_incrementer_X(double * candidate);
    void double_incrementer_Y(double * candidate);

    void reset_incrementers()
    {
        x_int_incrementers_vector.resize(0) ;
        x_double_incrementers_vector.resize(0) ;

        y_int_incrementers_vector.resize(0) ;
        y_double_incrementers_vector.resize(0) ;
    }
    //------------------------------------------
    void save_to_disk() ;

    virtual void manually_increment(double value)
    {
        manually_increment((int) value) ;
    }

    //------------------------------------------
    void manually_increment(int /*value*/)
    {
        cout << "Fuction: manually_increment for matrix must have 2 arguments...  press any key" << endl ;
        int liczba ;
        cin >> liczba ;
    }
    //------------------------------------------
    void manually_increment(int x_value, int y_value)
    {
        // cout << "spectrum_2D:: manually_increment" << endl;
        ptr_root_spectrum->Fill(x_value, y_value);      //@@@@@@@@@@@@@@@@@@
        //statistics();
    }
    //------------------------------------------
    void manually_increment(long x_value, long y_value)
    {
        // cout << "spectrum_2D:: manually_increment" << endl;
        ptr_root_spectrum->Fill(x_value, y_value);      //@@@@@@@@@@@@@@@@@@
        //statistics();
    }
    //------------------------------------------
    void manually_increment(long long x_value, long long y_value)
    {
        // cout << "spectrum_2D:: manually_increment" << endl;
        ptr_root_spectrum->Fill(x_value, y_value);      //@@@@@@@@@@@@@@@@@@
        //statistics();
    }

    void manually_increment(double x_value, double y_value)
    {
        ptr_root_spectrum->Fill(x_value, y_value);    // @@@@@@@@@@@@@@@@@@@
        //statistics();
    }

    void read_from_disk();
    /** No descriptions */
    void manually_increment_by(double x, double y, int value);
};
///////////////////////////////////////////////////////////////////////////////
#endif // SPECTRUM_H

