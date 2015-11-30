/***************************************************************************
                          Tuser_spectrum.cpp  -  description
                             -------------------
    begin                : Wed Sep 3 2003
    copyright            : (C) 2003 by dr Jerzy Grebosz IFJ Krakow
    email                :
***************************************************************************/

#include "Tuser_spectrum.h"
#include "Tuser_condition.h"

#include "spectrum.h"
#include "Tnamed_pointer.h"


#include "Tself_gate_ger_descr.h"
#include "Tself_gate_hec_descr.h"
#include "Tself_gate_mib_descr.h"
#include "Tself_gate_ger_addback_descr.h"
#include "Tself_gate_agata_psa_descr.h"

#include "Tincrementer_donnor.h"
#include "paths.h"

//user_spectrum_descripion text_desc ;
//*************************************************************************
Tuser_spectrum::Tuser_spectrum()
{
    cond_ptr = 0 ;

}
//*************************************************************************
Tuser_spectrum::~Tuser_spectrum()
{
    delete spec_ptr;
}
//*************************************************************************
/** Read the definition prepared in the text file (for example by cracow) */
void Tuser_spectrum::read_in_parameters(string s)
{


    desc.set_name(s);
    desc.read_from(string("./definitions_user_spectra/") + s + ".user_definition") ;

    // here we may fill the array of incrementers looking on the map
    set_incrementers();

}

