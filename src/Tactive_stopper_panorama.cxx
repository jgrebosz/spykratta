
#include "Tfrs.h"
#include "Tactive_stopper_panorama.h"

#ifdef MUN_ACTIVE_STOPPER_PRESENT



#include <Tpositron_absorber.h>
#include <Tactive_stopper_munich.h>
#include <Tpositron_absorber.h>

#include "Tactive_stopper_one_stripe.h" // just to have the acces to type of

//***********************************************************************************************
Tactive_stopper_panorama::Tactive_stopper_panorama(string name,
        string front_absorber_name,
        vector<string> stopper_names,
        string rear_absorber_name)
    : Tfrs_element(name), Tincrementer_donnor()
{
    front_absorber = dynamic_cast<Tpositron_absorber *>(owner->address_of_device(front_absorber_name)) ;
    for(uint i = 0 ; i < stopper_names.size(); i++)
    {
        stopper.push_back(
            dynamic_cast<Tactive_stopper_munich *>(owner->address_of_device(stopper_names[i]))
        ) ;
    }
    rear_absorber = dynamic_cast<Tpositron_absorber *>(owner->address_of_device(rear_absorber_name)) ;

    create_my_spectra();




    named_pointer[name_of_this_element + "__sum_energy_of_front_absorbers_plus_stoppers"] =
        Tnamed_pointer(&sum_energy_of_front_absorbers_plus_stoppers, 0, this) ;

    named_pointer[name_of_this_element + "__sum_energy_of_rear_absorbers_plus_stoppers"] =
        Tnamed_pointer(&sum_energy_of_rear_absorbers_plus_stoppers, 0, this) ;


    named_pointer[name_of_this_element + "__multiplicity_of_front_absorbers_plus_stoppers"] =
        Tnamed_pointer(&multiplicity_of_front_absorbers_plus_stoppers, 0, this) ;

    named_pointer[name_of_this_element + "__multiplicity_of_rear_absorbers_plus_stoppers"] =
        Tnamed_pointer(&multiplicity_of_rear_absorbers_plus_stoppers, 0, this) ;

    named_pointer[name_of_this_element + "__multiplicity_of_all_absorbers_plus_stoppers"] =
        Tnamed_pointer(&multiplicity_of_all_absorbers_plus_stoppers, 0, this) ;

}
//*************************************************************************************************
void Tactive_stopper_panorama::create_my_spectra()
{
    string folder("stopper/");


    string name =  name_of_this_element + "_x_trajectories_implantation"  ;
    spec_x_panorama_impl = new spectrum_2D(name,
                                           name,
                                           23, -10, 3 + 10,
                                           60, 0, 60,
                                           folder, "", "Many incrementers used to make this matrix, (so do not set polygon gates here)"
                                           //                                string("as X:") + name_of_this_element +"_y_when_ok" +
                                           //                                ", as Y: " + name_of_this_element+"_y_when_ok");
                                          );
    frs_spectra_ptr->push_back(spec_x_panorama_impl) ;


    name =  name_of_this_element + "_x_trajectories_decay"  ;
    spec_x_panorama_decay = new spectrum_2D(name,
                                            name,
                                            23, -10, 3 + 10,
                                            60, 0, 60,
                                            folder, "", "Many incrementers used to make this matrix, (so do not set polygon gates here)"
                                            //                                string("as X:") + name_of_this_element +"_y_when_ok" +
                                            //                                ", as Y: " + name_of_this_element+"_y_when_ok");
                                           );
    frs_spectra_ptr->push_back(spec_x_panorama_decay) ;

    //========== Y ===============
    name =  name_of_this_element + "_y_trajectories_implantation"  ;
    spec_y_panorama_impl = new spectrum_2D(name,
                                           name,
                                           23, -10, 3 + 10,
                                           40, 0, 40,
                                           folder, "", "Many incrementers used to make this matrix, (so do not set polygon gates here)"
                                           //                                string("as X:") + name_of_this_element +"_y_when_ok" +
                                           //                                ", as Y: " + name_of_this_element+"_y_when_ok");
                                          );
    frs_spectra_ptr->push_back(spec_y_panorama_impl) ;


    name =  name_of_this_element + "_y_trajectories_decay"  ;
    spec_y_panorama_decay = new spectrum_2D(name,
                                            name,
                                            23, -10, 3 + 10,
                                            40, 0, 40,
                                            folder, "", "Many incrementers used to make this matrix, (so do not set polygon gates here)"
                                            //                                string("as X:") + name_of_this_element +"_y_when_ok" +
                                            //                                ", as Y: " + name_of_this_element+"_y_when_ok");
                                           );
    frs_spectra_ptr->push_back(spec_y_panorama_decay) ;

}
//*************************************************************************************************
void Tactive_stopper_panorama::analyse_current_event()
{
    //   cout << "Tactive_stopper_panorama::analyse_current_event()" << endl;

    sum_energy_of_front_absorbers_plus_stoppers = 0;
    sum_energy_of_rear_absorbers_plus_stoppers = 0 ;
    multiplicity_of_front_absorbers_plus_stoppers = 0;
    multiplicity_of_rear_absorbers_plus_stoppers = 0;
    multiplicity_of_all_absorbers_plus_stoppers = 0;



    switch(active_stopper_namespace::active_stopper_event_type)
    {
    case active_stopper_namespace::implantation_event:
    {
        // loop over all plates of the front absorber ==================================
        double which = -0.5;
        for(uint i = 0 ; i < POSITRON_ABSORBER_NR ; i++)
        {
            bool is_valid = false;
            double position = front_absorber->give_x_pos_plate(i, &is_valid);

            if(is_valid)
            {

                if(position < 0 || position > 100)
                {
                    cout << "Tactive_stopper_panorama::analyse_current_event() ---> Be careful, the absorber positon is "
                         << position << endl;
                }
                // for(int m = 0 ; m < pixel_of_absorber; m++)
                spec_x_panorama_impl->manually_increment(which - i, position);

                // Y positon has no strips, just when it fired - it fried
                for(int m = 0 ; m < 40; m++)  // trick to fill all bins in this column
                    spec_y_panorama_impl->manually_increment(which - i, (double) m);

            }
        }

        // now the stopper part ================================================
        for(uint i = 0 ; i < stopper.size() ; i++)
        {
            bool is_valid = false;
            double position =  stopper[i]->give_universal_x_position_implantation(&is_valid);
            if(is_valid)
            {
                //           cout << "Receviced univerasl x positon" << position << endl;
                spec_x_panorama_impl->manually_increment(i, position);
            }

            position =  stopper[i]->give_universal_y_position_implantation(&is_valid);
            if(is_valid)
            {
                // cout << "Receviced univeasl Y positon" << position << endl;
                spec_x_panorama_impl->manually_increment(i, position);
            }
        }

        // now the rear absorbers ===============================================
        for(uint i = 0 ; i < POSITRON_ABSORBER_NR ; i++)
        {
            bool is_valid = false;
            double position = rear_absorber->give_x_pos_plate(i, &is_valid);
            if(is_valid)
            {
                //           for(int m = 0 ; m < pixel_of_absorber; m++)
                spec_x_panorama_impl->manually_increment(i + 3, position);
                // Y positon has no strips, just when it fired - it fried
                for(int m = 0 ; m < 40; m++)  // trick to fill all bins in this column
                    spec_x_panorama_impl->manually_increment(i + 3, m);
            }
        } // end for
    }
    break;

    //----------------------------------------------------------------------------------
    case active_stopper_namespace::decay_event:
        //------------------------------------------------------------------------------------
    {
        // loop over all plates of the front absorber ==================================
        double which = -0.5;
        for(uint i = 0 ; i < POSITRON_ABSORBER_NR ; i++)
        {
            bool is_valid = false;
            double position = front_absorber->give_x_pos_plate(i, &is_valid);

            if(is_valid)
            {

                if(position < 0 || position > 100)
                {
                    cout << "Tactive_stopper_panorama::analyse_current_event() ---> Be careful, the absorber positon is "
                         << position << endl;
                }
                // for(int m = 0 ; m < pixel_of_absorber; m++)
                spec_x_panorama_decay->manually_increment(which - i, position);

                // Y positon has no strips, just when it fired - it fried
                for(int m = 0 ; m < 40; m++)  // trick to fill all bins in this column
                    spec_y_panorama_decay->manually_increment(which - i, (double) m);

                multiplicity_of_front_absorbers_plus_stoppers++;
                multiplicity_of_all_absorbers_plus_stoppers++;

            }
        } // end for

        sum_energy_of_front_absorbers_plus_stoppers = front_absorber->give_decay_sum_energy();
        sum_energy_of_rear_absorbers_plus_stoppers = 0 ;

        // now the stopper part ================================================
        for(uint i = 0 ; i < stopper.size() ; i++)
        {
            bool is_valid = false;
            double position =  stopper[i]->give_universal_x_position_decay(&is_valid);
            if(is_valid)
            {
                //           cout << "Receviced universal x positon" << position << endl;
                spec_x_panorama_decay->manually_increment(i, position);
                sum_energy_of_front_absorbers_plus_stoppers += stopper[i]->give_decay_sum_energy();
                sum_energy_of_rear_absorbers_plus_stoppers += stopper[i]->give_decay_sum_energy();

                multiplicity_of_front_absorbers_plus_stoppers++;
                multiplicity_of_rear_absorbers_plus_stoppers++;
                multiplicity_of_all_absorbers_plus_stoppers++;

            }

            position =  stopper[i]->give_universal_y_position_decay(&is_valid);
            if(is_valid)
            {
                // cout << "Receviced univeasl Y positon" << position << endl;
                spec_y_panorama_decay->manually_increment(i, position);
            }
        }

        // now the rear absorbers ===============================================
        for(uint i = 0 ; i < POSITRON_ABSORBER_NR ; i++)
        {
            bool is_valid = false;
            double position = rear_absorber->give_x_pos_plate(i, &is_valid);
            if(is_valid)
            {
                //           for(int m = 0 ; m < pixel_of_absorber; m++)
                spec_x_panorama_decay->manually_increment(i + 3, position);
                // Y positon has no strips, just when it fired - it fried
                for(int m = 0 ; m < 40; m++)  // trick to fill all bins in this column
                    spec_y_panorama_decay->manually_increment(i + 3, m);

                multiplicity_of_rear_absorbers_plus_stoppers++;
                multiplicity_of_all_absorbers_plus_stoppers++;
            }
        }

        sum_energy_of_rear_absorbers_plus_stoppers += rear_absorber->give_decay_sum_energy();

    }
    break;
    default:
        break;
    } // endswitch

    calculations_already_done = true ;
}
//*************************************************************************************************
#endif   // #ifdef MUN_ACTIVE_STOPPER_PRESENT



