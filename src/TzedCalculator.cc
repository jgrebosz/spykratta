#include "TzedCalculator.h"
#include "Tnamed_pointer.h"

//************************************************************************************
TzedCalculator::TzedCalculator(string myname, string conf_file): Tfrs_element(myname)
{
    //theFile = NULL;
//   theFile = new ConfigurationFile( conf_file.c_str() );
//   if( read_configuration() > 0 )
//     init_data();

  
    my_selfgate_type =  Tselfgate_type::Types::miniball; // fake, because...
    create_my_spectra();

    named_pointer[name_of_this_element + "__dE_from_ic__when_valid"]
    = Tnamed_pointer(&dE_gained, &zed_from_ic_validator, this) ;
    named_pointer[name_of_this_element + "__path__when_valid"]
    = Tnamed_pointer(&path_gained, &zed_from_path_validator, this) ;
    named_pointer[name_of_this_element + "__E_gained_when_valid"]
    = Tnamed_pointer(&energy_gained, &zed_from_ic_validator, this) ;  // can be also validated with the other validator


    named_pointer[name_of_this_element + "__zed_from_ic_value__when_valid"]
    = Tnamed_pointer(&zed_from_ic_value, &zed_from_ic_validator, this) ;


    named_pointer[name_of_this_element + "__zed_from_path_value__when_valid"]
    = Tnamed_pointer(&zed_from_path_value, &zed_from_path_validator, this) ;


    named_pointer[name_of_this_element + "__Z__when_valid"]
    = Tnamed_pointer(&zed_universal, &flag_zed_universal_ok, this) ;

}
//************************************************************************************
TzedCalculator::~TzedCalculator()
{
    //delete theFile;
//     for(int i = 0 ; i < theBanManager.size() ; i++)
//     {
//         delete theBanManager[i];        //  = new banManager
//     }
}
//************************************************************************************
void TzedCalculator::create_my_spectra()
{
    string folder = "TzedCalculator/";
    //-------------------------
    //--------------------
    string name = name_of_this_element + "__dEab_vs_E" ;
    spec_zed_dE_vs_E  =
        new spectrum_2D(name, name,
                        4000, 0, 8000,
                        4000, 0, 8000,
                        folder,
                        "active, when option mass__for_mass_calibration_use_zed_from_dE is= 1",
                        "X:  " + name_of_this_element + "__E_gained_when_valid" +
                        ",   Y:  " + name_of_this_element + "__dE_from_ic_when_valid"); // incrementor name
    frs_spectra_ptr->push_back(spec_zed_dE_vs_E) ;
    zed_dE_vs_E_matrix_name = name;


    //--------------------
    name = name_of_this_element + "__E_vs_path" ;
    spec_zed_E_vs_path  =
        new spectrum_2D(name, name,
                        4000, 0, 4000,
                        4000, 0, 8000,
                        folder,
                        "active, when option mass__for_mass_calibration_use_zed_from_dE is= 0",
                        "X:  " + name_of_this_element + "__path_when_valid" +
                        ",   Y:  " + name_of_this_element + "__E_gained_when_valid"); // incrementor name
    frs_spectra_ptr->push_back(spec_zed_E_vs_path) ;
    zed_E_vs_path_matrix_name = name;

    // 1 D spectra ----------------------

    name = name_of_this_element + "__zet_from_ic"  ;
    spec_zed_from_ic = new spectrum_1D(name, name,
                                       100, 1, 101,
                                       folder,
                                       "This is Zed deduced from bananas placed on the matrix:  " + spec_zed_dE_vs_E->give_name(),
                                       name_of_this_element + "__zet_from_ic");
    frs_spectra_ptr->push_back(spec_zed_from_ic) ;

    // -----------------
    name = name_of_this_element + "__zet_from_path"  ;
    spec_zed_from_path = new spectrum_1D(name, name,
                                         100, 1, 101,
                                         folder,
                                         "This is Zed deduced from bananas placed on the matrix:  " + spec_zed_E_vs_path->give_name(),
                                         name_of_this_element + "__zet_from_path");
    frs_spectra_ptr->push_back(spec_zed_from_path) ;


}
//**************************************************************************************
void TzedCalculator::make_preloop_action(std::ifstream &s)
{
    gain_energy = Tfile_helper::find_in_file(s,  name_of_this_element  + "__gain_energy");
    gain_range  = Tfile_helper::find_in_file(s,  name_of_this_element  + "__gain_range");

    int z_start = 0;
    //  int a_start = 0;
    fragment_type = (int)  Tfile_helper::find_in_file(s, "fragments_like");
    
    p_add_max = (int)Tfile_helper::find_in_file(s,   "p_max_added");
    p_rem_max = (int)Tfile_helper::find_in_file(s,   "p_max_remov");

    if(fragment_type == 0) { // for target like fragments
        z_start = (int)Tfile_helper::find_in_file(s, "mcp_target_zed");
    }
    else if (fragment_type == 1){ // For projectile like fragments
       z_start = (int)Tfile_helper::find_in_file(s, "mcp_beam_zed");
    }

    z_min = z_start - p_rem_max;
    z_max = z_start + p_add_max;

    read_configuration();
}
//**************************************************************************************
int TzedCalculator::read_configuration()
{
    int valid_conf = 0;
    int ii;

    /* z_min = 25;
     z_max = 27; */

    //z_min should not be larger than z_max!
    if(z_min > z_max)
    {
        ii    = z_min;
        z_min = z_max;
        z_max = ii;
    }

//     cout << "Reading the BANANAS  for two matrices " << name_of_this_element << endl ;


    string matix_name[2];
    matix_name[0] = zed_dE_vs_E_matrix_name;
    matix_name[1] = zed_E_vs_path_matrix_name;


    for(int matrix = 0 ; matrix <= 1 ; matrix++) // there are two matrices with similar
    {
        for(int z = z_min, i = 0 ; z <= z_max ; z++, i++) // we produce Zet bananas only in chosen region
        {
            ostringstream s;
            s << matix_name[matrix]  << "_polygon_zed_" << z << ".poly" ;
            string name_polygon = s.str();

            TjurekPolyCond *polygon_ptr ;

            if(!read_banana(name_polygon, &polygon_ptr))
            {
                // if the polygon does not exitst yet
                cerr << "During Reading-in the parameters for TzedCalculator "
                     << name_of_this_element
                     << " it is impossible to read polygon gate: \n"
                     << name_polygon
                     //<< "  - Most probably it does not exist \n"
                     << "\n!!! The polygon is going to be automaticaly created for you - set its position properly"
                     << endl;

                // creating it ----------------------------------------
                ofstream plik(("polygons/" + name_polygon).c_str());
                if(!plik)         exit(1);

                int offset_x = i * 200;
                int offset_y = i * 100;

                int points[16] =
                {
                    117,        1986,
                    172,        2159,
                    477,        2481,
                    841,        2779,
                    1120,       2907,
                    1064,        2716,
                    759,        2415,
                    371,        2077
                };

                for(int n = 0 ; n < 16 ; n += 2)
                    plik << points[n] + offset_x << "\t" << points[n + 1] + offset_y << "\n";

                plik.close();
                // once more we try something what really exit
                if(! read_banana(name_polygon, &polygon_ptr))
                {
                    cout << "Could not create the polygon " << name_polygon << " for you " << endl;
                    exit(125);
                }
            }
            // here the polygon should be ready
//             if(z == 25) cout << name_polygon << " : \n" << polygon_ptr->printout_points()  << endl;

//          std::vector< pair<TjurekPolyCond*, int> >
            if(matrix == 0) zed_dE__banana_vector.push_back(pair<TjurekPolyCond*, int>(polygon_ptr, z));
            if(matrix == 1) zed_E_vs_path__banana_vector.push_back(pair<TjurekPolyCond*, int>(polygon_ptr, z));
        }
    }// end for matrix



#if 0
    // for checking
    for(unsigned nr = 0; nr < zed_dE__banana_vector.size(); nr++)
    {
        cout << nr << " Banana polygon gate called : "
             << zed_dE__banana_vector[nr].first->give_name()

             << " and " << zed_E_vs_path__banana_vector[nr].first->give_name()

             << " represents z = "
             << zed_dE__banana_vector[nr].second
             <<
             endl;
        // create_user_spectrum(names[nr]);
    }
#endif

//     exit(77);

    valid_conf = 1;
    return valid_conf;
}
//****************************************************************
void TzedCalculator::init_data()
{
#if 1

#else
    @@@@@@@@@@@@@@@@@@@@@@


// NOTE: THIS IS NEVER CALLED

    ///////////////////////////////////////
    /// Banana(s)
    //////////////////////////////////////
    theBanManager.clear();
    theBanManager.resize(z_max + 1);

    char endline[16];
    string filename;

    for(int ii = z_min; ii <= z_max; ii++)
    {
        sprintf(endline, "%2.2d.ban", ii);
        filename = ban_file_base + string(endline);

        theBanManager[ii] = new banManager(filename, ban_res_x, ban_res_y, ii, (double)ii);
    }

    ///////////////////////////////////////
    ///// Check overlaps            //////
    /////////////////////////////////////

    banManager* checkManager1 = 0;
    banManager* checkManager2 = 0;

    banGate* checkGate1 = 0;
    banGate* checkGate2 = 0;

    // have to loop on Z and on possible sub-bananas

    if(z_min < z_max)
    {
        for(int ii = z_min; ii <= z_max; ii++)
        {
            // loop on Z
            checkManager1 = theBanManager[ii];

            for(int jj = 0; jj < checkManager1->howManyGates(); jj++)
            {
                // in principle loop on all possible gates, in practice we expect only one!
                checkGate1 = checkManager1->getBanGate(jj);

                for(int kk = ii + 1; kk <= z_max; kk++)
                {
                    // second loop on Z
                    checkManager2 = theBanManager[kk];

                    for(int rr = 0; rr < checkManager2->howManyGates(); rr++)
                    {
                        // in principle loop on all possible gates, in practice we expect only one!
                        checkGate2 = checkManager2->getBanGate(rr);

                        if(checkGate1->overlapBan(checkGate2))
                        {
                            cout << " ==> Warning! Found overlap between bananas for Z = " << ii << " and Z = " << jj << endl;
                        }
                    }
                }
            }
        }
    }
#endif
}
//************************************************************************************
/**
 * @brief There is not simple way for calculating ZED.
 * So we do it with the help of the matrix.
 * We put there some bananas representing particular Zet values.
 * This funtion just checks it the current point in in one of bananas.
 *
 * There are two matrices, so modes of operation of this function (depending on argument 'type'
 *
 * @param path ...
 * @param energy ...
 * @param type ..- wher from we take zet (one of two posisble matrices:
 * spec_zed_dE_vs_E  (type TRUE)
 * or
 * spec_zed_E_vs_path (when type = FALSE)
 *
 * @return int
 **/