/********************************************************************/
void Tuser_spectrum::create_the_spectrum()
{

    string folder = "user/"  ;
    string sname = desc.get_name() ;

    if(desc.give_dimmension() == user_spectrum_description::spec_1D)
    {
        spec_ptr = new spectrum_1D(
            sname,
            sname,
            desc.give_bins_x(), desc.give_beg_x(), desc.give_end_x(),
            folder, "",
            "To check incrementers - go to the User Defined Spectra manager");
    }
    else
    {
        spec_ptr = new spectrum_2D(sname,
                                   sname,
                                   desc.give_bins_x(), desc.give_beg_x(), desc.give_end_x(),
                                   desc.give_bins_y(), desc.give_beg_y(), desc.give_end_y(),
                                   folder, "",
                                   "To check incrementers - go to the User Defined Spectra manager");
    }
    // owner->remember_user_spectrum(spec_ptr) ;
    // NO, the funcion which calls this funciton will take care
    // of registering it for saving, updating
    // it is so, because now I still do not know if it should
    // be registered in one, or in two vectors

}
//***********************************************************
user_spectrum_description  Tuser_spectrum::give_description()
{
    return desc ;
}
//***********************************************************
string Tuser_spectrum::give_name()
{
    return desc.get_name() ;
}
//***********************************************************
/** converting the string names into addresses of data */
void Tuser_spectrum::set_incrementers()
{

    x_incr_addresses.clear();
    y_incr_addresses.clear();
    clear_selfgates_map();

    // X ---------------

    // vector<string> & x_incr = desc.give_x_incr();
    vector< pair<string, string> > & x_incr = desc.give_x_incr();
    for(unsigned i = 0 ; i < x_incr.size() ; i++)
    {
        //cout << x_incr[i] << endl;
        // find this string in the table
        Tmap_of_named_pointers::iterator pos = named_pointer.find(x_incr[i].first);

        //#ifdef NIGDY
        if(pos == named_pointer.end())     // if not found
        {
            // look between deprecated names
            pos = perhaps_deprecated_name_of_incrementer(named_pointer, x_incr[i].first);
        }
        //#endif

        if(pos != named_pointer.end())
        {
            // Key found
            //   cout << "Found string " << (pos->first) << endl;
            // cout << pos->first << " odpowiada_adres "
            // << ((int) &(pos->second)) << endl;


            // try to find the self gate, if necessery

            x_incr_addresses.push_back(Tincr_entry(pos->second,       // Tnamed pointer
                                                   address_of_self_gate(x_incr[i].second))) ;           //  pointer to the descriptor

            // Tnamed_pointer ttt = pos->second ;
            // cout << "Vek ma dlugosc " << ttt.vek.size() ;
            // cout << "Oto adresy " << endl;
            // for(unsigned k = 0 ; k < ttt.vek.size() ; k++)
            // {
            //   cout << "k= " << k << ") " << ttt.vek[k].v_double_ptr
            //    << endl ;
            // }

        }
        else
        {

            // Key NOT found
            cout << "\nERROR: In the definition of user spectrum: "
                 << give_name() << " there is a name of the incrementer (variable):\n"
                 << x_incr[i].first
                 << "\nwhich is not known to the spy.\n"
                 "Please go to the cracow viewer now, open the definition of this spectrum\n"
                 "and choose one of the available incrementers"
                 << endl;

            exit(1);
        }
    }


    // Y --------------
    if(desc.give_dimmension() == user_spectrum_description::spec_2D)
    {
        // Y
        vector< pair<string, string> > & y_incr = desc.give_y_incr();
        for(unsigned i = 0 ; i < y_incr.size() ; i++)
        {
            //cout << y_incr[i] << endl;
            // find this string in the table
            Tmap_of_named_pointers::iterator pos = named_pointer.find(y_incr[i].first);

            if(pos == named_pointer.end())     // if not found
            {
                // look between deprecated names
                pos = perhaps_deprecated_name_of_incrementer(named_pointer, y_incr[i].first);
            }


            if(pos != named_pointer.end())
            {
                // Key found
                // cout << "Found string " << (pos->first) << endl;
                // cout << pos->first << " odpowiada_adres "
                // << ((int) &(pos->second)) << endl;


                y_incr_addresses.push_back(Tincr_entry(pos->second,       // Tnamed pointer
                                                       address_of_self_gate(y_incr[i].second))) ;           //  pointer to the descriptor

            }
            else
            {
                // Key NOT found
                cout << "\nERROR: In the definition of user spectrum " << give_name()
                     << " there is a name of the incrementer (variable):\n"
                     << y_incr[i].first
                     << "\nwhich is not known to the spy.\n"
                     "Please go to the cracow viewer now, open the definition of this spectrum\n"
                     "and choose one of the available incrementers"
                     << endl;

                exit(1);
            }
        }

    }

}
/**********************************************************************/
void Tuser_spectrum::make_incrementations()
{

    //  cout << "Spectrum " << desc.get_name() << "  checking" << endl;


    //  if(cond_result_ptr)
    //  {
    //    cout << "Condition was assigned" << endl;
    //    if(*cond_result_ptr == true)  cout << "Condition was TRUE " << endl;
    //    else cout << "Condition was FALSE " << endl;
    //   }

    if(
        (cond_result_ptr && (*cond_result_ptr == false)) // condition for this spectrum is false
        ||

        !desc.spectrum_enabled())   // or matrix is disabled
    {
        return ;
    }


    // if no condition == condition always true


    if(desc.give_dimmension() == user_spectrum_description::spec_1D)
    {
        make_incr_of_1D_spectrum();
    }
    else          // 2D spectra =============================
    {
        make_incr_of_2D_spectrum();
    }
}
//*******************************************************************
void Tuser_spectrum::save_to_disk()
{
    spec_ptr->save_to_disk();
}
//*******************************************************************
void Tuser_spectrum::remember_address_of_condition(Tuser_condition *t)
{
    cond_ptr = t ;
    if(cond_ptr)
    {
        cond_result_ptr = &cond_ptr->result ;
    }
    else
    {
        cond_result_ptr = 0 ;
    }
}
//******************************************************************
/** spectrum is 1D, so only X will be incremeted */
void Tuser_spectrum::make_incr_of_1D_spectrum()
{

//      cout
//        << "make_incr_of_1D_spectrum() loop on x_incr_addresses.size() = "
//        << x_incr_addresses.size() << endl;

    for(unsigned i = 0 ; i < x_incr_addresses.size() ; i++)
    {

        // in case of single entry we can say at once if the signal is not valid
        // (in case of multi entry we have to check all of entries


        if(x_incr_addresses[i].nam_ptr.flag_multi_entry == false)
        {

            if(x_incr_addresses[i].nam_ptr.data_usable != 0
                    &&
                    *(x_incr_addresses[i].nam_ptr.data_usable) == false)
            {
                continue ; // this is blocked (because for example not fired)
            }

            // here we may check the self_gate of this incrmenter

            if(x_incr_addresses[i].self_gate_descr_ptr)  // if selfgate was set
            {
                if(x_incr_addresses[i].self_gate_descr_ptr->not_for_1Dspectrum())
                {
                    cout << "NOTE: The self gate called "
                         //<< x_incr_addresses[i].self_gate_descr_ptr->name
                         << "  has settings option 'time difference between 2 gammas' as  ON "
                         "which is  nonsense in case of 1D spectrum: " << give_name()
                         << endl;
                }
                if(!x_incr_addresses[i].nam_ptr.ptr_detector->
                        check_selfgate(x_incr_addresses[i].self_gate_descr_ptr))
                {
                    continue ; // selfgate was false ;
                }
            }


//                int mmm = x_incr_addresses[i].nam_ptr.what_type ;

//               cout << "== Incremet USER spectrum " << this->give_name()  <<endl;

            switch(x_incr_addresses[i].nam_ptr.what_type)
            {
            case Tnamed_pointer::is_double:
                spec_ptr->manually_increment(*(x_incr_addresses[i].nam_ptr.double_ptr));
//                 cout << "DDDDD incremented with double value " << (*(x_incr_addresses[i].nam_ptr.double_ptr)) << endl;
                break;

            case   Tnamed_pointer::is_int:
                //cout << "IIIII  Incremet USER with int value " << *(x_incr_addresses[i].nam_ptr.int_ptr) << endl ;
                spec_ptr->manually_increment(*(x_incr_addresses[i].nam_ptr.int_ptr));
                break;

            case   Tnamed_pointer::is_bool:
                spec_ptr->manually_increment(*(x_incr_addresses[i].nam_ptr.bool_ptr));
                break;


            } // switch

        } //
        else    // this is multi
        {
            //cout << "so many multi " <<  x_incr_addresses[i].nam_ptr.vek.size() << endl;

            for(unsigned m = 0 ; m < x_incr_addresses[i].nam_ptr.vek.size() ; m++)
            {
                if(x_incr_addresses[i].nam_ptr.vek[m].v_data_usable != 0    // when defined
                        &&
                        *(x_incr_addresses[i].nam_ptr.vek[m].v_data_usable) == false)       // and  not valid
                {
                    continue ;
                }  // these were the condition to no


                // here we may check the self_gate of this incrmenter ----------

                if(x_incr_addresses[i].self_gate_descr_ptr)  // if selfgate was set
                {
                    if(!x_incr_addresses[i].nam_ptr.vek[m].v_ptr_detector->
                            check_selfgate(x_incr_addresses[i].self_gate_descr_ptr))
                    {
                        continue ; // selfgate was false ;
                    }
                }

                // -------- lets increment

                double value = 0 ;
                switch(x_incr_addresses[i].nam_ptr.vek[m].v_what_type)
                {
                case    Tnamed_pointer::is_double:
                    value = (*(x_incr_addresses[i].nam_ptr.vek[m].v_double_ptr));
                    break;
                case    Tnamed_pointer::is_int:
                    value = (*(x_incr_addresses[i].nam_ptr.vek[m].v_int_ptr));
                    break;
                case    Tnamed_pointer::is_bool:
                    value = (*(x_incr_addresses[i].nam_ptr.vek[m].v_bool_ptr));
                    break;
                }
                //            cout << "USER multi incr with " << value
                //              << "taken from address "
                //              << (x_incr_addresses[i].nam_ptr.vek[m].v_double_ptr)
                //              << endl;
                spec_ptr->manually_increment(value);

            }
            // ????? break;
        }
    } // for incrementers

}
//*******************************************************************
/** Has X and Y incrementers */
void Tuser_spectrum::make_incr_of_2D_spectrum()
{

    for(unsigned x = 0 ; x < x_incr_addresses.size() ; x++)
    {

        double x_value = 0 ;

        if(x_incr_addresses[x].nam_ptr.flag_multi_entry == false)
        {

            if(x_incr_addresses[x].nam_ptr.data_usable != 0
                    &&
                    *(x_incr_addresses[x].nam_ptr.data_usable) == false)
            {
                continue ; // this is blocked (because for example not fired)
            }


            //-----------------------------------
            // here we may check the self_gate of this incrmenter

            if(x_incr_addresses[x].self_gate_descr_ptr)  // if selfgate was set
            {
                x_incr_addresses[x].self_gate_descr_ptr-> this_is_x_incrementer();
                if(!x_incr_addresses[x].nam_ptr.ptr_detector->
                        check_selfgate(x_incr_addresses[x].self_gate_descr_ptr))
                {
                    continue ; // selfgate was false ;
                }
            }
            //------------------------------


            //    int mmm = x_incr_addresses[x].nam_ptr.what_type ;
            //cout << "Incremet USER with what value ? " <<endl;
            switch(x_incr_addresses[x].nam_ptr.what_type)
            {
            case Tnamed_pointer::is_double:
                x_value = *(x_incr_addresses[x].nam_ptr.double_ptr);
                break;

            case Tnamed_pointer::is_int:
                //cout << "Incremet USER with value "
                //   << *(x_incr_addresses[x].nam_ptr.int_ptr) << endl ;
                x_value = *(x_incr_addresses[x].nam_ptr.int_ptr);
                break;

            case Tnamed_pointer::is_bool:
                x_value = *(x_incr_addresses[x].nam_ptr.bool_ptr);
                break;

            } // switch

            // second argument is the addres of the detecors whic delvered the data
            find_y_to_increment_with(x_value, (x_incr_addresses[x].nam_ptr.ptr_detector));
        } //
        else    // this is multi
        {
            //cout << "so many multi " <<  x_incr_addresses[x].nam_ptr.vek.size() << endl;
            for(unsigned m = 0 ; m < x_incr_addresses[x].nam_ptr.vek.size() ; m++)
            {
                if(*(x_incr_addresses[x].nam_ptr.vek[m].v_data_usable) == false       // when not valid
                        && x_incr_addresses[x].nam_ptr.vek[m].v_data_usable != 0)   // and defined
                {
                    continue ;
                }  // these were the condition to no


                // here we may check the self_gate of this incrmenter ----------

                if(x_incr_addresses[x].self_gate_descr_ptr)  // if selfgate was set
                {
                    x_incr_addresses[x].self_gate_descr_ptr-> this_is_x_incrementer();
                    if(!x_incr_addresses[x].nam_ptr.vek[m].v_ptr_detector->
                            check_selfgate(x_incr_addresses[x].self_gate_descr_ptr))
                    {
                        continue ; // selfgate was false ;
                    }
                }

                // -------- lets increment



                switch(x_incr_addresses[x].nam_ptr.vek[m].v_what_type)
                {
                case    Tnamed_pointer::is_double:
                    x_value = (*(x_incr_addresses[x].nam_ptr.vek[m].v_double_ptr));
                    break;

                case    Tnamed_pointer::is_int:
                    x_value = (*(x_incr_addresses[x].nam_ptr.vek[m].v_int_ptr));
                    break;
                case    Tnamed_pointer::is_bool:
                    x_value = (*(x_incr_addresses[x].nam_ptr.vek[m].v_bool_ptr));
                    break;
                }
                //            cout << "USER multi incr with " << value
                //              << "taken from address "
                //              << (x_incr_addresses[x].nam_ptr.vek[m].v_double_ptr)
                //              << endl;

                // second argument is the addres of the detecors whic delvered the data
                find_y_to_increment_with(x_value, (x_incr_addresses[x].nam_ptr.vek[m].v_ptr_detector));

            } // end for
        } // else multi
    } // end for increm


}
//******************************************************************

