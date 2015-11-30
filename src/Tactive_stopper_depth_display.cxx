#include "Tactive_stopper_depth_display.h"


#ifdef MUN_ACTIVE_STOPPER_PRESENT

#include "Tnamed_pointer.h"
#include <sstream>

//***********************************************************************
//  constructor
#include "Tnamed_pointer.h"
#include <sstream>

#include "Tactive_stopper_munich.h"   // for implant/decay trigger
//***********************************************************************
//  constructor

Tactive_stopper_depth_display::Tactive_stopper_depth_display(string _name,
        int (TIFJEvent::*first_ptr)[how_many_767_channels],
        int (TIFJEvent::*second_ptr)[how_many_775_channels])
    :
    Tfrs_element(_name),
    signal_767ptr(first_ptr),
    signal_775ptr(second_ptr)
{
    //   cout << "For "
    //   << name_of_this_element
    //   << " flag_we_want_calibrate_signals is "
    //   << flag_we_want_calibrate_signals << endl;

    create_my_spectra();

    //    for(int i = 0 ; i < 32 ; i++)
    //      {
    //        named_pointer[name_of_this_element + "_first"]
    //          = Tnamed_pointer(&first_signal_data, 0, this) ;
    //      }
}

//************************************************************
void Tactive_stopper_depth_display::create_my_spectra()
{
    //the spectra have to live all the time, so here we can create
    // them during  the constructor
    string folder = "munich/" + name_of_this_element ;
    string name = name_of_this_element + "_depth_of_implantation";
    //-------------- for testing,  spectra of all ADC and TDCchannels
    spec_depth_implantation = new spectrum_1D(name, name,
            23, 0, 23,
            folder, "", "many different incrementers");
    frs_spectra_ptr->push_back(spec_depth_implantation) ;

    // similar for decay
    name = name_of_this_element + "_depth_of_decay";
    //-------------- for testing,  spectra of all ADC and TDCchannels
    spec_depth_decay = new spectrum_1D(name, name,
                                       23, 0, 23,
                                       folder, "", "many different incrementers");
    frs_spectra_ptr->push_back(spec_depth_decay) ;

}
//*************************************************************
void Tactive_stopper_depth_display::analyse_current_event()
{
    // cout << "analyse_current_event()  for "
    //         <<  name_of_this_element
    //          << endl ;
    for(uint i = 0 ; i < sizeof(please_increment_channel) / sizeof(bool) ; i++)
    {
        please_increment_channel[i] = false;
    }

    // at first 767 signals
    int ch = -999;
    for(int i = 0 ; i < how_many_767_channels ; i++)
    {
        signal_767data[i] = (event_unpacked->*signal_767ptr)[i];
        if(!signal_767data[i]) continue;
        ch = -999;
        // here we can redistribute the data to the spectrum

        int kanal = i; //signal_767data[i];

        // note:  stopper A is in the block 775
        if(kanal >= 0 && kanal <= 31)   // stopper B data
        {
            ch = 11;
        }
        else if(kanal >= 32 && kanal <= 63)  // stopper C data
        {
            ch = 12;
        }
        else if(kanal >= 119 && kanal <= 125)  // F1
        {
            ch = 8;
        }
        else if(kanal >= 112 && kanal <= 118)  // F0
        {
            ch = 9;
        }
        else if(kanal >= 80 && kanal <= 86)  // R0
        {
            ch = 13;
        }
        else if(kanal >= 87 && kanal <= 93)  // R0
        {
            ch = 14;
        }
        else  // some absorbers
        {
            switch(kanal)
            {
            case 103:
                ch = 0;
                break;
            case 102:
                ch = 1;
                break;
            case 101:
                ch = 2;
                break;
            case 100:
                ch = 3;
                break;
            case 99:
                ch = 4;
                break;
            case 98:
                ch = 5;
                break;
            case 97:
                ch = 6;
                break;
            case 96:
                ch = 7;
                break;


                // other rear absorbers
            case 64:
                ch = 15;
                break;
            case 65:
                ch = 16;
                break;
            case 66:
                ch = 17;
                break;
            case 67:
                ch = 18;
                break;
            case 68:
                ch = 19;
                break;
            case 69:
                ch = 20;
                break;
            case 70:
                ch = 21;
                break;
            case 71:
                ch = 22;
                break;

            default:
                ch = -999;
                break;
            } // end switch
        }

        if(ch >= 0) please_increment_channel[ch] = true;
    } // end of for 776

    //===========================
    // Stopper A has its data in the 775block
    //===========================
    for(int i = 0 ; i < how_many_775_channels ; i++)
    {
        signal_775data[i] = (event_unpacked->*signal_775ptr)[i];
        if(!signal_775data[i]) continue;
        ch = -999;
        // here we can redistribute the data to the spectrum

        int kanal = i; //signal_775data[i];

        // note:  stopper A is in the block 775
        if(kanal >= 0 && kanal <= 31)   // stopper B data
        {
            please_increment_channel[10] = true;
        }
    } // end for 775

    //=========================================
    // now we can increment the "fan-panorama" spectrum

    for(int i = 0 ; i < (int)(sizeof(please_increment_channel) / sizeof(bool)) ; i++)
    {
        if(!please_increment_channel[i]) continue;

        switch(active_stopper_namespace::active_stopper_event_type)
        {
        case active_stopper_namespace::decay_event :
            spec_depth_decay->manually_increment(i);
            break;
        case active_stopper_namespace::implantation_event :
            spec_depth_implantation->manually_increment(i);
            break;

        case active_stopper_namespace::undefined_event:
            break;
        } // switch
    } // for


    calculations_already_done = true ;
}
//*************************************************************
void Tactive_stopper_depth_display::make_preloop_action(ifstream &)
{}
//***************************************************************
#endif // #ifdef MUN_ACTIVE_STOPPER_PRESENT

