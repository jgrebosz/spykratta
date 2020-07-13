#include "Tbonn_magnet.h"
#include "Tnamed_pointer.h"


//***********************************************************************
//  constructor
Tbonn_magnet::Tbonn_magnet(string _name,
                           int TIFJEvent::*magnet_up_ptr, int TIFJEvent::*magnet_dn_ptr)
    : Tfrs_element(_name),
      magnet_up(magnet_up_ptr),
      magnet_dn(magnet_dn_ptr)

{
    create_my_spectra();

    named_pointer[name_of_this_element + "_up_raw"] =
        Tnamed_pointer(&polarity_up, 0, this) ;

    named_pointer[name_of_this_element + "_down_raw"] =
        Tnamed_pointer(&polarity_dn, 0, this) ;

    named_pointer[name_of_this_element + "_FLAG_polarity_up_is_in_default_gate"] =
        Tnamed_pointer(&flag_polarity_up_is_in_default_gate, 0, this) ;

    named_pointer[name_of_this_element + "_FLAG_polarity_dn_is_in_default_gate"] =
        Tnamed_pointer(&flag_polarity_dn_is_in_default_gate, 0, this) ;

}
//************************************************************************
void Tbonn_magnet::create_my_spectra()
{
    //the spectra have to live all the time, so here we can create
    // them during  the constructor

    string folder = ""  ;
    //-----------
    string name = name_of_this_element + "_polarity_up"  ;
    spec_magnet_up  = new spectrum_1D(name,
                                      name,
                                      4096, 0, 4096,
                                      folder,
                                      "GATE marks polarity 100% UP",
                                      name_of_this_element + "_polarity_up");
    frs_spectra_ptr->push_back(spec_magnet_up) ;

    //-----------
    name = name_of_this_element + "_polarity_down"  ;
    spec_magnet_dn  = new spectrum_1D(name,
                                      name,
                                      4096, 0, 4096,
                                      folder,
                                      "GATE marks polarity 100% DOWN",
                                      name_of_this_element + "_down_raw");
    frs_spectra_ptr->push_back(spec_magnet_dn) ;

    // default values
    gate_on_up_polarity[0] =  550 ;    // lower limit
    gate_on_up_polarity[1] = 2000 ;  // upper limit

    gate_on_dn_polarity[0] =  550 ;    // lower limit
    gate_on_dn_polarity[1] = 2000 ;  // upper limit
}
//**********************************************************************
void Tbonn_magnet::analyse_current_event()
{
    // cout << "analyse_current_event()  for "
    //         <<  name_of_this_element
    //          << endl ;


    polarity_up = event_unpacked->*magnet_up ;
    polarity_dn = event_unpacked->*magnet_dn ;

    spec_magnet_up ->manually_increment(polarity_up);
    spec_magnet_dn ->manually_increment(polarity_dn);


    // here should a code which test the polarisation
    //   because some other objects (conditional spectra can ask for this information)
    //

    if(polarity_up >= gate_on_up_polarity[0]      // lower limit
            &&
            polarity_up <= gate_on_up_polarity[1])    // upper limit
    {
        flag_polarity_up_is_in_default_gate = true ;
    }
    else
        flag_polarity_up_is_in_default_gate = false;

    // Down
    if(polarity_dn >= gate_on_dn_polarity[0]      // lower limit
            &&
            polarity_dn <= gate_on_dn_polarity[1])    // upper limit
    {
        flag_polarity_dn_is_in_default_gate = true ;
    }
    else
        flag_polarity_dn_is_in_default_gate = false;


    calculations_already_done = true ;

}
//**************************************************************************
void  Tbonn_magnet::make_preloop_action(ifstream & s)  // read the calibration factors, gates
{

    // cout << "Reading the calibration for " << name_of_this_element << endl ;



    // but this is in a different file....

//     cout << " reading the gates for " << name_of_this_element << " polarisation " << endl ;


    //-----------------

    // the new style, gates are created by my viewer, or manually
    // so that in one file there is only one gate, for this particullar object

    string core_of_the_name = "gates/" + name_of_this_element ;


    string nam_gate = core_of_the_name + "_polarity_up_gate_GATE.gate" ;
    ifstream plik_gatesX(nam_gate.c_str());

    if(! plik_gatesX)
    {

        //    string mess = "I can't open  file: " + nam_gate
        //          + "\nThis gate you can produce interactively using a 'cracow' viewer program" ;
        //    cout
        //        << mess
        //        << "\nI create a gate and assmue that the gate is wide open !!!!!!!!!!!!!! \a\a\a\n"
        //        << endl ;

        ofstream pliknew(nam_gate.c_str());
        pliknew   << gate_on_up_polarity[0] << "   "    // lower limit
                  << gate_on_up_polarity[1] << endl ;
    }
    else
    {
        double tmp = 0 ;
        plik_gatesX >> zjedz >> tmp;
        gate_on_up_polarity[0]  = static_cast<int>(tmp);     // lower limit

        plik_gatesX >> zjedz >> tmp;
        gate_on_up_polarity[1]  = static_cast<int>(tmp);     // upper limit

        //   plik_gatesX >> zjedz >> gate_on_polarity[0]     // lower limit
        //                 >> zjedz >> gate_on_polarity[1] ;   // upper limit

        if(! plik_gatesX)
        {
            string mess = "Error while reading 2 numbers from the gate file: " + nam_gate  ;
            cout << name_of_this_element << ", during reading the parameters:  "
                 << mess << endl;
            exit(-1) ;
        }
        plik_gatesX.close() ;
    }
    // DOWN-------------------------------


    nam_gate = core_of_the_name + "_polarity_down_gate_GATE.gate" ;
    plik_gatesX.open(nam_gate.c_str());

    if(! plik_gatesX)
    {

        //    string mess = "I can't open  file: " + nam_gate
        //          + "\nThis gate you can produce interactively using a 'cracow' viewer program" ;
        //    cout
        //        << mess
        //        << "\nI create a gate and assmue that the gate is wide open !!!!!!!!!!!!!! \a\a\a\n"
        //        << endl ;

        ofstream pliknew(nam_gate.c_str());
        pliknew   << gate_on_dn_polarity[0] << "   "    // lower limit
                  << gate_on_dn_polarity[1] << endl ;
    }
    else
    {
        double tmp = 0 ;
        plik_gatesX >> zjedz >> tmp;
        gate_on_dn_polarity[0]  = static_cast<int>(tmp);     // lower limit

        plik_gatesX >> zjedz >> tmp;
        gate_on_dn_polarity[1]  = static_cast<int>(tmp);     // upper limit

        //   plik_gatesX >> zjedz >> gate_on_polarity[0]     // lower limit
        //                 >> zjedz >> gate_on_polarity[1] ;   // upper limit

        if(! plik_gatesX)
        {
            string mess = "Error while reading 2 numbers from the gate file: " + nam_gate  ;
            cout << name_of_this_element << ", during reading the parameters:  "
                 << mess << endl;
            exit(-1) ;
        }
        plik_gatesX.close() ;
    }


}
//***************************************************************
