#include "TmassCalculator.h"
#include "Tnamed_pointer.h"
#include "TprismaManager.h"

#if  CURRENT_EXPERIMENT_TYPE == PRISMA_EXPERIMENT
const double PI = 3.14159265;
const double RADIANS_TO_DEGREES = 180./ (PI);

//******************************************************************************
TmassCalculator::TmassCalculator(string myname,     TprismaManager *  ptr_prisma_manager_):
    Tfrs_element(myname),
    inv_c_light(100. / 2.99792458),
    ptr_prisma_manager(ptr_prisma_manager_)
{
    theFile = NULL;

    /*  theFile = new ConfigurationFile( conf_file.c_str() );
      if( this->read_configuration() > 0 )
        this->
        init_data();*/



    create_my_spectra();

//--------- incrementers
    named_pointer[name_of_this_element + "__a_over_q_when_valid"]
    = Tnamed_pointer(&a_over_q, &a_over_q_ok, this) ;

//      named_pointer[name_of_this_element + "__a_over_q_when_valid"]
//     = Tnamed_pointer(&a_over_q, &a_over_q_ok, this) ;

    named_pointer[name_of_this_element + "__radius_multiplied_by_velocity"]
    = Tnamed_pointer(&radius_multiplied_by_velocity, &a_over_q_ok, this) ;

    named_pointer[name_of_this_element + "__energy_of_ion_chamber"]
    = Tnamed_pointer(&energy_of_ion_chamber, &a_over_q_ok, this) ;

    named_pointer[name_of_this_element + "__Q_charge_state__when_ok"]
    = Tnamed_pointer(&charge, &charge_ok, this) ;

    named_pointer[name_of_this_element + "__mass__when_ok"]
    = Tnamed_pointer(&mass, &mass_ok, this) ;

    named_pointer[name_of_this_element + "__A_(mass_number)_when_ok"]
    = Tnamed_pointer(&mass_num, &mass_num_ok, this) ;

    named_pointer[name_of_this_element + "_flag__zed_is_deduced_from_dE_vs_E_matrix"]
    = Tnamed_pointer(&flag_zed_from_dE_vs_E__otherwise_from_path,NULL, this) ;

    named_pointer[name_of_this_element + "__TKEL_total_kinetic_energy_loss"]
    = Tnamed_pointer(&total_kinetic_energy_loss, &total_kinetic_energy_loss_ok, this) ;

}
//******************************************************************************
TmassCalculator::~TmassCalculator()
{
//   delete theFile;

    for(int i = 0 ; i < theBanManager.size(); i++)
    {
        delete  theBanManager[i];
    }

    for(int i = 0 ; i < theMasManager.size(); i++)
    {
        delete  theMasManager[i];
    }
//     for(int i = 0 ; i < theChargeManager.size(); i++)
//     {
//         delete  theChargeManager[i];
//     }
    delete theCalManager;           // = new calManager( cal_file );

#ifdef ABERRATIONS___bad_algorithm
    delete theXmcManager;       // = new calManager( xmc_file );
    delete theYmcManager ;      // = new calManager( ymc_file );
    delete   theXfpManager;     //  = new calManager( xfp_file );
    //if
    delete    theXmcSpliManager ;   // = new curveManager( xmc_file );
    //if( ymc_uspl )
    delete  theYmcSpliManager ; // = new curveManager( ymc_file );
    // if( xfp_uspl )
    delete theXfpSpliManager;       // = new curveManager( xfp_file );
#endif
}
//**************************************************************************************
void TmassCalculator::create_my_spectra()
{
    string folder = "TmassCalculator/";
    //-------------------------
    string name = name_of_this_element + "__mass";
    //cout << "name = [" << name << "]" << endl;
    spec_mass  = new spectrum_1D(name, name,
                                 2000, 0, 200,
                                 folder,
                                 "",
                                 name);  // incrementor name
    frs_spectra_ptr->push_back(spec_mass) ;
    //--------------------

    name = name_of_this_element + "__a_over_q_uncal";
    spec_aoq_uncal  = new spectrum_1D(name, name,
                                      8000, 1, 1000,
                                      folder,
                                      "",
                                      "Please use the calibrated version");   // incrementor name
    frs_spectra_ptr->push_back(spec_aoq_uncal) ;
    //--------------------

    name = name_of_this_element + "__a_over_q_cal";
    spec_aoq_cal  = new spectrum_1D(name, name,
                                    8000, 1, 1000,
                                    folder,
                                    "",
                                    name_of_this_element + "__a_over_q_when_valid"); // incrementor name
    frs_spectra_ptr->push_back(spec_aoq_cal) ;


    //--------------------

    // this should be as many as many different zed values we expect z_min, z_max
    name = name_of_this_element + "__charge_states";
    spec_charge_states__for_any_zed  = new spectrum_2D(name, name,
            2500, 500, 5000,
            2500, 700, 10000,
            folder,
            "",
            "X:  " + name_of_this_element + "__radius_multiplied_by_velocity" +
            "     Y:  " + name_of_this_element + "__energy_of_ion_chamber"
                                                      );  // incrementor name
    frs_spectra_ptr->push_back(spec_charge_states__for_any_zed) ;
    //--------------------

    /*
    name = name_of_this_element + "__curvature_radius";
    cout << "name = [" << name << "]" << endl;
    spec_radius  = new spectrum_1D( name, name,
                                      8000, 0, 10,
                                      folder,
                                      "",
                                      name);  // incrementor name
    frs_spectra_ptr->push_back(spec_radius) ;*/





    name = name_of_this_element + "__deduced_charged_states";
    cout << "name = [" << name << "]" << endl;
    spec_deduced_charged_states  = new spectrum_1D(name, name,
            200, 0, 200,
            folder,
            "If this spectrum is empty, look at position of banana gates on matrix: " +
            spec_charge_states__for_any_zed->give_name(),
            "X:  " + name_of_this_element + "__charge_state");  // incrementor name
    frs_spectra_ptr->push_back(spec_deduced_charged_states) ;




    name = name_of_this_element + "__radius_multiplied_by_velocity";
    spec_radius_multiplied_by_velocity  = new spectrum_1D(name, name,
            1000,
            0,
            5000,
            folder,
            "",
            "X:  " + name_of_this_element + "__radius_multiplied_by_velocity"
                                                         );
    frs_spectra_ptr->push_back(spec_radius_multiplied_by_velocity) ;
    //--------------------
}
//******************************************************************************
void TmassCalculator::make_preloop_action(std::ifstream & s)
{
    int valid_conf = 0;
    int ii;

    string mass_table_filename;

   Tfile_helper::spot_in_file(s,  name_of_this_element  + "__mass_table_file");
   
    s >>  mass_table_filename ;

    //cout << "FF = " << mass_table_filename << endl;
    

   // std::string mass_table_file = "/home/online/prisma01/calibration/ame2012.txt";

    ifstream mass_input(mass_table_filename.c_str());
    if(!mass_input)
    {
        std::cerr << "Error input mass table file does not exist ! " << std::endl;
        std::cerr << "Should be at path: " << mass_table_filename << std::endl;
        exit(22);
    }


    for(int z_index = 0 ; z_index < 120 ; z_index++)
    {
        for(int a_index = 0 ; a_index < 300 ; a_index++)
        {
            ame_table[z_index][a_index] = 0.;
            qreact_value[z_index][a_index] = 0.;
        }
    }

    while(mass_input.good())
    {
        int temp_z, temp_a ;
        mass_input >> temp_z >> temp_a ;
        mass_input >> ame_table[temp_z][temp_a];
//  cout << temp_z << '\t' << temp_a << '\t' << ame_table[temp_z][temp_a] << endl;
    }


    // magnetic field

   // B_dipole = Tfile_helper::find_in_file(s,  name_of_this_element  + "__B_dipole") * tesla;
    TfastSolver* ptr_fastsolver = ptr_prisma_manager->give_fastsolver_ptr();
   
    B_dipole = ptr_fastsolver->Get_B_dipole();
    
    cout << "B dipole in mass Calculator routine = " << B_dipole << " T" << endl;
    
    // calibration file for A/Q

    a_over_q_gain = Tfile_helper::find_in_file(s,  name_of_this_element  + "__a_over_q_gain");
    a_over_q_offs = Tfile_helper::find_in_file(s,  name_of_this_element  + "__a_over_q_offs");

    /// multiplicative factors
    r_fact = Tfile_helper::find_in_file(s,  name_of_this_element  + "__r_fact");
    e_fact = Tfile_helper::find_in_file(s,  name_of_this_element  + "__e_fact");

//    z_min = Tfile_helper::find_in_file(s,  "z_min");
//    z_max = Tfile_helper::find_in_file(s,  "z_max");

    fragment_type = (int) Tfile_helper::find_in_file(s, "fragments_like");

    p_add_max = (int) Tfile_helper::find_in_file(s,   "p_max_added");
    p_rem_max = (int)Tfile_helper::find_in_file(s,   "p_max_remov");

    n_add_max = (int) Tfile_helper::find_in_file(s,   "n_max_added");
    n_rem_max = (int) Tfile_helper::find_in_file(s,   "n_max_remov");

    max_charge_states = (int) Tfile_helper::find_in_file(s,   "max_Q_states");

    flag_zed_from_dE_vs_E__otherwise_from_path = (int) Tfile_helper::find_in_file(s,
            name_of_this_element  +
            "__for_mass_calibration_use_zed_from_dE");


    z_start = 0;
    a_start = 0;

    TprismaMcp *ptr_mcp = ptr_prisma_manager->give_mcp_ptr();

    if(fragment_type == 0)   // for target like fragments
    {
        z_start = ptr_mcp->get_target_zed();
        a_start = ptr_mcp->get_target_mass_number();
    }
    else if(fragment_type == 1)   // For projectile like fragments
    {
        z_start = ptr_mcp->get_beam_zed();
        a_start = ptr_mcp->get_beam_mass_number();
    }

    z_min = z_start - p_rem_max;
    z_max = z_start + p_add_max;

    a_min = a_start - (p_rem_max + n_rem_max);
    a_max = a_start + (p_add_max + n_add_max);

    // charge states

    q_max = z_max;
    q_min = z_min - max_charge_states;

//     cout
//             << "Z_min = " << z_min << ", Z_max " << z_max
//             << "\nA_min = " << a_min << ", a_max " << a_max
//             << "\nQ_min = " << q_min << ", Q_max " <<q_max<< endl;

    cout << '\t' << '\t' ;
    int temp_index=1;
    for(int index_a = 0 ; a_min+2*p_add_max+index_a<=a_max ; index_a++)
    {
        if(a_min-a_start+index_a+p_add_max <= 0) cout << (a_min+p_add_max)-a_start+index_a << "n" << '\t' << '\t' ;
        else cout << index_a-n_rem_max << "n" << '\t' << '\t' ;
    }
    cout << endl;
    for(int index_z = z_start + p_add_max ; index_z>=z_min ; index_z--)
    {
        cout << index_z-z_start << "p" << '\t' ;
        //Dcout << a_start+(index_z-z_start)-n_rem_max << '\t' << a_start+(index_z-z_start)+n_add_max << endl;
        for(int index_a = a_start+(index_z-z_start)-n_rem_max; index_a<=a_start+(index_z-z_start)+n_add_max ; index_a++)
        {
            //cout << index_z << '\t' << index_a << endl;
            qreact_value[index_z][index_a] = qreact_gs_to_gs(index_z,index_a);
            cout << qreact_value[index_z][index_a] << '\t';
        }
        temp_index--;
        cout << endl;
    }



    massA_bananas_vector_for_zed.clear();
    charge_state_bananas_vector_for_zed.clear();


// MAKE charge states  spectrum for each Z
    for(int z = z_min ; z <= z_max ; z++)
    {

        // create spectrum - if it does not exist yet
        string matix_name = spec_charge_states__for_any_zed->give_name();
        ostringstream s;
        s << "_for_Z_" << z ;
        matix_name += s.str();



        //
        //     cout << "Reading the BANANAS  for charge_states  " << name_of_this_element << endl ;
        if(charge_state_bananas_vector_for_zed.size() <= z)
        {

            charge_state_bananas_vector_for_zed.resize(z + 1);
            spec_charge_states__for_z.resize(z + 1);
            //spec_mass_vs_xfp__for_z.resize(z + 1);
        }

        if(spec_charge_states__for_z[z] == NULL)
        {
            spec_charge_states__for_z[z] =
                new spectrum_2D(matix_name, matix_name,
                                2500, 1000, 3000,
                                2500, 1500, 8000,
                                "TmassCalculator/",
                                "",
                                "X:  " + name_of_this_element + "__radius_multiplied_by_velocity" +
                                "     Y:  " + name_of_this_element + "__energy_of_ion_chamber"
                                + "   (with condition on Zed)"
                               );  // incrementor name
            frs_spectra_ptr->push_back(spec_charge_states__for_z[z]) ;
        }

        // now bananas for charge state matrices
        int current_q_max = q_max;
        if(q_max > z) current_q_max = z;

        int current_q_min = q_min;
        if(q_min < z-max_charge_states) current_q_min = z-max_charge_states;

        for(int q = current_q_min, i = 0  ; q <= current_q_max ; q++, i++) // we produce Zet bananas only in chosen region
        {

            // at first make the polygons

            ostringstream s;
            s << matix_name  << "_polygon_q_" << q << ".poly" ;
            string name_polygon = s.str();

            TjurekPolyCond *polygon_ptr ;

            if(!read_banana(name_polygon, &polygon_ptr))
            {
                // if the polygon does not exitst yet
                cerr << "During Reading-in the parameters for TmassCalculator "
                     << name_of_this_element
                     << " it is impossible to read polygon gate: \n"
                     << name_polygon
                     //<< "  - Most probably it does not exist \n"
                     << "\n!!! The polygon is going to be automaticaly created for you - set its position properly"
                     << endl;

                // creating it ----------------------------------------
                ofstream plik(("polygons/" + name_polygon).c_str());
                if(!plik)         exit(1);


                double x_edge_of_matrix =
                    spec_charge_states__for_z[z]->give_root_spectrum_pointer()->GetXaxis()->GetBinLowEdge(0);
                double offset_x = i * 110 + x_edge_of_matrix - 400; // 400 is cosmetics
                int offset_y = i * 100;

                int points[16] =
                {
                    592, 1982,
                    575, 2237,
                    583, 2581,
                    599, 2857,
                    640, 3118,
                    663, 2878,
                    659, 2545,
                    646, 2175
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
            charge_state_bananas_vector_for_zed[z].push_back(pair<TjurekPolyCond*, int>(polygon_ptr, q));

            // as a result of events passing from this polygon the folloiwng matrix will be incremented

            if(spec_a_over_q_vs_xfp__for_z_for_q.size() < z + 1)
            {
                spec_a_over_q_vs_xfp__for_z_for_q.resize(z + 1);
            }

            if(spec_a_over_q_vs_xfp__for_z_for_q[z].size() < q + 1)
            {
                spec_a_over_q_vs_xfp__for_z_for_q[z].resize(q + 1);
            }


            if(!spec_a_over_q_vs_xfp__for_z_for_q[z][q])
            {
                ostringstream s;
                s << name_of_this_element << "__a_over_q_vs_x_fp___for_Z_"
                  << z << "_for_q_" << q ;
                string name = s.str();
                spec_a_over_q_vs_xfp__for_z_for_q[z][q] =
                    new spectrum_2D(name, name,
                                    1000, 0, 1,
                                    1000, 200, 600,
                                    "TmassCalculator/",
                                    "",
                                    "X:   PPAC__x_fp_in_meters_when_valid "
                                    "   Y:  " + name_of_this_element +
                                    "__a_over_q_when_valid  (with conditions on Z and Q"); // incrementor name
                frs_spectra_ptr->push_back(spec_a_over_q_vs_xfp__for_z_for_q[z][q]) ;
            }
        } // for(int q

        make_A_bananas_on_the_matrices_A_vs_Xfp__for_z(z);


    }// for(int z

#ifdef ABERRATIONS___bad_algorithm
    // correction for aberrations
    xmc_file = string(theFile->get_string("xmc_file", "xmc.cal"));
    xmc_gain = Tfile_helper::find_in_file(s,  name_of_this_element  + "_xmc_gain",  1.);
    xmc_offs = Tfile_helper::find_in_file(s,  name_of_this_element  + "_xmc_offs",  0.);

    ymc_file = string(theFile->get_string("ymc_file", "ymc.cal"));
    ymc_gain = Tfile_helper::find_in_file(s,  name_of_this_element  + "_ymc_gain",  1.);
    ymc_offs = Tfile_helper::find_in_file(s,  name_of_this_element  + "_ymc_offs",  0.);

    xfp_file = string(theFile->get_string("xfp_file", "xfp.cal"));
    xfp_gain = Tfile_helper::find_in_file(s,  name_of_this_element  + "_xfp_gain",  1.);
    xfp_offs = Tfile_helper::find_in_file(s,  name_of_this_element  + "_xfp_offs",  0.);

    xmc_spli = string(theFile->get_string("xmc_spli", "xmc.spli"));
    xmc_uspl = (int) Tfile_helper::find_in_file(s,  name_of_this_element  + "_xmc_uspl",  0);

    ymc_spli = string(theFile->get_string("ymc_spli", "ymc.spli"));
    ymc_uspl = (int) Tfile_helper::find_in_file(s,  name_of_this_element  + "_ymc_uspl",  0);

    xfp_spli = string(theFile->get_string("xfp_spli", "xfp.spli"));
    xfp_uspl = (int) Tfile_helper::find_in_file(s,  name_of_this_element  + "_xfp_uspl",  0);

#endif

    valid_conf = 1;

    /////////////////////////////////////////////////
    ////////// Calibration for charge states
    ////////////////////////////////////////////////


//     char endline[16] = { 0};
//     string filename;
//
//
//     theChargeManager.clear();
//     theChargeManager.resize(z_max + 1);
//
//     for(int ii = z_min; ii <= z_max; ii++)
//     {
//         sprintf(endline, "%2.2d.cal", ii);
//         filename = chg_file_base + string(endline);
//
//         theChargeManager[ii] = new calManager(filename);
//     }

    ////////////////////////////////////////////////
    /// Calibration for A/Q
    /////////////////////////////////////////////////
    //   cal_file = string( theFile->get_string("cal_file", "a_over_q.cal") );

    Tfile_helper::spot_in_file(s,  name_of_this_element  + "__a_over_q_calibrations");
    theCalManager = new calManager(s, 6.0);  // has 6 lines of numbers

    theCalManager->setGain(a_over_q_gain);
    theCalManager->setOffset(a_over_q_offs);

#ifdef ABERRATIONS___bad_algorithm
    ////////////////////////////////////////////////
    /// Calibration for aberrations
    /////////////////////////////////////////////////
    theXmcManager = new calManager(xmc_file);

    theXmcManager->setGain(xmc_gain);
    theXmcManager->setOffset(xmc_offs);

    theYmcManager = new calManager(ymc_file);

    theYmcManager->setGain(ymc_gain);
    theYmcManager->setOffset(ymc_offs);

    theXfpManager = new calManager(xfp_file);

    theXfpManager->setGain(xfp_gain);
    theXfpManager->setOffset(xfp_offs);

    if(xmc_uspl)
        theXmcSpliManager = new curveManager(xmc_file);

    if(ymc_uspl)
        theYmcSpliManager = new curveManager(ymc_file);

    if(xfp_uspl)
        theXfpSpliManager = new curveManager(xfp_file);
#endif

    a_over_q_ok = false;

}
//******************************************************************************
int TmassCalculator::read_configuration()
{
    int valid_conf = 0;
    int ii;
    /*
        z_min = theFile->get_int("z_min",  1);
        z_max = theFile->get_int("z_max",  2);

        // z_min should not be larger than z_max!
        if(z_min > z_max)
        {
            ii    = z_min;
            z_min = z_max;
            z_max = ii;
        }

        // charge states
        q_min = theFile->get_int("q_min",  1);
        q_max = theFile->get_int("q_max",  2);

        // q_min should not be larger than q_max!
        if(q_min > q_max)
        {
            ii    = q_min;
            q_min = q_max;
            q_max = ii;
        }

        // q_max should not be larger than z_max!
        if(q_max > z_max)
            q_max = z_max;

        // mass numbers
        a_min = theFile->get_int("a_min",  1);
        a_max = theFile->get_int("a_max",  2);

        // a_min should not be larger than a_max!
        if(a_min > a_max)
        {
            ii    = a_min;
            a_min = a_max;
            a_max = ii;
        }

        // a_max should not be smaller than z_max!
        if(a_max < z_max)
            a_max = z_max;
        if(a_min < z_min)
            a_min = z_min;


        //  z_max++;
        //  q_max++;
        //  a_max++;
    */
    // banana
    ban_res_x = theFile->get_int("ban_res_x",  8192);
    ban_res_y = theFile->get_int("ban_res_y",  8192);
    /*
        ban_file_base = string(theFile->get_string("ban_file_base", "charge_ban_"));
        mas_file_base = string(theFile->get_string("mas_file_base", "mass_ban_"));
        chg_file_base = string(theFile->get_string("chg_file_base", "charge_cal_"));*/

    // magnetic field
    B_dipole = theFile->get_double("B_dipole",  0.5) * tesla;

    // calibration file for A/Q
    //cal_file = string(theFile->get_string("cal_file", "a_over_q.cal"));
    a_over_q_gain = theFile->get_double("a_over_q_gain",  1.);
    a_over_q_offs = theFile->get_double("a_over_q_offs",  0.);

    /// multiplicative factors
    r_fact = theFile->get_double("r_fact",  1.0);
    e_fact = theFile->get_double("e_fact",  1.0);

    //multi_bananas = theFile->get_int("multi_bananas",  0);  // 0 --> no overlaps between bananas; 1 --> overlaps allowed

#ifdef ABERRATIONS___bad_algorithm
    // correction for aberrations
    xmc_file = string(theFile->get_string("xmc_file", "xmc.cal"));
    xmc_gain = theFile->get_double("xmc_gain",  1.);
    xmc_offs = theFile->get_double("xmc_offs",  0.);

    ymc_file = string(theFile->get_string("ymc_file", "ymc.cal"));
    ymc_gain = theFile->get_double("ymc_gain",  1.);
    ymc_offs = theFile->get_double("ymc_offs",  0.);

    xfp_file = string(theFile->get_string("xfp_file", "xfp.cal"));
    xfp_gain = theFile->get_double("xfp_gain",  1.);
    xfp_offs = theFile->get_double("xfp_offs",  0.);

    xmc_spli = string(theFile->get_string("xmc_spli", "xmc.spli"));
    xmc_uspl = theFile->get_int("xmc_uspl",  0);

    ymc_spli = string(theFile->get_string("ymc_spli", "ymc.spli"));
    ymc_uspl = theFile->get_int("ymc_uspl",  0);

    xfp_spli = string(theFile->get_string("xfp_spli", "xfp.spli"));
    xfp_uspl = theFile->get_int("xfp_uspl",  0);

#endif

    valid_conf = 1;
    return valid_conf;
}
//******************************************************************************
void TmassCalculator::init_data()
{
    char endline[16];
    string filename;

#if 0
    ///////////////////////////////////////
    /// Banana(s)
    //////////////////////////////////////
    theBanManager.clear();
    theBanManager.resize((z_max + 1) * (z_max + 1));



    for(int ii = z_min; ii <= z_max; ii++)
    {
        for(int jj = q_min; jj <= q_max; jj++)
        {
            sprintf(endline, "%2.2d_%2.2d.ban", ii, jj);
            filename = ban_file_base + string(endline);

            // weight (and pin) is the charge state!
            theBanManager[ii * z_max + jj] = new banManager(filename, ban_res_x, ban_res_y, jj, (double)jj);
        }
    }

    ////////////////////////////////////////////////
    /// Check for overlaps in charge state bananas
    ////////////////////////////////////////////////

    banManager* checkManager1 = 0;
    banManager* checkManager2 = 0;

    banGate* checkGate1 = 0;
    banGate* checkGate2 = 0;

    if(z_min < z_max)
    {
        for(int ss = z_min; ss <= z_max; ss++)
        {
            // outer loop on Z; then for each Z loops on charge states ...
            if(q_min < q_max)
            {
                for(int ii = q_min; ii <= q_max; ii++)
                {
                    // loop on charge states
                    checkManager1 = theBanManager[ss * z_max + ii];

                    for(int jj = 0; jj < checkManager1->howManyGates(); jj++)
                    {
                        // in principle loop on all possible gates, in practice we expect only one!
                        checkGate1 = checkManager1->getBanGate(jj);

                        for(int kk = ii + 1; kk <= q_max; kk++)
                        {
                            // second loop on charge states
                            checkManager2 = theBanManager[ss * z_max + kk];

                            for(int rr = 0; rr < checkManager2->howManyGates(); rr++)
                            {
                                // in principle loop on all possible gates, in practice we expect only one!
                                checkGate2 = checkManager2->getBanGate(rr);

                                if(checkGate1->overlapBan(checkGate2))
                                {
                                    cout << " ==> Warning! Found overlap between bananas for Z = " << ss << ", q = " << ii << " and q = " << kk << endl;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
#endif   // 0

#if 0
    theMasManager.clear();
    theMasManager.resize((a_max + 1) * (a_max + 1));

    for(int ii = z_min; ii <= z_max; ii++)
    {
        for(int jj = a_min; jj <= a_max; jj++)
        {
            sprintf(endline, "%2.2d_%3.3d.ban", ii, jj);
            filename = mas_file_base + string(endline);

            // weight (and pin) is the mass number!
            theMasManager[ii * a_max + jj] = new banManager(filename, ban_res_x, ban_res_y, jj, (double)jj);
        }
    }

    ////////////////////////////////////////////////
    /// Check for overlaps in mass number bananas
    ////////////////////////////////////////////////
    if(z_min < z_max)
    {
        for(int ss = z_min; ss <= z_max; ss++)
        {
            // outer loop on Z; then for each Z loops on masses ...
            if(a_min < a_max)
            {
                for(int ii = a_min; ii <= a_max; ii++)
                {
                    // loop on mass numbers
                    checkManager1 = theMasManager[ss * a_max + ii];

                    for(int jj = 0; jj < checkManager1->howManyGates(); jj++)
                    {
                        // in principle loop on all possible gates, in practice we expect only one!
                        checkGate1 = checkManager1->getBanGate(jj);

                        for(int kk = ii + 1; kk <= a_max; kk++)
                        {
                            // second loop on mass numbers
                            checkManager2 = theMasManager[ss * a_max + kk];

                            for(int rr = 0; rr < checkManager2->howManyGates(); rr++)
                            {
                                // in principle loop on all possible gates, in practice we expect only one!
                                checkGate2 = checkManager2->getBanGate(rr);

                                if(checkGate1->overlapBan(checkGate2))
                                {
                                    cout << " ==> Warning! Found overlap between bananas for Z = " << ss << ", A = " << ii << " and A = " << kk << endl;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
#endif // 0
    /////////////////////////////////////////////////
    ////////// Calibration for charge states
//     ////////////////////////////////////////////////
//     theChargeManager.clear();
//     theChargeManager.resize(z_max + 1);
//
//     for(int ii = z_min; ii <= z_max; ii++)
//     {
//         sprintf(endline, "%2.2d.cal", ii);
//         filename = chg_file_base + string(endline);
//
//         theChargeManager[ii] = new calManager(filename);
//     }

    ////////////////////////////////////////////////
    /// Calibration for A/Q
    /////////////////////////////////////////////////
    theCalManager = new calManager(cal_file);

    theCalManager->setGain(a_over_q_gain);
    theCalManager->setOffset(a_over_q_offs);

#ifdef ABERRATIONS___bad_algorithm
    ////////////////////////////////////////////////
    /// Calibration for aberrations
    /////////////////////////////////////////////////
    theXmcManager = new calManager(xmc_file);

    theXmcManager->setGain(xmc_gain);
    theXmcManager->setOffset(xmc_offs);

    theYmcManager = new calManager(ymc_file);

    theYmcManager->setGain(ymc_gain);
    theYmcManager->setOffset(ymc_offs);

    theXfpManager = new calManager(xfp_file);

    theXfpManager->setGain(xfp_gain);
    theXfpManager->setOffset(xfp_offs);

    if(xmc_uspl)
        theXmcSpliManager = new curveManager(xmc_file);

    if(ymc_uspl)
        theYmcSpliManager = new curveManager(ymc_file);

    if(xfp_uspl)
        theXfpSpliManager = new curveManager(xfp_file);
#endif

    a_over_q_ok = false;
}

//******************************************************************************
//******************************************************************************
//******************************************************************************
//******************************************************************************
#define Dcout  if(0)cout
//******************************************************************************
int TmassCalculator::deduce_mass_number(int zed, double energy, double radius, double betagamma, double x_fp)
{
    Dcout << "TmassCalculator::mass_number =============================================" << endl;

    if((zed < z_min) || (zed > z_max))
    {
        //cout << "Rejected because  TmassCalculator::mass_number     Zet id unrealistic " << zed << ", returning 0 " << endl;
        return 0;
    }
    else
    {
//         cout << "TmassCalculator::mass_number--> Zet  = " << zed  << endl;
    }
    mass_num = 0;

    double y_point_mass = calculate_mass_from_a_over_q(zed, energy, radius, betagamma);
    double x_point = (x_fp / mm);

    if(mass_ok && x_point > 0)
    {

        // there is an matrix A vs X_fp (for each Z value)
        // on which there are plenty of bananas representing different values of A
        // now we will scan over this A values (which is true)

//         for(int a = a_min, i = 0; a < a_max; a++, i++)  // 'a' bananas on the matrix[z][q],

        for( int i = 0; i < massA_bananas_vector_for_zed[zed].size() ;  i++)
        {
            //cout << "For A=" << ii << endl;
            //int index = zed * a_max + a;

            // check if it is inside of the gate
            // Where is this banana gate ??????????????? "mass number bananas"

            // for every zed value  there is  a separate charge state matrix
            // with its own bananas for charge state identification

            //we scan   a_min--a_max

            int a = massA_bananas_vector_for_zed[zed][i].second ;
            Dcout << "TmassCalculator::mass_number--> Zet id = " << zed
                  << " A = " << a
                  << endl;

//             if(massA_bananas_vector_for_zed.size() < zed + 1)
//             {
//                 cout << "too small vector Z" << endl;
//             }
//             if(massA_bananas_vector_for_zed[zed].size() < a + 1)
//             {
//                 cout << "too small vector ii , size is " << massA_bananas_vector_for_zed[zed].size() << endl;
//             }
//
//             if(massA_bananas_vector_for_zed[zed][i].first == NULL)
//             {
//                 cout << "empty polygon vector zz, ii" << endl;
//             }

//             cout << "Testing if point [" << x_point/1000 << "  "<< y_point_mass
//                  << "] is in banana - zed = " << zed << " i=" << i  << ", A = "
//                  << massA_bananas_vector_for_zed[zed][i].second << endl;
                 

//             if(massA_bananas_vector_for_zed[zed][i].second)
                    if(massA_bananas_vector_for_zed[zed][i].first->Test((x_point/ 1000), y_point_mass))
            {
                mass_num = massA_bananas_vector_for_zed[zed][i].second;
//                 if(a != mass_num)
//                 {
//               cout << "descrepency ? "
//               << massA_bananas_vector_for_zed[zed][i].second << endl;
//                 }
                    mass_num_ok = true;
                    q_reaction(zed);
                    //Dcout << "succesfully found mass_number = " << mass_num << endl;

                    return mass_num;

                }
        } // for ii (A)
    }
    else
    {
        Dcout << "mass is not OK or x_fp == 0,  so A can not be recognised" << endl;
    }// end if mass_ok
    Dcout << " is mass_num_ok ? = " << mass_num << endl;
    return mass_num;
}
//******************************************************************************
double TmassCalculator::calculate_mass_from_a_over_q(int zed, double energy, double radius, double betagamma)
{
    Dcout << "F. TmassCalculator::calculate_mass_from_a_over_q " << endl;

    //if(mass_ok) return mass; // if it was already calculated for this event




    if((zed < z_min) || (zed > z_max))
    {
        //cout << "rejected because z = " << zed << endl;
        return 0.;
    }
//     cout << "F. TmassCalculator::calculate_mass_from_a_over_q     OK  z = " << zed << endl;
    a_over_q_ok = false;

    //a_over_q = theCalManager->getCalValue( zed, zed,  a_over_q_uncal( zed, radius, betagamma ) );

    a_over_q =  a_over_q_offs  + (a_over_q_uncal(zed, radius, betagamma) * a_over_q_gain);
    a_over_q_ok = true;
    spec_aoq_cal->manually_increment(a_over_q);

//      if(ptr_prisma_manager->x_fp() > 0.01) cout << "still in    calculate_mass_from_a_over_q x_fp = "
//          << (ptr_prisma_manager->x_fp()) << endl;

    Dcout << "before calling deduce_charge_state " << endl;
    double charge = deduce_charge_state(zed, energy, radius, betagamma); // once more calculating?
    if(charge_ok)
    {
        int    chgnum = (int)floor(charge + 0.5);     // for proper rounding
        double x_plane = ptr_prisma_manager->x_fp();

        //         if(x_plane > 0.01)
        Dcout << "Incr macierzy z " << zed << " chgnum= " << chgnum
              << " x = " << x_plane
              << ", y= " << a_over_q << endl;

        spec_a_over_q_vs_xfp__for_z_for_q[zed][chgnum]->
        manually_increment(x_plane , a_over_q);



//         cout << "Before calibrating charge = " << charge << endl;
        if(true) // if(theChargeManager.size() > zed && theChargeManager[zed] != NULL)
        {
            // charge = theChargeManager[zed]->getCalValue(chgnum, chgnum, charge);
            mass = a_over_q * charge ;
            mass = mass/100.;
            mass_ok = true;
            Dcout << "calculate_mass_...  -- > mass = " << mass
                  << " because a / q = " << a_over_q
                  << " * charge = " << charge
                  << endl;
            if(mass >= 1)
            {
                spec_mass->manually_increment(mass);
                spec_mass__for_z[zed]->manually_increment(mass);
                spec_mass_vs_xfp__for_z[zed]->manually_increment(x_plane  , mass);
            }
            return mass;
        }
        else
            return 0;
    }
    return 0;

}
//******************************************************************************
double TmassCalculator::deduce_charge_state(int zed, double energy, double radius, double betagamma)
{
    Dcout << "TmassCalculator::deduce_charge_state " << endl;

    charge = 0.;
    charge_ok = false;


    if((zed < z_min) || (zed > z_max))
    {
        //cout << "Rejected -- TmassCalculator::deduce_charge_state bec z = " << zed << endl;
        return 0.;
    }
//     cout << "TmassCalculator::deduce_charge_state for  z = " << zed << endl;



    radius_multiplied_by_velocity = r_fact * r_beta(zed, radius, betagamma);
    energy_of_ion_chamber = e_fact * energy ;

//      cout
//             << " r_fact =" <<  r_fact
//             << " r_beta(zed, radius, betagamma) =" << r_beta(zed, radius, betagamma)
//             << " radius_multiplied_by_velocity=" << radius_multiplied_by_velocity
//             << endl;

    // matrix where we can recognize charge states
    spec_charge_states__for_any_zed->manually_increment(radius_multiplied_by_velocity, energy_of_ion_chamber);

    spec_radius_multiplied_by_velocity->manually_increment(radius_multiplied_by_velocity);

    // NOTE: for every zed there should be a separate charge state matrix
    // with its own bananas for charge state identification


    // checking all bananas


    // if it is inside one of them, then the charge is


// loop over all registered bananas form this matrix
    // for checking
    if(radius_multiplied_by_velocity || energy_of_ion_chamber) // for zeros does not make sense to check
    {
        // matrix where we can recognize charge states
        spec_charge_states__for_z[zed]->manually_increment(radius_multiplied_by_velocity, energy_of_ion_chamber);

//         for(int q = q_min, i = 0  ; q <= q_max ; q++, i++)

        for(int i = 0  ; i < charge_state_bananas_vector_for_zed[zed].size() ; i++)

        {
//             cout << "TmassCalculator::deduce_charge_state for  scaning q bananas q =" << q << endl;
//             cout << "charge_state_bananas_vector_for_zed[zed].size() " << charge_state_bananas_vector_for_zed[zed].size()
//                  << endl;

            //if(q >= charge_state_bananas_vector_for_zed[zed][q].size()) continue ;

            // check the point (x=energy_gained, y  dE_gained  - if it in the banana
//             cout << "Before testing banana for zed = " << zed << " q" << q << endl;
            if(charge_state_bananas_vector_for_zed[zed][i].first->Test(radius_multiplied_by_velocity, energy_of_ion_chamber))
            {
                charge = charge_state_bananas_vector_for_zed[zed][i].second;
                charge_ok = true;
                spec_deduced_charged_states->manually_increment(charge);
                Dcout << "Success --------------> Q = " << charge << "   (For zed = " << zed << ") " << endl;

                return charge;

            }
        } // end for nr
    }
    else
    {
        Dcout << "Cant deduce calculate Q because radius_multiplied_by_velocity = " << radius_multiplied_by_velocity
              << ", energy_of_ion_chamber=" << energy_of_ion_chamber << endl;
    }
    Dcout << "Q Not assigned " << endl;
    return charge;
}
//******************************************************************************


//***************************************************************************************
double TmassCalculator::q_reaction(int fragment_z)
{

    // z - z
    // a

    if(!mass_num_ok) return 0;

    const double light_c = 299792458.;
    TprismaMcp *ptr_mcp = ptr_prisma_manager->give_mcp_ptr();

    int beam_z =        ptr_mcp->get_beam_zed();
    int beam_mass_num = ptr_mcp->get_beam_mass_number();

    int target_z = ptr_mcp->get_target_zed();
    int target_mass_num = ptr_mcp->get_target_mass_number();

// Mass of fragment, beam and target taking into account default mass
    double mass_fragment = mass_ame(fragment_z ,mass_num);
    double mass_beam     = mass_ame(beam_z     ,beam_mass_num);
    double mass_target   = mass_ame(target_z   ,target_mass_num);

// Calculation of fragment energy
    double fragment_energy = Energy_k(mass_fragment);

// Beam energy given in calibration file
    double beam_energy = ptr_mcp->get_beam_energy();

// Angle of the fragment

    double theta = ptr_mcp->get_theta_lab();
    double phi = ptr_mcp->get_phi_lab();

// Total kinetic energy loss

    if(fragment_energy && theta)
    {
        total_kinetic_energy_loss = 1./(mass_beam+mass_target-mass_fragment) *
                                    ((mass_fragment-mass_target)*beam_energy
                                     +(mass_beam+mass_target)*fragment_energy
                                     - 2*sqrt(mass_beam*mass_fragment*beam_energy*fragment_energy)*cos(theta));
        total_kinetic_energy_loss_ok = true;
        spec_tkel__for_z_a[fragment_z][mass_num]->manually_increment(total_kinetic_energy_loss);
        spec_angle_used_q_reac__for_z_a[fragment_z][mass_num]->manually_increment(theta*RADIANS_TO_DEGREES);
        spec_energy_used_q_reac__for_z_a[fragment_z][mass_num]->manually_increment(fragment_energy);
        spec_Ek_vs_theta__for_z_a[fragment_z][mass_num]->manually_increment(theta*RADIANS_TO_DEGREES,fragment_energy);
    }
    else
    {
        total_kinetic_energy_loss=0;
        total_kinetic_energy_loss_ok = false;
    }


//   cout <<  "Qreac = " <<  total_kinetic_energy_loss << '\t' << theta << '\t' << cos(theta) << endl;




    //increment spectrum of tkel  for this combination of Z, A


//    cout << theta*180./3.1415 << endl;
    return total_kinetic_energy_loss;
}

//***************************************************************************************

double TmassCalculator::mass_ame(int z, int a)
{

    const double amu_proton = 938.2720 * MeV;
    const double amu_neutron = 939.5653 * MeV;

    int n = a - z ;

    double mass = z*amu_proton + n*amu_neutron - ame_table[z][a]/1000;
    return mass;

}

//***************************************************************************************

double TmassCalculator::Energy_k(double mass)
{

    double gamma = ptr_prisma_manager->gamma();

    double energy_k = mass*(gamma-1.);

    return energy_k;

}

//***************************************************************************************

double TmassCalculator::qreact_gs_to_gs(int z, int a)
{

    TprismaMcp *ptr_mcp = ptr_prisma_manager->give_mcp_ptr();

    int beam_z = ptr_mcp->get_beam_zed();
    int beam_a = ptr_mcp->get_beam_mass_number();

    int target_z = ptr_mcp->get_target_zed();
    int target_a = ptr_mcp->get_target_mass_number();

    int partner_z = beam_z + target_z - z;
    int partner_a = beam_a + target_a - a;

// Mass of fragment, beam and target taking into account default mass
    double mass_fragment = mass_ame(z          ,a);
    double mass_beam     = mass_ame(beam_z     ,beam_a);
    double mass_target   = mass_ame(target_z   ,target_a);
    double mass_partner  = mass_ame(partner_z  ,partner_a);

    double q0_value = (mass_fragment+mass_partner) - (mass_beam + mass_target) ;

    return q0_value;

}
//***************************************************************************************


#ifdef ABERRATIONS___bad_algorithm
double TmassCalculator::Mass(int zed, double energy, double radius, double betagamma, double x_mcp, double y_mcp, double x_fp)
{
    if((zed < z_min) || (zed > z_max))
        return 0.;

    mass = 0.;
    a_over_q_ok = false;

    // first, uncorrected A/q
    a_over_q = this->a_over_q_uncal(zed, radius, betagamma);
    a_over_q_ok = true;

    // apply correction for aberrations, first x_mcp, then y_mcp, finally x_fp
    a_over_q -= this->Correct_Xmc(zed, x_mcp / mm);
    a_over_q -= this->Correct_Ymc(zed, y_mcp / mm);
    a_over_q -= this->Correct_Xfp(zed, x_fp / mm);

    a_over_q = theCalManager->getCalValue(zed, zed, a_over_q);


    double charge = charge_state(zed, energy, radius, betagamma);
    int    chgnum = (int)floor(charge + 0.5);     // for proper rounding

    charge = theChargeManager[zed]->getCalValue(chgnum, chgnum, charge);

    mass = a_over_q * charge;

    return mass;
}
#endif
//******************************************************************************
#ifdef ABERRATIONS___bad_algorithm
int TmassCalculator::mass_number(int zed, double energy, double radius, double betagamma, double x_fp, double x_mcp, double y_mcp)
{
    if((zed < z_min) || (zed > z_max))
        return 0;

    mass_num = 0;

    for(int ii = a_min; ii < a_max; ii++)
    {
        int index = zed * a_max + ii;
        if(theMasManager[index]->inside(zed, x_fp / mm, this->Mass(zed, energy, radius, betagamma, x_mcp, y_mcp, x_fp)))
        {
            if(multi_bananas)
                mass_num += theMasManager[index]->getP_Id();
            else
                return theMasManager[index]->getP_Id();
        }
    }
    return mass_num;
}
#endif

//******************************************************************************

double TmassCalculator::a_over_q_uncal(int zed, double radius, double betagamma)
{
    //   if( (zed<z_min) || (zed>z_max) )
    //     return 0.;


    spec_aoq_uncal->manually_increment(inv_c_light * (B_dipole / tesla) * (radius / m) / (betagamma));
    return inv_c_light * (B_dipole / tesla) * (radius / m) / (betagamma);
}

//******************************************************************************

double TmassCalculator::r_beta(int zed, double radius, double betagamma)
{
    //   if( (zed<z_min) || (zed>z_max) )
    //     return 0.;

    const double c_light = 29979.2458; // in convenient units

    return c_light * (B_dipole / tesla) * (radius / m) * (betagamma);

}

//*******************************************************************************

void TmassCalculator::zeroing_variables()
{
    charge = 0;
    charge_ok = false; // validator

    mass = 0;
    mass_ok = false;

    mass_num = 0;
    mass_num_ok = false;

    a_over_q = 0;
    a_over_q_ok = false;
    calculations_already_done = false;

}

//*******************************************************************************

void TmassCalculator::analyse_current_event()
{
    int z = 0 ;
    if(flag_zed_from_dE_vs_E__otherwise_from_path)
    {
        z = ptr_prisma_manager->zed_deduced_from_EdE();
    }
    else // otherwise zed
    {
        z = ptr_prisma_manager->zed_deduced_from_pathE();
    }

    if(z)
    {
        deduce_mass_number(
            z,
            ptr_prisma_manager->ic_energy(),
            ptr_prisma_manager->radius(),
            ptr_prisma_manager->betagamma(),
            ptr_prisma_manager->x_fp()
        );
    }

    calculations_already_done = true;
}
//*****************************************************************************************
void TmassCalculator::make_A_bananas_on_the_matrices_A_vs_Xfp__for_z(int z)
{
    if(spec_mass_vs_xfp__for_z.size() <= z)
    {
        spec_mass__for_z.resize(z + 1);
        spec_mass_vs_xfp__for_z.resize(z + 1);
        massA_bananas_vector_for_zed.resize(z + 1);
    }

    if(massA_bananas_vector_for_zed.size() <= z)
    {
        massA_bananas_vector_for_zed.resize(z + 1);
    }


    // we also need spectra for recognition of A

    ostringstream s;
    s << name_of_this_element + "__mass_vs_xfp_" << "_for_Z_" << z ;
    string matix_name = s.str();

    if(spec_mass_vs_xfp__for_z[z] == NULL)
    {
        spec_mass_vs_xfp__for_z[z] =
            new spectrum_2D(matix_name, matix_name,
                            1000, 0, 1,
                            2000, 0, 200,
                            "TmassCalculator/",
                            "",
                            "X:  ...__x_fp"
                            "     Y:  " + name_of_this_element + "__mass..."
                            + "   (with condition on Zed)"
                           );  // incrementor name
        frs_spectra_ptr->push_back(spec_mass_vs_xfp__for_z[z]) ;
    }
    ostringstream name;
    name << name_of_this_element + "_mass__for_Z_" << z;
    if(spec_mass__for_z[z] == NULL)
    {
        spec_mass__for_z[z] =
            new spectrum_1D(name.str(),name.str(),
                            2000, 0, 200,
                            "TmassCalculator/",
                            "",
                            "Mass for Z " + z
                           );  // incrementor name
        frs_spectra_ptr->push_back(spec_mass__for_z[z]) ;
    }

    // loop for on mass vs xfp matrix for particular a
//     cout << z_start << "  " << z << "   "
// << a_start-(z_start-z)-n_rem_max << "   "
// << a_start-(z_start-z)+n_add_max << endl ;


    for(int a = a_start-(z_start-z)-n_rem_max, i = 0  ;
        a <= a_start-(z_start-z)+n_add_max
        ; a++, i++) // we produce Zet bananas only in chosen region
    {

        // cout << a << '\t';
        // at first make the polygons
        ostringstream s;
        s << matix_name  << "_polygon_A_" << a << ".poly" ;
        string name_polygon = s.str();

        TjurekPolyCond *polygon_ptr ;

        if(!read_banana(name_polygon, &polygon_ptr))
        {
            // if the polygon does not exitst yet
            cerr << "During Reading-in the parameters for TmassCalculator "
                 << name_of_this_element
                 << " it is impossible to read polygon gate: \n"
                 << name_polygon
                 //<< "  - Most probably it does not exist \n"
                 << "\n!!! The polygon is going to be automaticaly created for you - set its position properly"
                 << endl;

            // creating it ----------------------------------------
            ofstream plik(("polygons/" + name_polygon).c_str());
            if(!plik)         exit(1);

            int offset_x = 0;
            int offset_y = i * 3;

            double points[8] =
            {
                0.9,     15.00,         // x, y of the point nr 0     polygon: A_50
                0.1,     15.00,         // x, y of the point nr 1     polygon: A_50
                0.1,     16.00,
                0.9,     16.00
            };

            for(int n = 0 ; n < 8 ; n += 2)
                plik << points[n] + offset_x << "\t" << points[n + 1] + offset_y << "\n";

            plik.close();
            // once more we try something what really exit
            if(! read_banana(name_polygon, &polygon_ptr))
            {
                cout << "Could not create the polygon " << name_polygon << " for you " << endl;
                exit(125);
            }
        }
//         cout << endl;
        // here the polygon should be ready
//             if(z == 25) cout << name_polygon << " : \n" << polygon_ptr->printout_points()  << endl;

//          std::vector< pair<TjurekPolyCond*, int> >
//         cout << "before adding a banana to the vector - Z= " << z << ", A = " << a
//              << "\n    current size of massA_bananas_vector_for_zed is "
//              << massA_bananas_vector_for_zed.size()
//              << endl;
        massA_bananas_vector_for_zed[z].push_back(pair<TjurekPolyCond*, int>(polygon_ptr, a));

        // make the tkel spectra for this combination of Z, A
        if(spec_tkel__for_z_a.size() < z_max)
        {
            spec_tkel__for_z_a.resize(z_max+1);
            spec_energy_used_q_reac__for_z_a.resize(z_max+1);
            spec_angle_used_q_reac__for_z_a.resize(z_max+1);
            spec_Ek_vs_theta__for_z_a.resize(z_max+1);

        }
        if(spec_tkel__for_z_a[z].size() < a_max)
        {
            spec_tkel__for_z_a[z].resize(a_max+1);
            spec_energy_used_q_reac__for_z_a[z].resize(a_max+1);
            spec_angle_used_q_reac__for_z_a[z].resize(a_max+1);
            spec_Ek_vs_theta__for_z_a[z].resize(a_max+1);
        }

        ostringstream name;
        name << name_of_this_element + "_TKEL__for_Z_" << z << "_A_" << a ;

        if(spec_tkel__for_z_a[z][a] == NULL)
        {
            spec_tkel__for_z_a[z][a] =
                new spectrum_1D(name.str(),name.str(),
                                1200, -150, 150,
                                "TmassCalculator/",
                                "",
                                "TKEL " + z
                               );  // incrementor name
            frs_spectra_ptr->push_back(spec_tkel__for_z_a[z][a]) ;
        }

        name.str("");
        name << name_of_this_element + "_angle_used_TKEL__for_Z_" << z << "_A_" << a ;

        if(spec_angle_used_q_reac__for_z_a[z][a] == NULL)
        {
            spec_angle_used_q_reac__for_z_a[z][a] =
                new spectrum_1D(name.str(),name.str(),
                                720, 0, 180,
                                "TmassCalculator/",
                                "",
                                "Angle_TKEL " + z
                               );  // incrementor name
            frs_spectra_ptr->push_back(spec_angle_used_q_reac__for_z_a[z][a]) ;
        }


        name.str("");
        name << name_of_this_element + "_energy_used_TKEL__for_Z_" << z << "_A_" << a ;

        if(spec_energy_used_q_reac__for_z_a[z][a] == NULL)
        {
            spec_energy_used_q_reac__for_z_a[z][a] =
                new spectrum_1D(name.str(),name.str(),
                                2000, 0, 1000,
                                "TmassCalculator/",
                                "",
                                "Energy_TKEL " + z
                               );  // incrementor name
            frs_spectra_ptr->push_back(spec_energy_used_q_reac__for_z_a[z][a]) ;
        }

        name.str("");
        name << name_of_this_element + "_Ek_vs_theta_lab__for_Z_" << z << "_A_" << a;

        if(spec_Ek_vs_theta__for_z_a[z][a] == NULL)
        {
            spec_Ek_vs_theta__for_z_a[z][a] = new spectrum_2D(name.str(),name.str(),
                    360,0,180,
                    2000,0,1000,
                    "TmassCalculator/",
                    "",
                    string("As X: mcp__theta_radians_laboratory_when_valid    (its was here recalculated to degrees)") +
                    ",as Y: " + name_of_this_element + "__kinetic_energy_when_valid");
            frs_spectra_ptr->push_back(spec_Ek_vs_theta__for_z_a[z][a]);
        }


    } // for(int a

}
//******************************************************************************
#ifdef ABERRATIONS___bad_algorithm
double TmassCalculator::Correct_Xmc(int zed, double value)
{
    double retvalue;

    if(xmc_uspl)
        retvalue = theXmcSpliManager->interpolate(zed, value);
    else
        retvalue = theXmcManager->getCalValue(zed, zed, value);

    return retvalue;
}
//******************************************************************************
double TmassCalculator::Correct_Ymc(int zed, double value)
{
    double retvalue;

    if(ymc_uspl)
        retvalue = theYmcSpliManager->interpolate(zed, value);
    else
        retvalue = theYmcManager->getCalValue(zed, zed, value);

    return retvalue;
}
//******************************************************************************
double TmassCalculator::Correct_Xfp(int zed, double value)
{
    double retvalue;

    if(xfp_uspl)
        retvalue = theXfpSpliManager->interpolate(zed, value);
    else
        retvalue = theXfpManager->getCalValue(zed, zed, value);

    return retvalue;
}
#endif

#endif //   CURRENT_EXPERIMENT_TYPE == PRISMA_EXPERIMENT