/** No descriptions */
void Tuser_spectrum::find_y_to_increment_with(double x_value, void *det_address)
{
    double y_value = 0 ;

    for(unsigned i = 0 ; i < y_incr_addresses.size() ; i++)
    {
        // in case of single entry we can say at once if the signal is not valid
        // (in case of multi entry we have to check all of entries

        if(y_incr_addresses[i].nam_ptr.flag_multi_entry == false)
        {
            if(y_incr_addresses[i].nam_ptr.data_usable != 0
                    &&
                    *(y_incr_addresses[i].nam_ptr.data_usable) == false)
            {
                continue ; // this is blocked (because for example not fired)
            }

            //-----------------------------------
            // here we may check the self_gate of this incrmenter

            if(y_incr_addresses[i].self_gate_descr_ptr)  // if selfgate was set
            {
                y_incr_addresses[i].self_gate_descr_ptr->this_is_y_incrementer();
                if(!y_incr_addresses[i].nam_ptr.ptr_detector->
                        check_selfgate(y_incr_addresses[i].self_gate_descr_ptr))
                {
                    continue ; // selfgate was false ;
                }
            }
            //------------------------------




            //    int mmm = y_incr_addresses[i].nam_ptr.what_type ;
            //cout << "Incremet USER with what value ? " <<endl;
            switch(y_incr_addresses[i].nam_ptr.what_type)
            {
            case Tnamed_pointer::is_double:
                y_value = *(y_incr_addresses[i].nam_ptr.double_ptr);
                break;

            case   Tnamed_pointer::is_int:
                //cout << "Incremet USER with value " << *(y_incr_addresses[i].nam_ptr.int_ptr) << endl ;
                y_value = *(y_incr_addresses[i].nam_ptr.int_ptr);
                break;

            case   Tnamed_pointer::is_bool:
                y_value = *(y_incr_addresses[i].nam_ptr.bool_ptr);
                break;

            } // switch

            // ========= now we can try to increment ============
            switch(desc.give_policy())
            {
            case 0:  // always

//             flag_talking_histograms = true;
//                 cout << "User spectrum will increment at : "
//                 << x_value << ", " << y_value << endl;

                ((spectrum_2D*)(spec_ptr))->manually_increment(x_value, y_value);
//             flag_talking_histograms = false;
                break;

            case 1:  // only when different
                if(det_address != (y_incr_addresses[i].nam_ptr.ptr_detector))
                {
                    ((spectrum_2D*)(spec_ptr))->manually_increment(x_value, y_value);
                }
                break;

            case 2:  // only when the same
                if(det_address == (y_incr_addresses[i].nam_ptr.ptr_detector))
                {
                    ((spectrum_2D*)(spec_ptr))->manually_increment(x_value, y_value);
                }
                break;
            } // switch
        } //
        else    // this is multi
        {
            //cout << "so many multi " <<  y_incr_addresses[i].nam_ptr.vek.size() << endl;

            for(unsigned m = 0 ; m < y_incr_addresses[i].nam_ptr.vek.size() ; m++)
            {
                if(*(y_incr_addresses[i].nam_ptr.vek[m].v_data_usable) == false       // when not valid
                        && y_incr_addresses[i].nam_ptr.vek[m].v_data_usable != 0)   // and defined
                {
                    continue ;
                }  // these were the condition to no


                // here we may check the self_gate of this incrmenter ----------

                if(y_incr_addresses[i].self_gate_descr_ptr)  // if selfgate was set
                {
                    y_incr_addresses[i].self_gate_descr_ptr->this_is_y_incrementer();
                    if(!y_incr_addresses[i].nam_ptr.vek[m].v_ptr_detector->
                            check_selfgate(y_incr_addresses[i].self_gate_descr_ptr))
                    {
                        continue ; // selfgate was false ;
                    }
                }

                // -------- lets increment

                switch(y_incr_addresses[i].nam_ptr.vek[m].v_what_type)
                {
                case    Tnamed_pointer::is_double:
                    y_value = (*(y_incr_addresses[i].nam_ptr.vek[m].v_double_ptr));
                    break;
                case    Tnamed_pointer::is_int:
                    y_value = (*(y_incr_addresses[i].nam_ptr.vek[m].v_int_ptr));
                    break;
                case    Tnamed_pointer::is_bool:
                    y_value = (*(y_incr_addresses[i].nam_ptr.vek[m].v_bool_ptr));
                    break;
                }
                //            cout << "USER multi incr with " << value
                //              << "taken from address "
                //              << (y_incr_addresses[i].nam_ptr.vek[m].v_double_ptr)
                //              << endl;



                //     // below we incremt only when these two addresses are different
                //     void *adresik =y_incr_addresses[i].nam_ptr.vek[m].v_ptr_detector ;
                //
                //        if(x_value == y_value)
                //        {
                //            cout << "uwaga !, x_value="
                //            << x_value
                //            << ", y_value="<< y_value
                //            << ",   x_adr = "
                //            << address
                //            << ", y_adr = "
                //            << adresik << endl;
                //
                //        }


                // ========= now we can try to increment ============
                switch(desc.give_policy())
                {
                case 0:  // always
                    ((spectrum_2D*)(spec_ptr))->manually_increment(x_value, y_value);
                    break;

                case 1:  // only when different
                    if(det_address !=  y_incr_addresses[i].nam_ptr.vek[m].v_ptr_detector)
                    {
                        ((spectrum_2D*)(spec_ptr))->manually_increment(x_value, y_value);
                    }
                    break;

                case 2:  // only when the same
                    if(det_address ==  y_incr_addresses[i].nam_ptr.vek[m].v_ptr_detector)
                    {
                        ((spectrum_2D*)(spec_ptr))->manually_increment(x_value, y_value);
                    }
                    break;
                } // switch


            } // for

        } // else multi

    } // for incrementers

}
//*********************************************************************
/** This function finds in the special vector the selfgate description
If this description is on in the vector, reads it from the disk, stores
in the vector and gives the address */
Tself_gate_abstract_descr*  Tuser_spectrum::address_of_self_gate(string sg_name)
{
    // finding in the map such a selfgate
    map<string, Tself_gate_abstract_descr* >::iterator pos =
        map_of_selfgates.find(sg_name);

    if(pos != map_of_selfgates.end())
    {
        return pos->second ;
    }

    // Recognize the extension as some known types
    // read the proper description

    string lowercase_name = sg_name;
    for(unsigned i = 0 ; i < lowercase_name.size() ; i++)
    {
        lowercase_name[i] = tolower(lowercase_name[i]);
    }
    if(lowercase_name == "no_self_gate") return nullptr;


    Tself_gate_abstract_descr* d  = nullptr;
    try {
        d = Tself_gate_abstract_descr::create_descr_for_sg_file(sg_name);
    }
    catch(runtime_error e)
    {
        cerr << "ERROR: During creation of user defined spectrum called "
             << desc.get_name() << "\n   "  << e.what()      << endl;
        throw;
    }
    map_of_selfgates[sg_name] = d;
    return d ;
}
/***************************************************************************/
void Tuser_spectrum::clear_selfgates_map()
{

    map<string, Tself_gate_abstract_descr* >::iterator it =
        map_of_selfgates.begin();

    for(; it != map_of_selfgates.end() ; it++)
    {
        delete it->second ;
    }
    map_of_selfgates.clear() ;
}
//*******************************************************************
/** in case if the name of the incrementer was not found, perhaps this name is
on the list of the deprecated pointers, obsolate, deprecated */

