#include "Tscintillator.h"

#include "Tfrs.h"
#include "Tcluster.h"
#include "Tnamed_pointer.h"


using namespace std;
#include <cmath>
//***********************************************************************
//  constructor
Tscintillator::Tscintillator(string _name,
                             string name_of_focus_point,
                             bool horizontal,   // horizontal or vertical orientation
                             int TIFJEvent::*left_down_ptr__,
                             int TIFJEvent::*right_up_ptr__,
                             int TIFJEvent::*dt_ptr__)

    : Tfrs_element(_name),
      horizontal_orientation(horizontal) ,
      left_down_ptr(left_down_ptr__),
      right_up_ptr(right_up_ptr__),
      dt_ptr(dt_ptr__)
{


    if(name_of_focus_point == ""
            ||
            name_of_focus_point == "NONE"
            ||
            name_of_focus_point == "none"
      )
    {
        related_focus_point = 0 ;
    }
    else
    {
        related_focus_point = dynamic_cast<Tfocal_plane*>(owner->address_of_device(name_of_focus_point)) ;
        if(!related_focus_point)
        {
            cout << "\nError in program ofr related focus ("
                 << name_of_focus_point
                 << ")point to the  "
                 << name_of_this_element << endl;
            exit(1);
        }
    }


    cal_factor_offset =
        cal_factor_slope =
            cal_factor_quadratic =
                cal_factor_third_degree =
                    cal_factor_4th_degree = 0 ;

    create_my_spectra();


    named_pointer[name_of_this_element + "_energy_when_ok"]
    = Tnamed_pointer(&e, &e_ok, this) ;

    named_pointer[name_of_this_element + (horizontal_orientation ? "_left" : "_down")]
    = Tnamed_pointer(&left_down, 0, this) ;

    named_pointer[name_of_this_element + (horizontal_orientation ? "_right" : "_up")]
    = Tnamed_pointer(&right_up, 0, this) ;

    named_pointer[name_of_this_element +  "_delta_t"]
    = Tnamed_pointer(&dt, 0, this) ;



    named_pointer[name_of_this_element + (horizontal_orientation ? "_x" : "_y")
                  + "_extrapolated_from_focal_plane"]
    = Tnamed_pointer(&x_or_y_extrapolated_here, &x_or_y_extrapolation_ok, this) ;


    named_pointer[name_of_this_element +  "_position_when_ok" ]
    = Tnamed_pointer(&position, &position_ok, this) ;


    // for rates spectra
    named_pointer[name_of_this_element + (horizontal_orientation ? "_left_fired" : "_down_fired")]
    = Tnamed_pointer(&left_down, &left_down_fired, this) ;

    named_pointer[name_of_this_element + (horizontal_orientation ? "_right_fired" : "_up_fired")]
    = Tnamed_pointer(&right_up, &right_up_fired, this) ;


}
//************************************************************************
void Tscintillator::create_my_spectra()
{
    // the spectra have to live all the time, so here we can create
    // them during  the constructor

    string folder = "scintillators/" + name_of_this_element ;

    string name = name_of_this_element + "_energy"  ;
    spec_e = new spectrum_1D(name,
                             name,
                             4096, 0, 4096,
                             folder, "",
                             name_of_this_element + "_energy_when_ok");
    frs_spectra_ptr->push_back(spec_e) ;

    //--------------
    name = name_of_this_element + "_dt"  ;
    spec_dt = new spectrum_1D(name,
                              name,
                              4096, 0, 4096,
                              folder,
                              "GATE for '_position'",
                              name_of_this_element +  "_delta_t");

    frs_spectra_ptr->push_back(spec_dt) ;

    sci_dt_gate_low = 0 ;   // if somebody forgot to create gate, it
    sci_dt_gate_high = 4093 ; // will be set wide open and created on the disk

    //--------------
    if(related_focus_point)   // if not null
    {
        name = name_of_this_element + "_mwPos_vs_dt"  ;
        matr_mw_sci = new spectrum_2D(name,
                                      name,
                                      //4096,  0, 4096,
                                      256,  0, 4090,
                                      256, -100, 100,
                                      // 400, 0, 400,
                                      folder, "",
                                      string("x: ") + name_of_this_element +  "_delta_t"
                                      ", y: " + name_of_this_element + (horizontal_orientation ? "_x" : "_y")
                                      + "_extrapolated_from_focal_plane"    // incrementers dt, x_or_y_extrapolated_here
                                     );
        frs_spectra_ptr->push_back(matr_mw_sci) ;
    }

    //    for(int i = 0 ; i < 4096 ; i++)
    //    {
    //      for(int y = 0 ; y < 4096 ; y++)
    //        (TH2*)(matr_mw_sci->give_root_spectrum_pointer())->Fill(i, y, i);
    //    }

    //--------------
    name = name_of_this_element + "_position"  ;
    spec_position = new spectrum_1D(name,
                                    name,
                                    400, -100, 100,
                                    folder,
                                    "When _dt gate is passed",
                                    name_of_this_element +  "_position_when_ok");
    frs_spectra_ptr->push_back(spec_position) ;



    //--- Nami wants two electrodes separately raw spectra

    name = name_of_this_element +
           (horizontal_orientation ? "_left_raw" :  "_down_raw")  ;

    spec_left_down = new spectrum_1D(name,
                                     name,
                                     4096, 0, 4096,
                                     folder, "",
                                     name_of_this_element + (horizontal_orientation ? "_left_fired" : "_down_fired"));

    frs_spectra_ptr->push_back(spec_left_down) ;


    name = name_of_this_element +
           (horizontal_orientation ? "_right_raw" :  "_up_raw")  ;

    spec_right_up   = new spectrum_1D(name,
                                      name,
                                      4096, 0, 4096,
                                      folder, "",
                                      name_of_this_element + (horizontal_orientation ? "_right_fired" : "_up_fired"));
    frs_spectra_ptr->push_back(spec_right_up) ;


    //--------------
    name = name_of_this_element + "_position_extrapolated"  ;
    spec_position_extr = new spectrum_1D(name,
                                         name,
                                         400, -100, 100,
                                         folder, "",
                                         name_of_this_element + (horizontal_orientation ? "_x" : "_y")
                                         + "_extrapolated_from_focal_plane");
    frs_spectra_ptr->push_back(spec_position_extr) ;

}
//**********************************************************************
void Tscintillator::analyse_current_event()
{
    //cout << "analyse_current_event for " <<  name_of_this_element  << endl ;

    if(name_of_this_element == "sci41HybridHoriz")
    {
        cout << "This is " <<  name_of_this_element << endl;
    }

    // initially
    e_ok = false ;
    left_down_fired = right_up_fired = false ;

    //
    if((left_down = event_unpacked->*left_down_ptr))    // <-- really assignment
    {
        left_down_fired = true ;
    }
    if((right_up = event_unpacked->*right_up_ptr))   // <-- really assignment
    {
        right_up_fired = true;
    }

    spec_left_down -> manually_increment(left_down);
    spec_right_up  -> manually_increment(right_up);


    //  if(name_of_this_element == "sci21LR")
    //  {
    //    cout << "sci21 left = " << left_down
    //    << ", right = " << right_up
    //    << endl;
    //  }


    // geometric average
    e = sqrt((double) left_down  * right_up);

    if(e >= 0 && e < 4096)
    {
        if(e > 0)
        {
            //cout << name_of_this_element << " OK energy " << e << endl;
            e_ok = true;
        }
        else
        {
            //cout << name_of_this_element << " NOT ok energy " << e << endl;
        }
        spec_e ->manually_increment(e);
    }
    else
    {
        //error = 2;
        cout << "error 2, because in SCI" << name_of_this_element << " energy = "
             << e << " is outside range 0-7999 "
             << endl ;

        e = 0;
    }



    // now we can try to make a position analysis

    // PRODUCING MATRIX TO MAKE POSTION CALIBRATION FACTORS

    // this were TIME data comming from the scintilator,
    // Having this we are able to estimate the geometric position
    // of the scintillation inside the cyistal.
    // Unfortunately we can not calibrate
    // it in a classical way. (how to convert this time into milimeters)
    // But there is a trick...


    // In the first phase of the experiment we have also Multiwires
    // working. They can help us to extrapolate the point where
    // ion hit the scintillator. So we have two information comming
    // from different devices. If MW was calibrated - we can use
    // this calibration to make the calibration of the scintillator



    // this is an "official comment":
    //------Relation between the position of MW and  SCI----------

    // this is important in the moment of calibration
    // when the data of mw are present, and we want to see the
    // matrices to see invent the calibration factors



    // at first raw spectrum
    dt = event_unpacked->*dt_ptr ;
    spec_dt->manually_increment(dt);


    // if this scintillator is related to any focusing point which
    // was measured by two neighbouring MWs
    x_or_y_extrapolation_ok = false ;


    if(related_focus_point)    // not null
    {
        //double tmp = 0 ;
        bool flag_do_it = false ;
        // scintilattor can be horizontal (left-right) or vertical (down-up)
        if(horizontal_orientation
                &&
                related_focus_point-> was_x_ok())      // x
        {
            x_or_y_extrapolated_here = related_focus_point->extrapolate_x_to_distance(distance) ;
            x_or_y_extrapolation_ok = true ;
            flag_do_it = true ;
        }

        else if(!horizontal_orientation                    // vertical
                &&
                related_focus_point-> was_y_ok())
        {
            x_or_y_extrapolated_here = related_focus_point->extrapolate_y_to_distance(distance) ;
            x_or_y_extrapolation_ok = true ;
            flag_do_it = true ;
        }


        if(x_or_y_extrapolation_ok)
        {
            spec_position_extr->manually_increment(x_or_y_extrapolated_here);
        }

        //-------------------------------------------
        if(dt > 0.5)
        {

            if(flag_do_it)
            {
                // increment the matrix
                //  cout << "matrix " << name_of_this_element << " will be increm with x="
                //       << static_cast<double>( *ptr_left_right_dt)
                //       << ", y=" << tmp
                //       << endl ;


                //int idt =
                double ddt = (double)dt  ;
                matr_mw_sci->manually_increment(ddt, x_or_y_extrapolated_here);   // extrapolated_position_value

                //       matr_mw_sci->manually_increment(2000, 0 );
                //       matr_mw_sci->manually_increment(1000, 0);
                //       matr_mw_sci->manually_increment(3000, 0 );
                //       matr_mw_sci->manually_increment(2000, 50 );
                //       matr_mw_sci->manually_increment(2000, -50);

            }
        } // if dt
    }

    // POSITION CALCULATION

    // now there is a part assuming that above calibration has already been made
    // and calibration factors are stored in the calibration file.
    // there is no conflict between these two actions. (above: producing matrices
    // to read from them calibration factors
    // and action below: making real calculation of the position on scintilator,



    position = -900 ;
    position_ok = false;


    if(
        (sci_dt_gate_low <= (dt))
        &&
        ((dt) <= sci_dt_gate_high)
    )
    {
        double tmp =  dt + randomizer::randomek() ;

        position =  cal_factor_offset + (cal_factor_slope * tmp)
                    + (cal_factor_quadratic * tmp * tmp)
                    +
                    (cal_factor_third_degree ?      // if third degree was given
                     (cal_factor_third_degree * tmp * tmp * tmp)
                     :
                     0)
                    +
                    (cal_factor_4th_degree ?      // if third degree was given
                     (cal_factor_4th_degree * tmp * tmp * tmp * tmp)
                     :
                     0)
                    ;

        position_ok = true;
        spec_position->manually_increment(position)  ;
    }

    calculations_already_done = true ;
}
//**************************************************************************
void  Tscintillator::make_preloop_action(ifstream & s)  // read the calibration factors, gates
{

    cout << "Reading the calibration for " << name_of_this_element << endl ;

    distance = Tfile_helper::find_in_file(s, name_of_this_element  + "_distance");

    // reading the caligration factors

    cal_factor_offset = Tfile_helper::find_in_file(s, name_of_this_element  + "_offset");
    cal_factor_slope = Tfile_helper::find_in_file(s, name_of_this_element  + "_slope");
    cal_factor_quadratic = Tfile_helper::find_in_file(s, name_of_this_element  + "_quadratic");
    try
    {
        cal_factor_third_degree = Tfile_helper::find_in_file(s,
                                  name_of_this_element  + "_third_degree");
    }
    catch(Tno_keyword_exception mess)
    {
        cal_factor_third_degree = 0 ;
        // stream 's' may be in the error state
        s.clear() ;
    }

    try
    {
        cal_factor_4th_degree = Tfile_helper::find_in_file(s,
                                name_of_this_element  + "_4th_degree");

    }
    catch(Tno_keyword_exception mess)
    {
        cal_factor_4th_degree = 0 ;
        // stream 's' may be in the error state
        s.clear() ;
    }

    //  catch(... )
    //  {
    //    cout << "other exception ? " << endl;
    //    }

    // reading gates
    // but this is in a different file....

    //   cout << " reading the gates for " << name_of_this_element << " dt signals " << endl ;


    //-----------------

    // here we
    string gates_file = "gates/scintillators_gates.txt" ;

    try
    {

        string core_of_the_name = "gates/" + name_of_this_element ;
        string nam_gate = core_of_the_name + "_dt_gate_GATE.gate" ;

        ifstream plik_gates(nam_gate.c_str());
        if(! plik_gates)
        {
            string mess = "I can't open  file: " + nam_gate  ;
            // throw mess ;
            cout << mess << "\nSo I assume that the gate is wide open !!!!!!!!!!! \a\a\a\n" << endl;
            ofstream pliknew(nam_gate.c_str());
            pliknew <<  sci_dt_gate_low << "   "
                    << sci_dt_gate_high
                    << "\t\t // left and right value for the gate" << endl;

        }
        else
        {


            plik_gates >> zjedz >> sci_dt_gate_low     // lower limit
                       >> zjedz >> sci_dt_gate_high ;   // upper limit

            if(! plik_gates)
            {
                string mess = "Error while reading 2 numbers from the gate file: " + nam_gate  ;
                throw mess ;
            }
            plik_gates.close() ;
        }


    }
    catch(string  sss)
    {

        cerr << "Error while reading file with gates \""
             << gates_file
             << "\" :\n"
             << sss << endl  ;

        exit(-1) ; // is it healthy ?
    }



}
//***********************************************************************
/** No descriptions */
double Tscintillator::give_e()
{
    return e_ok ? e : 0 ;
}
