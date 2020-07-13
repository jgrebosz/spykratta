#include "TprismaSide.h"

#if CURRENT_EXPERIMENT_TYPE==PRISMA_EXPERIMENT
#include "Tnamed_pointer.h"
//*****************************************************************************************
TprismaSide::TprismaSide(string name, int ndet, int npar) : TprismaDetector(name)
{
    n_det = ndet;
    n_par = npar;

    /*theFile = new ConfigurationFile( conf_file.c_str() );
    if( read_configuration() > 0 )
      init_data();
    */
	     my_selfgate_type =  Tselfgate_type::cluster_addback; // fake, because..
    create_my_spectra();

	 
    named_pointer[name_of_this_element + "__flag_protest"]
    = Tnamed_pointer(&flag_protest, NULL, this) ;
}
//****************************************************************
void TprismaSide::create_my_spectra()
{
    spectrum_1D * spec_tmp = NULL;
    ostringstream s;
    string name;
    string folder = "IonisationChamber/";

    spec_side_de_raw.resize(n_det);

    for(int i = 0 ; i < n_det ; i++)
    {
        for(int k = 0 ; k < 4 ; k++)
        {
            s.str("");          // if(name.size() ) s.seekp(ios::beg);
            s << name_of_this_element + "__"
              //<< setfill('0') << setw(2)
              << i << "_" << char(k + 'A') << "_dE_raw" ;
            name = s.str();
            /*      cout << i << ", " << k << "   widmo [" << name << "]" << endl;*/

            spec_tmp  = new spectrum_1D(name, name,
                                        4095, 1, 4096,
                                        folder,
                                        "",
                                        "the only incrementor available is: "  + name_of_this_element +  "_flag_protest");  // incrementor name
            frs_spectra_ptr->push_back(spec_tmp) ;

            spec_side_de_raw[i].push_back(spec_tmp);

            //.........
        }// for k
    } // for i

    name =  name_of_this_element + "__multiplicity";
    spec_multiplicity  = new spectrum_1D(name, name,
                                         10, 0, 10,
                                         folder,
                                         "multiplicity of segments with energy > lower_th",
                                         "the only incrementor available is: " +  name_of_this_element +  "_flag_protest");  // incrementor name
    frs_spectra_ptr->push_back(spec_multiplicity) ;


}
//****************************************************************
void TprismaSide::make_preloop_action(std::ifstream &s)
{
    valid_conf = 0;
    //////////////////////////////////
    // definition of the parameters
    //////////////////////////////////
    // preliminary check
    if(n_par < 4)
    {
        cout << " Error! Only " << n_par << " parameters defined, but Side has normally more ..." << endl;
        exit(__LINE__);
    }

    ind_s[0] = (int) Tfile_helper::find_in_file(s,  name_of_this_element  + "_ind_s1");
    ind_s[1] = (int) Tfile_helper::find_in_file(s,  name_of_this_element  + "_ind_s2");
    ind_s[2] = (int) Tfile_helper::find_in_file(s,  name_of_this_element  + "_ind_s3");
    ind_s[3] = (int) Tfile_helper::find_in_file(s,  name_of_this_element  + "_ind_s4");

    // should check for inconsistencies
    for(int ii = 0; ii < 4; ii++)
    {
        if(ind_s[ii] > (n_par - 1))
        {
            ostringstream t;
            t << " Error with definition of parameter " << ind_s[ii] << "(" << ii << ")" ;
            Tfile_helper_exception m;
            m.message = t.str();
            throw m;

        }
        for(int jj = 0; jj < ii; jj++)
        {
            if(ind_s[ii] == ind_s[jj])
            {

                ostringstream t;
                t  << " Error! Duplicated parameters "  << ind_s[ii] << "(" << ii << "), " <<
                   ind_s[jj] << "(" << jj << ")";
                Tfile_helper_exception m;
                m.message = t.str();
                throw m;
            }
        }
    }
    // everything OK, can store in "real" params

    /////////////////////////////////////
    //// No banana or calibration files
    ////////////////////////////////////

    //////////////////////////////////
    //// Threshold for data validity
    //////////////////////////////////
    lower_th = Tfile_helper::find_in_file(s,  name_of_this_element  + "_lower_th"); // equivalent to conditions found in GSORT
    valid_conf = 1;

}
//****************************************************************
int TprismaSide::read_configuration()
{
    valid_conf = 0;
    //////////////////////////////////
    // definition of the parameters
    //////////////////////////////////
    // preliminary check
    if(n_par < 4)
    {
        cout << " Error! Only " << n_par << " parameters defined, but Side has normally more ..." << endl;
        return valid_conf;
    }

    ind_s[0] = theFile->get_int("ind_s1",  0);
    ind_s[1] = theFile->get_int("ind_s2",  1);
    ind_s[2] = theFile->get_int("ind_s3",  2);
    ind_s[3] = theFile->get_int("ind_s4",  3);

    // should check for inconsistencies
    for(int ii = 0; ii < 4; ii++)
    {
        if(ind_s[ii] > (n_par - 1))
        {
            cout << " Error with definition of parameter " << ind_s[ii] << "(" << ii << ")" << endl;
            return valid_conf;
        }
        for(int jj = 0; jj < ii; jj++)
        {
            if(ind_s[ii] == ind_s[jj])
            {
                cout << " Error! Duplicated parameters "  << ind_s[ii] << "(" << ii << "), " <<
                     ind_s[jj] << "(" << jj << ")" << endl;
            }
        }
    }
    // everything OK, can store in "real" params

    /////////////////////////////////////
    //// No banana or calibration files
    ////////////////////////////////////

    //////////////////////////////////
    //// Threshold for data validity
    //////////////////////////////////
    lower_th = theFile->get_double("lower_th", 4.);  // equivalent to conditions found in GSORT

    valid_conf = 1;
    return valid_conf;
}
//*****************************************************************************************
int TprismaSide::validate_event(std::vector<detData*>& theEvent)
{
    int nvalid = 0;
    detData* aData = 0;
    valid_evt = 0;

    err_code = 0;
    flag_protest = false;

    // if there are no "side" data, event is valid!
    if(theEvent.size() < 1)
    {
        processed_event = 1;
        valid_evt = 1;
        // multiplicity spectrum
        spec_multiplicity -> manually_increment(nvalid);
        return 1;
    }

    processed_event = 0;

    for(int ii = 0; ii < (int)theEvent.size(); ii++)
    {
        aData = theEvent[ii];
        for(int jj = 0; jj < 4; jj++)
        {
//             // spectra of energy
//             cout << "aData->num_det()" << aData->num_det() << " jj=" << jj
//             << " spec_side_de_raw.size() = "
//             << spec_side_de_raw.size() 
//             << " spec_side_de_raw[aData->num_det()].size() = "
//             << spec_side_de_raw[aData->num_det()].size()
//             << endl;
            
            spec_side_de_raw[aData->num_det() ][jj]->manually_increment((*aData)[ind_s[jj]]);

            if((*aData)[ind_s[jj]] >= lower_th)
                nvalid++;
        }
    }
    // multiplicity spectrum
    spec_multiplicity -> manually_increment(nvalid);

    if(nvalid > 0)
    {
        err_code = 2048;
        flag_protest = true;
        return -1;
    }
    else
    {
        processed_event = 1;
        valid_evt = 1;
        return 1;
    }
}
//*****************************************************************************************
#endif // CURRENT_EXPERIMENT_TYPE==PRISMA_EXPERIMENT
