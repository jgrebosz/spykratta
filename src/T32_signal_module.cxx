#include "T32_signal_module.h"


#ifdef NIGDY  // IT IS TEMPLATE, SO ALL FUNCTIONS ARE IN THE HEADER FILE

#include "Tnamed_pointer.h"
#include <sstream>


//***********************************************************************
//  constructor

template <calkow nr_of_channels>
T32_signal_module<nr_of_channels>::T32_signal_module(string _name,
        int (TIFJEvent::*first_ptr)[nr_of_channels],
        bool flag_produce_fan_spectrum_arg, // generally we do not want. Let the user make them as user defined
        bool flag_produce_signal_spectra_arg // generally we do not want. Let the user make them as user defined
                                                    )
    : Tfrs_element(_name),  signal_ptr(first_ptr),
      flag_produce_signal_spectra(flag_produce_signal_spectra_arg),
      flag_produce_fan_spectrum(flag_produce_fan_spectrum_arg)
{
    create_my_spectra();

    //    for(int i = 0 ; i < 32 ; i++)
    //      {
    //        named_pointer[name_of_this_element + "_first"]
    //          = Tnamed_pointer(&first_signal_data, 0, this) ;
    //      }

}

//************************************************************
template <calkow nr_of_channels>
void T32_signal_module<nr_of_channels>::create_my_spectra()
{
    //the spectra have to live all the time, so here we can create
    // them during  the constructor
    string folder = "munich/" + name_of_this_element ;
    //-------------- for testing,  spectra of all ADC and TDCchannels
    for(int i = 0 ; i < nr_of_channels ; i++)
    {
        // incrementers
        ostringstream plik ;
        plik << name_of_this_element
             << "["
             << (i < 10 ? "0" : "")
             << i << "]" ;

        string name = plik.str();
        named_pointer[name] = Tnamed_pointer(&signal_data[i], 0, this) ;

        // spectra
        if(flag_produce_signal_spectra)
        {
            spec_chan[i] = new spectrum_1D(name, name,
                                           4095, 1, 4096,
                                           folder, "", noraw);
            frs_spectra_ptr->push_back(spec_chan[i]) ;

        }
    }
    // fan spectrum
    if(flag_produce_fan_spectrum)
    {
        spec_fan = new spectrum_1D(
            name_of_this_element + "_fan",
            name_of_this_element + "_fan",
            nr_of_channels, 0, nr_of_channels,
            folder, "", noraw);
        frs_spectra_ptr->push_back(spec_fan) ;
    }
}
//*************************************************************

template <calkow nr_of_channels>
void T32_signal_module<nr_of_channels>::analyse_current_event()
{


    cout << "analyse_current_event()  for "
         <<  name_of_this_element
         << endl ;

    for(int i = 0 ; i < nr_of_channels ; i++)
    {
        signal_data[i] = (event_unpacked->*signal_ptr)[i];
        if(flag_produce_signal_spectra)
        {
            spec_chan[i]->manually_increment(signal_data[i]);

        }

        if(flag_produce_fan_spectrum && signal_data[i])
        {
            spec_fan->manually_increment(i);
        }
    }



    calculations_already_done = true ;

}
#endif // NIGDY


#ifdef NIGDY
//**************************************************************
// read the calibration factors, gates
template <calkow nr_of_channels>
void  T32_signal_module<nr_of_channels>::make_preloop_action(ifstream & /*s*/)
{}
#endif // NIGDY
//***************************************************************