Tmap_of_named_pointers::iterator
Tuser_spectrum::perhaps_deprecated_name_of_incrementer(Tmap_of_named_pointers & np, string orgstr)
{
    //  cout << "Looking between the deprecated names... the name "
    //   << orgstr << endl;
    // modifying the string
    vector<string> stary;
    vector<string> nowy;

    stary.push_back("succesfull") ;
    nowy.push_back("successful") ;

    stary.push_back("cluster_crys") ;
    nowy.push_back("cluster_") ;

    stary.push_back("time_LR_diff_cal") ;
    nowy.push_back("time_LR_cal") ;

    stary.push_back("time_diff_calibrated") ;
    nowy.push_back("time_cal") ;

    stary.push_back("stopper_implantation_") ;
    nowy.push_back("stopper_AllModules__implantation_") ;

    stary.push_back("stopper2_implantation_") ;
    nowy.push_back("stopper2_AllModules__implantation_") ;

    stary.push_back("addback") ;
    nowy.push_back("multi_addback");

    stary.push_back("music");  // to allow spectra working with old music (musicBig) and new, munich - just (music)
    nowy.push_back("musicBig") ;

    stary.push_back("musicBig") ;
    nowy.push_back("music");

    stary.push_back("Adback") ;
    nowy.push_back("Addback");

    for(unsigned int i = 0 ; i < stary.size() ; i++)
    {
        string str = orgstr;
        string::size_type pos  = str.find(stary[i]);
        //cout << "Checking " << stary[i] << endl;
        if(pos != string::npos)
        {
            //        cout << "Found!" << endl;
            // if old was found - replace it
            str.replace(pos, stary[i].size(), nowy[i]);
            //       cout << "After replace str = >"<< str << "<" << endl;

            return   np.find(str); // and return the iterator to the new name
        }
        // if the old string not found, keep on searching

    }
    // not found at all - return the iterator to nowhere
    return   np.find(orgstr);


}