int TzedCalculator::Zed_remove(double path, double energy, bool type)     // type true = EdE (IC), false = path
{
#if 0
//     cout <<  " TzedCalculator::Zed(double path, double energy, bool type)  where:\n"
//          << "\t path=" << path
//          << ", energy = " << energy
//          << ", type="  << type << endl;

    zed = 0;
    valid = false; // put zeroing somewhere else

    energy_gained = gain_energy * energy;
    if(type)
    {
        dE_gained = gain_range * path ;
        spec_zed_dE_vs_E->manually_increment(energy_gained,   dE_gained);
        // now we can look if this point is on one of Zet bananas

        // loop over all registered bananas form this matrix
        // for checking
        if(energy_gained || dE_gained) // for zeros does not make sense to check
            for(unsigned nr = 0; nr < zed_dE__banana_vector.size(); nr++)
            {
                // check the point (x=energy_gained, y  dE_gained  - if it in the banana
                if(zed_dE__banana_vector[nr].first->Test(energy_gained, dE_gained))
                {
                    zed = zed_E_vs_path__banana_vector[nr].second;

//                 cout << nr << "!!!!!!!!!!!!!! The point is inside polygon gate called : "
//                      << zed_dE__banana_vector[nr].first->give_name()
//
// //                      << " and " << zed_E_vs_path__banana_vector[nr].first->give_name()
//
//                      << " so it means it is  z = " << zed
//                      << endl;
                    spec_zed_from_ic->manually_increment(zed);
                    // TODO: set the incrementer value
                    valid = true;
                    break;

                }
//             else
//             {
//                 cout << "Point x = " << energy_gained << ", y=" << dE_gained
//                      << " is outside polygon " << zed_dE__banana_vector[nr].first->give_name()
//                      << endl;
//             }
            }

        // return particular zet
    }
    else
    {
        path_gained = gain_range * path ;
        spec_zed_E_vs_path->manually_increment(path_gained, energy_gained);
// loop over all registered bananas form this matrix
        // for checking
        if(energy_gained || path_gained) // for zeros does not make sense to check
            for(unsigned nr = 0; nr < zed_E_vs_path__banana_vector.size(); nr++)
            {
                // check the point (x=energy_gained, y  dE_gained  - if it in the banana
                if(zed_E_vs_path__banana_vector[nr].first->Test(path_gained, energy_gained))
                {
                    zed = zed_E_vs_path__banana_vector[nr].second;

//                 cout << nr << "!!!!!!!!!!!!!! The point is inside polygon gate called : "
//                      << zed_dE__banana_vector[nr].first->give_name()
//
// //                      << " and " << zed_E_vs_path__banana_vector[nr].first->give_name()
//
//                      << " so it means it is  z = " << zed
//                      << endl;
                    spec_zed_from_path->manually_increment(zed);
                    // TODO: set the incrementer value
                    valid = true;
                    break;

                }
            } // end for nr
    } // end else type


    //cout << "TzedCalculator::Zed = " << zed << endl;
#if 0
    for(int ii = z_min; ii <= z_max; ii++)
    {
        if(theBanManager[ii]->inside(z_min, gain_range * path, gain_energy * energy))   // warning - inversed x, y
        {
            if(multi_bananas)
                zed += theBanManager[ii]->getP_Id();
            else
                return theBanManager[ii]->getP_Id();
        }
    }
    //cout << "zed = " << zed << endl;
    if(type)
    {
        spec_zed_ic->manually_increment(zed);
    }
    else
    {
        spec_zed_path->manually_increment(zed);
    }
#endif

    return zed;
#endif
    return -1;
}
//************************************************************************************
void TzedCalculator::analyse_current_event()
{
// empty, all calcuations are done from Prisma Mangager
// by calling twice Zed()
// once using really path, second time using ic_eE_energy
}
//************************************************************************************
int TzedCalculator::deduce_zed_from_bananas_on_matrix_E_vs_path(double path, double energy)
{

//     cout <<  " TzedCalculator::calculate_Zed_from_path(double path, double energy)  where:\n"
//          << "\t path=" << path
//          << ", energy = " << energy
    zed_from_path_value = 0;
    zed_from_path_validator = false;

    energy_gained = gain_energy * energy;
    path_gained = gain_range * path ;

    spec_zed_E_vs_path->manually_increment(path_gained, energy_gained);
// loop over all registered bananas form this matrix
    // for checking
    if(energy_gained || path_gained) // for zeros does not make sense to check
        for(unsigned nr = 0; nr < zed_E_vs_path__banana_vector.size(); nr++)
        {
            // check the point (x=energy_gained, y  dE_gained  - if it in the banana
            if(zed_E_vs_path__banana_vector[nr].first->Test(path_gained, energy_gained))
            {
                zed_from_path_value = zed_E_vs_path__banana_vector[nr].second;
                zed_from_path_validator = true;
                spec_zed_from_path->manually_increment(zed_from_path_value);

                zed_universal = zed_from_path_value;
                flag_zed_universal_ok = true;

//                 cout << nr << "!!!!!!!!!!!!!! The point is inside polygon gate called : "
//                      << zed_dE__banana_vector[nr].first->give_name()
//
// //                      << " and " << zed_E_vs_path__banana_vector[nr].first->give_name()
//
//                      << " so it means it is  z = " << zed
//                      << endl;

                break;

            }
        } // end for nr


    return zed_from_path_value;
}
//************************************************************************************
int TzedCalculator::deduce_zed_from_bananas_on_matrix_dE_vs_E(double ic_energy_AB_DE, double ic_energy)
{
    zed_from_ic_value = 0 ;
    zed_from_ic_validator = false;

    energy_gained = gain_energy * ic_energy;
    dE_gained = gain_range * ic_energy_AB_DE ;

    spec_zed_dE_vs_E->manually_increment(energy_gained,   dE_gained);
    // now we can look if this point is on one of Zet bananas

    // loop over all registered bananas form this matrix
    // for checking
    if(energy_gained || dE_gained) // for zeros does not make sense to check
        for(unsigned nr = 0; nr < zed_dE__banana_vector.size(); nr++)
        {
            // check the point (x=energy_gained, y  dE_gained  - if it in the banana
            if(zed_dE__banana_vector[nr].first->Test(energy_gained, dE_gained))
            {
                zed_from_ic_value = zed_E_vs_path__banana_vector[nr].second;
                zed_from_ic_validator = true;
//                 cout << nr << "!!!!!!!!!!!!!! The point is inside polygon gate called : "
//                      << zed_dE__banana_vector[nr].first->give_name()
// //                      << " and " << zed_E_vs_path__banana_vector[nr].first->give_name()
//                      << " so it means it is  z = " << zed
//                      << endl;
                spec_zed_from_ic->manually_increment(zed_from_ic_value);
                break;

            }
//             else
//             {
//                 cout << "Point x = " << energy_gained << ", y=" << dE_gained
//                      << " is outside polygon " << zed_dE__banana_vector[nr].first->give_name()
//                      << endl;
//             }
        }

    return zed_from_ic_value;
}
//****************************************************************
void TzedCalculator::zeroing_variables()
{
   zed_from_ic_value = 0;
   zed_from_ic_validator = false;

   zed_from_path_value = 0; 
   zed_from_path_validator = false;

    // only one of them is valid at the time.(depending on wish
    // in PrismaManager:  flag_zet_from_dE
    // To make it universal - below there is a universal Z value, taken from
    //     on e of those above
   zed_universal= 0;
   flag_zed_universal_ok = false;
   energy_gained = 0;
path_gained = 0;
    dE_gained = 0;
    }
    //********************************************
