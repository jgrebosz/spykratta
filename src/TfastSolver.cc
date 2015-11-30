#include "TfastSolver.h"

#include "Tnamed_pointer.h"
//**************************************************************************************
/////////////////////////////////////////////////////////////
/// Constructor 1: argument is same number needed by      ///
/// PRISMA_SOLVER in gsort, namely sqrt(B_q/B_d)          ///
/// B_dipole, B_quadrupole set to fake values             ///
/////////////////////////////////////////////////////////////
TfastSolver::TfastSolver(string myname,  double ratio): Tfrs_element(myname)
{
    theFile = NULL;
    initial_zeroing_of_pointers();
    //   theFile = new ConfigurationFile( "prisma.conf" );
    B_sqrt       = 1. / ratio;
    B_ratio      = B_sqrt * B_sqrt;
    B_quadrupole = 1.* tesla;
    B_dipole     = B_quadrupole * B_ratio;

    gets_ratio = true;
    initGeometry();
    create_my_spectra();
}
//**************************************************************************************
/////////////////////////////////////////////////////////////
/// Constructor 2: arguments are B_dipole, B_quadrupole   ///
/////////////////////////////////////////////////////////////
TfastSolver::TfastSolver(string myname,   double b1, double b2): Tfrs_element(myname)
{
    theFile = NULL;
    initial_zeroing_of_pointers();
    //   theFile = new ConfigurationFile( "prisma.conf" );
    B_dipole     = b1;
    B_quadrupole = b2;
    B_ratio      = B_quadrupole / B_dipole;
    B_sqrt       = sqrt(B_ratio);

    gets_ratio = true;
    //   initGeometry();
    create_my_spectra();
}
//**************************************************************************************
///////////////////////////////////////////////////////////////
/// Constructor 3: only configuration file, expects to find ///
/// ratio in the configuration file                         ///
/// Fake values are anyway set!                             ///
///////////////////////////////////////////////////////////////
TfastSolver::TfastSolver(string myname,   char* name): Tfrs_element(myname)
{
    theFile = NULL;
    initial_zeroing_of_pointers();
    B_dipole     = 1.* tesla;
    B_quadrupole = 1.* tesla;
    B_ratio      = B_quadrupole / B_dipole;
    B_sqrt       = sqrt(B_ratio);
    //   theFile = new ConfigurationFile( name );

    gets_ratio = false;
    //   initGeometry();
    create_my_spectra();
}
//**************************************************************************************
////////////////////////////////////////////////////////////////
/// Constructor 4: configuration file, but ratio is provided ///
////////////////////////////////////////////////////////////////
TfastSolver::TfastSolver(string myname,   char* name, double ratio): Tfrs_element(myname)
{
    theFile = NULL;
    initial_zeroing_of_pointers();
    //   theFile = new ConfigurationFile( name );
    gets_ratio = true;

    B_sqrt       = 1. / ratio;
    B_ratio      = B_sqrt * B_sqrt;
    B_quadrupole = 1.* tesla;
    B_dipole     = B_quadrupole * B_ratio;

    //   initGeometry();
    create_my_spectra();
}
//**************************************************************************************
///////////////////////////////////////////////////////////////
/// Constructor 5: only configuration file, expects to find ///
/// ratio in the configuration file                         ///
/// Fake values are anyway set!                             ///
///////////////////////////////////////////////////////////////
TfastSolver::TfastSolver(string myname,   string name): Tfrs_element(myname)
{
    theFile = NULL;
    initial_zeroing_of_pointers();
    B_dipole     = 1.* tesla;
    B_quadrupole = 1.* tesla;
    B_ratio      = B_dipole / B_quadrupole;
    B_sqrt       = sqrt(B_ratio);
    //   theFile = new ConfigurationFile( name.c_str() );

    gets_ratio = false;

    //   initGeometry();
	  	 	 // if the selfgate is diffenet than not_available
   my_selfgate_type =  Tselfgate_type::Types::hector;;       // fake, because...
    create_my_spectra();
}
//**************************************************************************************
////////////////////////////////////////////////////////////////
/// Constructor 6: configuration file, but ratio is provided ///
////////////////////////////////////////////////////////////////
TfastSolver::TfastSolver(string myname,   string name, double ratio): Tfrs_element(myname)
{
    theFile = NULL;
    initial_zeroing_of_pointers();
    //   theFile = new ConfigurationFile( name.c_str() );

    gets_ratio = true;

    B_sqrt       = 1. / ratio;
    B_ratio      = B_sqrt * B_sqrt;
    B_quadrupole = 1.* tesla;
    B_dipole     = B_quadrupole * B_ratio;

    //   initGeometry();
	
    create_my_spectra();
}
//**************************************************************************************
////////////////////////////////////////////////////////////////
/// Constructor 7: expects to find everything in the default ///
/// configuration file                                       ///
/// Fake values are anyway set!                              ///
////////////////////////////////////////////////////////////////
TfastSolver::TfastSolver(string myname): Tfrs_element(myname)
{
    theFile = NULL;
    initial_zeroing_of_pointers();
    //   theFile = new ConfigurationFile( "prisma.conf" );
    B_dipole     = 1.* tesla;
    B_quadrupole = 1.* tesla;
    B_ratio      = B_dipole / B_quadrupole;
    B_sqrt       = sqrt(B_ratio);

    gets_ratio = false;

    //   initGeometry();
    create_my_spectra();
}
//**************************************************************************************
TfastSolver::~TfastSolver()
{
    delete theFile;

    free_reservations();

}
//**************************************************************************************
void TfastSolver::free_reservations()
{
    delete   [] ic_q ;            //   = new double[nSec];
    delete    []  ic_zero;        // = new Vector3D[nSec];
    delete    []  x_min;          //   = new Vector3D[nSec];
    delete    []  x_max;          //   = new Vector3D[nSec];

    delete   []    chamber_data;  // = new double[15*det_len];

    delete   []   dE;         // = new double[nSec-1];

    delete   []  firstid ;            //= new int[nSec-1];
    delete   []  lastid;          //  = new int[nSec-1];
    delete   []  index1;          //  = new int[nSec];
    delete   []  index2;          //  = new int[nSec];

    delete   [] centre_section ;      // = new double[det_len];
}
//*************************************************************************************
/**
 * Some memory is reserved every preLoop. To avoid error in first preLoop, the following pointers
 * are initially zeroed
 */
void TfastSolver::initial_zeroing_of_pointers()
{
    ic_zero  =
        x_min  =
            x_max  = NULL;
    ic_q   =
        chamber_data  =
            centre_section   =
                dE  = NULL;
    firstid   =
        lastid  =
            index1  =
                index2  = 0;

}
//**************************************************************************************
void TfastSolver::create_my_spectra()
{
      my_selfgate_type =  Tselfgate_type::Types::hector;       // fake, because...
      
    // so many constructors, so we mae incrementers here
    named_pointer[name_of_this_element + "__trajectory_length_when_solved"]
    = Tnamed_pointer(&traj_length, &solved_ev, this) ;

    // so many constructors, so we mae incrementers here
    named_pointer[name_of_this_element + "__curvature_radius_when_solved"]
    = Tnamed_pointer(&curv_radius, &solved_ev, this) ;


    string folder = "FastSolver/";
    //-------------------------
    string name = name_of_this_element + "__length";
//   cout << "name = [" << name << "]" << endl;
    spec_length  = new spectrum_1D(name, name,
                                   2000, 5, 7,
                                   folder,
                                   "",
                                   name_of_this_element + "__trajectory_length_when_solved"); // incrementor name
    frs_spectra_ptr->push_back(spec_length) ;
    //--------------------


    name = name_of_this_element + "__curvature_radius";
//   cout << "name = [" << name << "]" << endl;
    spec_radius  = new spectrum_1D(name, name,
                                   1500, 0.5, 2,
                                   folder,
                                   "in meters",
                                   name_of_this_element + "__curvature_radius_when_solved"); // incrementor name
    frs_spectra_ptr->push_back(spec_radius) ;

}
//**************************************************************************************
void TfastSolver::make_preloop_action(std::ifstream & s)
{

    // read almost everything from file!
    target_mcp_distance = Tfile_helper::find_in_file(s,  name_of_this_element  + "_target_mcp_distance") * mm;
    angle_mcp           = Tfile_helper::find_in_file(s,  name_of_this_element  + "_angle_mcp") * degree;

    target_quad_distance = Tfile_helper::find_in_file(s,  name_of_this_element  + "_target_quad_distance") * mm;
    quad_radius = Tfile_helper::find_in_file(s,  name_of_this_element  + "_quad_radius") * mm;
    sqrt_radius = sqrt(quad_radius);
    quad_length = Tfile_helper::find_in_file(s,  name_of_this_element  + "_quad_length") * mm;
    // geometrical radius: store the square of it for convenience
    geom_radius = Tfile_helper::find_in_file(s,  name_of_this_element  + "_geom_radius") * mm;
    geom_radius = geom_radius * geom_radius;

    target_dipole_distance = Tfile_helper::find_in_file(s,  name_of_this_element  + "_target_dipole_distance") * mm;
    dip_in_m = tan(20.*degree);
    dip_in_q = target_dipole_distance;
    dipole_height = Tfile_helper::find_in_file(s,  name_of_this_element  + "_dipole_height") * mm;

    dip_out_m = tan(125.*degree);
    dip_out_q = (160. + 60. * (sqrt(3.) - dip_out_m)) * cm;

    fp_m = -sqrt(3.);
    double dip_fp_dist = Tfile_helper::find_in_file(s,  name_of_this_element  + "_dip_fp_dist") * mm;
    double xf = 60.*cm + dip_fp_dist * cos(30.*degree);
    double yf = dip_in_q + 60.*sqrt(3.) * cm + dip_fp_dist * sin(30.*degree);
    fp_q = yf + sqrt(3.) * xf;

    double fp_half_length = Tfile_helper::find_in_file(s,  name_of_this_element  + "_fp_half_length") * mm;
    fp_zero = Vector3D(xf - sin(30.*degree) * fp_half_length, yf + cos(30.*degree) * fp_half_length, 0.);

    guess_radius = Tfile_helper::find_in_file(s,  name_of_this_element  + "_guess_radius") * mm;
    tolerance = Tfile_helper::find_in_file(s,  name_of_this_element  + "_tolerance") * mm;

    fp_dispersion = getFocalPlaneDispersion();

    // in case magnetic fields (or ratio) were not provided,
    // should decode them from file!
    double b_dip =0.; 
    double b_quad = 0.;
    //double b_rat =0.;
    if(!gets_ratio)
    {
        b_dip  = Tfile_helper::find_in_file(s,  name_of_this_element  + "_prisma_dipole");
        b_quad = Tfile_helper::find_in_file(s,  name_of_this_element  + "_prisma_quadrupole");
	
	b_dip = ConvertADC_Tesla_Dipole(b_dip);
	b_quad = ConvertADC_Tesla_Quadrupole(b_quad);

        // ratio not found, fields found???
        if((b_dip <= 0.) || (b_quad <= 0.))
        {
            // one of the fields not found, send warning
            cout << " Warning! Missing information, fields will be set to default values." << endl;
         }
         else
         {
         // both fields found
            B_dipole     = b_dip;
            B_quadrupole = b_quad;
            B_ratio      = B_dipole / B_quadrupole;
            B_sqrt       = sqrt(B_ratio);
         }
    }
    cout << " Dipole field is " << B_dipole / tesla << " T" << endl;
    cout << " Quadrupole field is " << B_quadrupole / tesla << " T" << endl;
    cout << " Field ratio sqrt(B_D/B_Q) is " << B_sqrt << endl;

    // to speed up calculations, we store quad_length/(sqrt_radius*B_sqrt)
    // this determines the motion in the quadrupole!
    coef_k = sqrt_radius * B_sqrt;
    k_coef = quad_length / coef_k;

    //Ionization chamber;
    ic_fp_distance = Tfile_helper::find_in_file(s,  name_of_this_element  + "_ic_fp_distance") * mm;
    ic_depth       = Tfile_helper::find_in_file(s,  name_of_this_element  + "_ic_depth") * mm;
    ic_width       = Tfile_helper::find_in_file(s,  name_of_this_element  + "_ic_width") * mm;

    ic_threshold      = (int) Tfile_helper::find_in_file(s,  name_of_this_element  + "_ic_threshold");
    ic_up_thresh      = (int) Tfile_helper::find_in_file(s,  name_of_this_element  + "_ic_up_thresh");
    angular_tolerance = Tfile_helper::find_in_file(s,  name_of_this_element  + "_angular_tolerance") * degree;

    nSec           = (int) Tfile_helper::find_in_file(s,  name_of_this_element  + "_ic_sections") + 1;

    free_reservations();
    ic_q    = new double[nSec];
    ic_zero = new Vector3D[nSec];
    x_min   = new Vector3D[nSec];
    x_max   = new Vector3D[nSec];

    // 30/01/08 with new data format, only four numbers!!!
    det_len = nSec - 1;

    // "extreme left" point of each section
    for(int ii = 0; ii < nSec; ii++)
    {
        ic_zero[ii] = fp_zero + (ic_fp_distance + ii * ic_depth) * Vector3D(cos(30.*degree), sin(30.*degree), 0.);
        ic_q[ii]    = ic_zero[ii].Y() - fp_m * ic_zero[ii].X();
    }

    // 30/01/08 changed nSec --> det_len
    centre_section = new double[det_len];
    for(int ii = 0; ii < nSec - 1; ii++)
    {
        centre_section[ii] = ic_fp_distance + (ii + 0.5) * ic_depth;
    }

    // "safe" allocation
    // 30/01/08 changed nSec --> det_len
    chamber_data = new double[15 * det_len];

    dE = new double[nSec - 1];

    firstid = new int[nSec - 1];
    lastid  = new int[nSec - 1];
    index1  = new int[nSec];
    index2  = new int[nSec];

}
//**************************************************************************************
double TfastSolver::ConvertADC_Tesla_Dipole(double adc_dipole)
{
    const unsigned int nb_coeff = 4;
    const double coeff[nb_coeff] = {0.00433272,  1.05918e-06, 1.79795e-13, -1.77315e-19};
    double dipole_field = 0;
    for(unsigned int k = 0 ; k< nb_coeff ; k++)
        dipole_field += coeff[k]*pow(adc_dipole,k);
	
 //   cout << "Dipole Field in Tesla = " << dipole_field;
    
    return dipole_field;
}

//**************************************************************************************
double TfastSolver::ConvertADC_Tesla_Quadrupole(double adc_quadrupole)
{
    const unsigned int nb_coeff = 4;
    const double coeff[nb_coeff] = {-0.000135434,  1.14131e-06, 2.69985e-14, -3.32439e-20}  ;
    double quadrupole_field = 0;
    for(unsigned int k = 0 ; k< nb_coeff ; k++)
        quadrupole_field += coeff[k]*pow(adc_quadrupole,k);
	
//    cout << "Quadrupole Field in Tesla = " << quadrupole_field;
    
    return quadrupole_field;
}

//**************************************************************************************
void TfastSolver::initGeometry()
{
#if 0
    //   create_my_spectra();

    // read almost everything from file!
    target_mcp_distance = theFile->get_double("target_mcp_distance", 250.) * mm;
    angle_mcp           = theFile->get_double("angle_mcp", 135.) * degree;

    target_quad_distance = theFile->get_double("target_quad_distance", 500.) * mm;
    quad_radius = theFile->get_double("quad_radius", 160.) * mm;
    sqrt_radius = sqrt(quad_radius);
    quad_length = theFile->get_double("quad_length", 600.) * mm;
    // geometrical radius: store the square of it for convenience
    geom_radius = theFile->get_double("geom_radius", 150.) * mm;
    geom_radius = geom_radius * geom_radius;

    target_dipole_distance = theFile->get_double("target_dipole_distance", 1600.) * mm;
    dip_in_m = tan(20.*degree);
    dip_in_q = target_dipole_distance;
    dipole_height = theFile->get_double("dipole_height", 200.) * mm;

    dip_out_m = tan(125.*degree);
    dip_out_q = (160. + 60. * (sqrt(3.) - dip_out_m)) * cm;

    fp_m = -sqrt(3.);
    double dip_fp_dist = theFile->get_double("dip_fp_dist", 3285.) * mm;
    double xf = 60.*cm + dip_fp_dist * cos(30.*degree);
    double yf = dip_in_q + 60.*sqrt(3.) * cm + dip_fp_dist * sin(30.*degree);
    fp_q = yf + sqrt(3.) * xf;

    double fp_half_length = theFile->get_double("fp_half_length", 500.) * mm;
    fp_zero = Vector3D(xf - sin(30.*degree) * fp_half_length, yf + cos(30.*degree) * fp_half_length, 0.);

    guess_radius = theFile->get_double("guess_radius", 1200.) * mm;
    tolerance = theFile->get_double("tolerance", 1.) * mm;

    fp_dispersion = getFocalPlaneDispersion();

    // in case magnetic fields (or ratio) were not provided,
    // should decode them from file!
    double b_quad =0.;
    double b_rat = 0.;
    if(!gets_ratio)
    {
        b_dip  = theFile->get_double("prisma_dipole", -1.0) * tesla;
        b_quad = theFile->get_double("prisma_quadrupole", -1.0) * tesla;
      //  b_rat  = theFile->get_double("field_ratio", -1.0);

        if(b_rat <= 0.)
        {
            // ratio not found, fields found???
            if((b_dip < 0.) || (b_quad < 0.))
            {
                // one of the fields not found, send warning
                cout << " Warning! Missing information, fields will be set to default values." << endl;
            }
            else
            {
                // both fields found
                B_dipole     = b_dip;
                B_quadrupole = b_quad;
                B_ratio      = B_dipole / B_quadrupole;
                B_sqrt       = sqrt(B_ratio);
            }
        }
/*        else
        {
            // ratio found, uses fake fields
            // b_rat = sqrt(B_Q/B_D) ==> B_sqrt = sqrt(B_D/B_Q)
            B_sqrt       = 1. / b_rat;
            B_ratio      = B_sqrt * B_sqrt;
            B_quadrupole = 1.* tesla;
            B_dipole     = B_quadrupole * B_ratio;
            B_dipole     = 1.* tesla;
        }
*/
    }
    cout << " Dipole field is " << B_dipole / tesla << " T" << endl;
    cout << " Quadrupole field is " << B_quadrupole / tesla << " T" << endl;
    cout << " Field ratio sqrt(B_D/B_Q) is " << B_sqrt << endl;

    // to speed up calculations, we store quad_length/(sqrt_radius*B_sqrt)
    // this determines the motion in the quadrupole!
    coef_k = sqrt_radius * B_sqrt;
    k_coef = quad_length / coef_k;

    //Ionization chamber;
    ic_fp_distance = theFile->get_double("ic_fp_distance", 720.0) * mm;
    ic_depth       = theFile->get_double("ic_depth", 250.0) * mm;
    ic_width       = theFile->get_double("ic_width", 100.0) * mm;

    ic_threshold      = theFile->get_int("ic_threshold", 5);
    ic_up_thresh      = theFile->get_int("ic_up_thresh", 3500);
    angular_tolerance = theFile->get_double("angular_tolerance", 1.3) * degree;

    nSec           = theFile->get_int("ic_sections", 4) + 1;

    ic_q    = new double[nSec];
    ic_zero = new Vector3D[nSec];
    x_min   = new Vector3D[nSec];
    x_max   = new Vector3D[nSec];

    // 30/01/08 with new data format, only four numbers!!!
    det_len = nSec - 1;

    // "extreme left" point of each section
    for(int ii = 0; ii < nSec; ii++)
    {
        ic_zero[ii] = fp_zero + (ic_fp_distance + ii * ic_depth) * Vector3D(cos(30.*degree), sin(30.*degree), 0.);
        ic_q[ii]    = ic_zero[ii].Y() - fp_m * ic_zero[ii].X();
    }

    // 30/01/08 changed nSec --> det_len
    centre_section = new double[det_len];
    for(int ii = 0; ii < nSec - 1; ii++)
    {
        centre_section[ii] = ic_fp_distance + (ii + 0.5) * ic_depth;
    }

    // "safe" allocation
    // 30/01/08 changed nSec --> det_len
    chamber_data = new double[15 * det_len];

    dE = new double[nSec - 1];

    firstid = new int[nSec - 1];
    lastid  = new int[nSec - 1];
    index1  = new int[nSec];
    index2  = new int[nSec];
#endif
}
//**********************************************************************
// Draws straight line from target to quadrupole entrance using
// start detector coordinates (passed with units!)
bool TfastSolver::getQuadrupoleEntrance(double x, double y)
{
    x_mcp = x;
    y_mcp = y;

    position_mcp = Vector3D(x * sin(angle_mcp), target_mcp_distance + x * cos(angle_mcp), y);

    // store angles for Doppler correction
    Vector3D claraPosition;
    claraPosition = Vector3D(position_mcp.Z(), -position_mcp.X(), position_mcp.Y());
    th_mcp = claraPosition.theta();
    ph_mcp = claraPosition.phi();
    if(ph_mcp < 0.) ph_mcp += 360.*degree;

    pos_quad_in = (target_quad_distance / position_mcp.Y()) * position_mcp;

    // should not continue if quadrupole is not entered!!!
    if((pos_quad_in.X()*pos_quad_in.X() + pos_quad_in.Z()*pos_quad_in.Z()) > geom_radius)
    {
        cout << "Imposible to make trajectory for this event because\n"
        "should not continue if quadrupole is not entered!!!"
        "\nSomething is wrong with geometry parameters introduced in your 'prisma_calibration.txt' file.\nPlease correct it."
        << endl;
        return false;
    }
    vel_quad_in = pos_quad_in / abs(pos_quad_in);

    // coefficients for quadrupole
    tg_alpha = vel_quad_in.X() / vel_quad_in.Y();
    tg_beta  = vel_quad_in.Z() / vel_quad_in.Y();
    coefB    = pos_quad_in.X();
    coefD    = pos_quad_in.Z();

    //    cout << "Success  the quadrupole is  entered OK " << endl;
    return true;
}
//**************************************************************************************
// Approximate solution of the equation of motion
// (see thesis, A.Meroni)
bool TfastSolver::getQuadrupoleExit()
{
    double curvsqrt = sqrt(curv_radius);
    double coeff = curvsqrt * coef_k;
    coefA  = tg_alpha * coeff;
    coefC  = tg_beta  * coeff;

    //coef_k = quad_length / ( sqrt_radius * B_sqrt ); (init_geometry)
    double argument = k_coef / sqrt(curv_radius);
    double sinhx = sinh(argument);
    double coshx = cosh(argument);
    double sinx  = sin(argument);
    double cosx  = cos(argument);

    double x_exit = coefA * sinhx + coefB * coshx;
    double z_exit = coefC * sinx  + coefD * cosx;

    pos_quad_out = Vector3D(x_exit, target_quad_distance + quad_length, z_exit);

    double vel_x = tg_alpha * coshx + coefB * sinhx / coeff;
    double vel_z = tg_beta  * cosx  + coefD * sinx  / coeff;

    vel_quad_out = Vector3D(vel_x, 1., vel_z);
    vel_quad_out /= abs(vel_quad_out);

    return true;
}
//**************************************************************************************
// Draws straight line from quadrupole exit to dipole entrance
bool TfastSolver::getDipoleEntrance()
{
    double y_entrance;
    double x_entrance;
    if(vel_quad_out.X() == 0.)
    {
        y_entrance = dip_in_q;
        x_entrance = 0.;
    }
    else
    {
        double coef_ang = vel_quad_out.Y() / vel_quad_out.X();
        double intercept = pos_quad_out.Y() - coef_ang * pos_quad_out.X();

        x_entrance = (intercept - dip_in_q) / (dip_in_m - coef_ang);
        y_entrance = (dip_in_m * intercept - coef_ang * dip_in_q) / (dip_in_m - coef_ang);
    }

    double z_entrance;
    if(vel_quad_out.Z() == 0.)
        z_entrance = pos_quad_out.Z();
    else
        z_entrance = pos_quad_out.Z() + (vel_quad_out.Z() / vel_quad_out.Y()) * (y_entrance - pos_quad_out.Y());

    pos_dip_in = Vector3D(x_entrance, y_entrance, z_entrance);
    vel_dip_in = vel_quad_out;

    return true;
}
//**************************************************************************************
// First finds centre of the curvilineal trajectory (orthogonal to the entrance direction
// at a distance equal to the curvature radius), then finds the exit point by solving the
// second degree equation. Finally gets the exit velocity (orthogonal to the line centre-
// exit point.
bool TfastSolver::getDipoleExit()
{
    // ignores z coordinate
    p_dip_in = Vector3D(pos_dip_in.X(), pos_dip_in.Y(), 0.);

    // centre of the circular trajectory
    traj_centre = p_dip_in + curv_radius * Vector3D(vel_dip_in.Y(), -vel_dip_in.X(), 0.) / sqrt(vel_dip_in.X() * vel_dip_in.X() + vel_dip_in.Y() * vel_dip_in.Y());

    // coefficients to solve the intersection line-circle
    double a0 = dip_out_q - traj_centre.Y();
    double a1 = 1 + dip_out_m * dip_out_m;
    double a2 = traj_centre.X() - dip_out_m * a0;
    double a3 = traj_centre.X() * traj_centre.X() + a0 * a0 - curv_radius * curv_radius;

    // there must be two distinct real solutions!
    double delta = a2 * a2 - a1 * a3;

    if(delta <= 0.) return false;

    double x1 = (a2 - sqrt(delta)) / a1;
    double y1 = dip_out_m * x1 + dip_out_q;

    pos_dip_out = Vector3D(x1, y1, 0.);
    double coefang = -1.* (x1 - traj_centre.X()) / (y1 - traj_centre.Y());
    vel_dip_out = Vector3D(1., coefang, 0.) / sqrt(1. + coefang * coefang);

    return true;
}
//**************************************************************************************
// Draws straight line from dipole exit to focal plane
bool TfastSolver::getFocalPlanePosition()
{
    // finds equation of line describing trajectory of the particle
    c_ang  = vel_dip_out.Y() / vel_dip_out.X();
    interc = pos_dip_out.Y() - c_ang * pos_dip_out.X();

    // solves the equation
    double xf = (interc - fp_q) / (fp_m - c_ang);
    double yf = (fp_m * interc - c_ang * fp_q) / (fp_m - c_ang);

    fp_position = Vector3D(xf, yf, 0.);

    fp_coordinate = abs(fp_position - fp_zero);
    if(xf < fp_zero.X())
        fp_coordinate = - fp_coordinate;

    return true;
}
//**************************************************************************************
// computes the dispersion in the focal plane corresponding to a variation in the curvature radius (for the central trajectory)
double TfastSolver::getFocalPlaneDispersion()
{
    // assume particles entering the dipole along the optical axis
    pos_dip_in = Vector3D(0., dip_in_q, 0.);
    vel_dip_in = Vector3D(0., 1., 0.);

    var_radius = 10.*cm;

    // First assume curv_radius = guess_value - 10 cm
    curv_radius = guess_radius - var_radius;
    if(!getDipoleExit())
    {
        cerr << " --> Problem in calculating the focal plane dispersion (dipole)!" << endl;
        return -1.;
    }
    if(!getFocalPlanePosition())
    {
        cerr << " --> Problem in calculating the focal plane dispersion (focal plane)!" << endl;
        return -1.;
    }
    Vector3D pos1 = fp_position;

    // Then assume curv_radius = guess_value + 10 cm
    curv_radius = guess_radius + var_radius;
    if(!getDipoleExit())
    {
        cerr << " --> Problem in calculating the focal plane dispersion (dipole)!" << endl;
        return -1.;
    }
    if(!getFocalPlanePosition())
    {
        cerr << " --> Problem in calculating the focal plane dispersion (focal plane)!" << endl;
        return -1.;
    }
    Vector3D pos2 = fp_position;

    // Finally obtains the required value!
    return (2.*var_radius) / abs(pos1 - pos2);

}
//**************************************************************************************
// calculates curvature radius starting from starting and final point
bool TfastSolver::findTrajectory(double xe, double ye, double xf)
{
    solved_ev = 0;
    if(!getQuadrupoleEntrance(xe, ye))
        return false;

    int iterations = 0;
    int max_iterations = 100;

    curv_radius = guess_radius;

    while(1)
    {
        if(++iterations > max_iterations) return false;
        getQuadrupoleExit();
        getDipoleEntrance();
        if(!getDipoleExit()) return false;
        getFocalPlanePosition();

        // converged?
        double difference = fp_coordinate - xf;
        if(fabs(difference) < tolerance)
        {
            traj_length  = getTrajLength();
            solved_ev = 1;
            spec_length->manually_increment(traj_length);
            spec_radius->manually_increment(curv_radius);
            return true;
        }
        // NB: negative difference --> should decrease the radius
        // NB: positive difference --> should increase the radius

        curv_radius += difference * fp_dispersion;
    }
    cout << "here we are never! " << endl;
    return true;
}
//**************************************************************************************
// calculates curvature radius starting from starting angles and final point
bool TfastSolver::findATrajectory(double theta, double phi, double xf)
{
    th_mcp = theta;
    ph_mcp = phi;

    // calculate x_mcp, y_mcp and use findTrajectory
    double y, xm, ym;
    // trivial when trajectory is along the vertical plane x=0
    if(((theta * phi) == 0.) || (phi == 180.*degree))
    {
        xm = 0.;
        ym = target_mcp_distance * cos(phi) * tan(theta);
    }
    // otherwise find the intersection of two lines
    else
    {
        y  = tan(theta) * sin(phi) * target_mcp_distance / (1. + tan(angle_mcp) * tan(theta) * sin(phi));
        xm = y / cos(angle_mcp);
        ym = -sin(angle_mcp) * xm / tan(phi);
    }
    return findTrajectory(xm, ym, xf);
}
//**************************************************************************************
// From dipole entrance to focal plane vertical coordinate is neglected.
// Straight lines everywhere but the dipole!
double TfastSolver::getTrajLength()
{
    double length = 0.;

    length += abs(pos_quad_in - position_mcp);
    length += abs(pos_dip_in - pos_quad_out);
    length += abs(fp_position - pos_dip_out);

    double costheta = 0.;
    costheta = vel_dip_in.X() * vel_dip_out.X() + vel_dip_in.Y() * vel_dip_out.Y();
    costheta /= abs(Vector3D(vel_dip_in.X(), vel_dip_in.Y(), 0.));
    costheta /= abs(Vector3D(vel_dip_out.X(), vel_dip_out.Y(), 0.));
    double theta = acos(costheta);
    length += curv_radius * theta;

    length += abs(pos_quad_out - pos_quad_in);

    return length;
}
//**************************************************************************************
#define MIN(a,b)      (((a)<(b))?(a):(b))
#define MAX(a,b)      (((a)>(b))?(a):(b))

////////////////////////////////////////////////////////////////////////////
/// Treatment of the Ionization Chamber:
/// it is assumed that data are stored in an array of doubles of size
/// nSectors*nSections, where nSections is the number of sections
/// (depth), nSectors is the number of sectors (horizontal) having
/// at least one section firing. In practice, nSections = 4.
///
/// Changed 30/01/08: new data format to avoid using random numbers
/// Sector index is stored in an array of integers.
///////////////////////////////////////////////////////////////////////////
//#if 0
bool TfastSolver::FindChamber(int n_det, int* origIndex, double* origData)
{
    ic_energy = 0.;
    ic_a_de   = 0.;
    ic_ab_de  = 0.;
    ic_path   = 0.;
    ic_range  = 0.;

    if(!GoodSections(n_det, origIndex, origData))
        return false;
    if(!processChamber(n_det))
        return false;

    solved_ch = 1;

    return true;
}
//**************************************************************************************
bool TfastSolver::GoodSections(int n_det, int* origIndex, double* origData)
{
    // copy data into buffer for further manipulation
    memcpy(chamber_data, origData, n_det * det_len * sizeof(double));

    // stores indexes
    int* detnumbers = new int[n_det];
    memcpy(detnumbers, origIndex, n_det * sizeof(int));

    // calculates which sections should be crossed by the present (reconstructed)
    // particle trajectory (including angular tolerance)
    double theta = atan(c_ang);
    if(theta < 0.)
        theta += 180.*degree;

    double theta1 = theta + angular_tolerance;
    double c_a1   = tan(theta1);
    double int1   = pos_dip_out.Y() - c_a1 * pos_dip_out.X();

    double theta2 = theta - angular_tolerance;
    double c_a2   = tan(theta2);
    double int2   = pos_dip_out.Y() - c_a2 * pos_dip_out.X();

    double xf;
    double yf;

    for(int ii = 0; ii < nSec; ii++)
    {
        // intersection 1: + tolerance
        xf = (int1 - ic_q[ii]) / (fp_m - c_a1);
        yf = (fp_m * int1 - c_a1 * ic_q[ii]) / (fp_m - c_a1);
        index1[ii] = (int)floor(abs(Vector3D(xf, yf, 0.) - ic_zero[ii]) / ic_width);
        if(index1[ii] < 0) index1[ii] = -1;

        // intersection 2: - tolerance
        xf = (int2 - ic_q[ii]) / (fp_m - c_a2);
        yf = (fp_m * int2 - c_a2 * ic_q[ii]) / (fp_m - c_a2);
        index2[ii] = (int)floor(abs(Vector3D(xf, yf, 0.) - ic_zero[ii]) / ic_width);
        if(index2[ii] < 0) index2[ii] = -1;
    }
    for(int ii = 0; ii < nSec - 1; ii++)
    {
        firstid[ii] = MIN(index1[ii], index1[ii + 1]);
        lastid [ii] = MAX(index2[ii], index2[ii + 1]);
    }

    // now checks whether sections above threshold are compatible with the trajectory
    // sets to zero otherwise!!! Chamber is rejected if more than 1 section above
    // threshold fires outside of the reconstructed trajectory
    int    position = 0;
    int    ind_det;
    double buffer;
    int    n_reject = 0;
    last_sec = -1;
    for(int jj = 0; jj < n_det; jj++)
    {
        ind_det = detnumbers[jj];
        for(int ii = 0; ii < nSec - 1; ii++)
        {
            buffer = chamber_data[position++];
            if((buffer > ic_threshold) && (buffer < ic_up_thresh))
            {
                if((ind_det < firstid[ii]) && (ind_det > lastid[ii]))
                {
                    n_reject++;
                    chamber_data[position - 1] = 0.;
                }
                else
                    last_sec = MAX(ii, last_sec);
            }
            else
            {
                chamber_data[position - 1] = 0.;
            }
        }
    }
    if(n_reject > 1 || last_sec < 0)
        return false;
    else
        return true;
}
//**************************************************************************************
bool TfastSolver::processChamber(int n_det)
{
    // we assume here that bad sections have already been rejected
    //double* dE = new double[nSec-1];

    for(int ii = 0; ii < nSec - 1; ii++)
        dE[ii] = 0.;

    // evaluate energy deposition in each of the sections
    int position = 0;
    for(int jj = 0; jj < n_det; jj++)
    {
        // 30/01/08 not needed any more with the new data format!!!
        // dummy = chamber_data[position++];  // detector number
        for(int ii = 0; ii < nSec - 1; ii++)
        {
            dE[ii] += chamber_data[position++];
        }
    }

    // "weighted distance" as weighted average of section distance (weight: deposited energy)
    ic_energy = 0.;
    ic_a_de   = 0.;
    ic_ab_de  = 0.;
    double ztmp = 0.;
    for(int ii = 0; ii < nSec - 1; ii++)
    {
        ic_energy += dE[ii];
        ztmp += centre_section[ii] * dE[ii];
    }

    ic_a_de  = dE[0];
    ic_ab_de = dE[0] + dE[1];

    // subtract offset (distance from PPAC to IC is in vacuum and does not contribute to
    // energy loss
    double zz = ztmp / ic_energy - ic_fp_distance;

    // entrance angle in the focal plane
    double costheta = vel_dip_out.X() * cos(30.*degree) + vel_dip_out.Y() * sin(30.*degree);
    costheta /= abs(vel_dip_out);

    // corrects for entrance angle
    ztmp = zz / costheta / mm;

    // Final data manipulation - copied from GSORT subroutine
    // Does anybody know why data are handled this way???

    double depth = ic_depth / mm;

    double dEr = 0.2 * ic_energy / dE[0];

    double etmp = 0.;
    double dE2 = 0.;
    double sqdE = 0.;
    double tmp = 0.;
    double ddE2;
    double sqddE;
    for(int ii = 0; ii < nSec - 1; ii++)
    {
        ddE2  = dE[ii] * dE[ii];
        sqddE = sqrt(dE[ii]);
        dE2  += ddE2;
        etmp += ((double)(2 * ii + 1)) * ddE2;
        tmp  += ((double)(2 * ii + 1)) * sqddE;
        sqdE += sqddE;
    }
    if(dE2  == 0.) return false;
    if(sqdE == 0.) return false;

    etmp = 0.5 * depth * etmp / dE2;
    tmp  = 0.5 * depth * tmp  / sqdE;

    ic_path  = 5.0 * (ztmp - 0.25 * (etmp + tmp - 2.0 * ztmp));
    ic_range = pow(dEr * (ic_path - 400.0), 0.282830) * 400.0;

    return true;
}
//#endif
//**************************************************************************************
bool TfastSolver::FindChamber(int* origIndex, std::vector<detData*>& origData)
{
    ic_energy = 0.;
    ic_a_de   = 0.;
    ic_ab_de  = 0.;
    ic_path   = 0.;
    ic_range  = 0.;

    if(!GoodSections(origIndex, origData))
        return false;
    if(!processChamber(origIndex, origData))
        return false;
    solved_ch = 1;

    return true;
}
//**************************************************************************************
bool TfastSolver::GoodSections(int* origIndex, std::vector<detData*>& origData)
{
    // calculates which sections should be crossed by the present (reconstructed)
    // particle trajectory (including angular tolerance)
    double theta = atan(c_ang);
    if(theta < 0.)
        theta += 180.*degree;

    double theta1 = theta + angular_tolerance;
    double c_a1   = tan(theta1);
    double int1   = pos_dip_out.Y() - c_a1 * pos_dip_out.X();

    double theta2 = theta - angular_tolerance;
    double c_a2   = tan(theta2);
    double int2   = pos_dip_out.Y() - c_a2 * pos_dip_out.X();
    double xf;
    double yf;

    for(int ii = 0; ii < nSec; ii++)
    {
        // intersection 1: + tolerance
        xf = (int1 - ic_q[ii]) / (fp_m - c_a1);
        yf = (fp_m * int1 - c_a1 * ic_q[ii]) / (fp_m - c_a1);
        index1[ii] = (int)floor(abs(Vector3D(xf, yf, 0.) - ic_zero[ii]) / ic_width);
        if(index1[ii] < 0) index1[ii] = -1;

        // intersection 1: - tolerance
        xf = (int2 - ic_q[ii]) / (fp_m - c_a2);
        yf = (fp_m * int2 - c_a2 * ic_q[ii]) / (fp_m - c_a2);
        index2[ii] = (int)floor(abs(Vector3D(xf, yf, 0.) - ic_zero[ii]) / ic_width);
        if(index2[ii] < 0) index2[ii] = -1;
    }
    for(int ii = 0; ii < nSec - 1; ii++)
    {
        firstid[ii] = MIN(index1[ii], index1[ii + 1]);
        lastid [ii] = MAX(index2[ii], index2[ii + 1]);
    }

    // now checks whether sections above threshold are compatible with the trajectory
    // sets to zero otherwise!!! Chamber is rejected if more than 1 section above
    // threshold fires outside of the reconstructed trajectory
    int    position = 0;
    int    ind_det;
    double buffer;
    int    n_reject = 0;
    last_sec = -1;

    for(int jj = 0; jj < (int)origData.size(); jj++)
    {
        ind_det = origData[jj]->num_det();
        for(int ii = 0; ii < nSec - 1; ii++)
        {
            position = origIndex[ii];
            buffer = (*origData[jj])[position];
            //cout << " detec " << ind_det << " " << buffer << endl;
            if((buffer > ic_threshold) && (buffer < ic_up_thresh))
            {
                if((ind_det < firstid[ii]) && (ind_det > lastid[ii]))
                {
                    //      cout << " Scarto!" << endl;
                    //      cout << (*origData[jj]);
                    n_reject++;
                    origData[jj]->set(position, 0.);
                    //      cout << (*origData[jj]);
                }
                else
                    last_sec = MAX(ii, last_sec);
            }
            else
            {
                //    cout << " Butto!" << endl;
                //    cout << (*origData[jj]);
                origData[jj]->set(position, 0.);
                //    cout << (*origData[jj]);
            }
        }
    }
    //  if( n_reject > 1 )
    //    cout << " Rigettati " << n_reject << endl;
    if(n_reject > 1 || last_sec < 0)
        return false;
    else
        return true;
}
//**************************************************************************************
bool TfastSolver::processChamber(int* origIndex, std::vector<detData*>& origData)
{
    //#if 0
    // we assume here that bad sections have already been rejected
    //double* dE = new double[nSec-1];

    for(int ii = 0; ii < nSec - 1; ii++)
        dE[ii] = 0.;

    // evaluate energy deposition in each of the sections
    int position = 0;
    for(int jj = 0; jj < (int)origData.size(); jj++)
    {
        //position = origIndex[ii];
        //cout << " Position " << position << endl;
        for(int ii = 0; ii < nSec - 1; ii++)
        {
            position = origIndex[ii];

            //      if( (*origData[jj])[position] > ic_threshold && (*origData[jj])[position] < 3500. )
            if((*origData[jj])[position] > ic_threshold)    // data higher than upper threshold were zeroed!
                dE[ii] += (*origData[jj])[position];
            //cout << " DE " << ii << " " << dE[ii] << endl;
        }
    }
    //#endif
    // "weighted distance" as weighted average of section distance (weight: deposited energy)
    ic_energy = 0.;
    ic_a_de   = 0.;
    ic_ab_de  = 0.;
    double ztmp = 0.;
    for(int ii = 0; ii < nSec - 1; ii++)
    {
        ic_energy += dE[ii];
        ztmp += centre_section[ii] * dE[ii];
    }
    ic_a_de  = dE[0];
    ic_ab_de = dE[0] + dE[1];
    //cout << " IC energy " << ic_energy << " " << ztmp << endl;

    // subtract offset (distance from PPAC to IC is in vacuum and does not contribute to
    // energy loss
    double zz = ztmp / ic_energy - ic_fp_distance;

    // entrance angle in the focal plane
    double costheta = vel_dip_out.X() * cos(30.*degree) + vel_dip_out.Y() * sin(30.*degree);
    costheta /= abs(vel_dip_out);

    // corrects for entrance angle
    ztmp = zz / costheta / mm;

    // Final data manipulation - copied from GSORT subroutine
    // Does anybody know why data are handled this way???

    double depth = ic_depth / mm;

    double dEr = 0.2 * ic_energy / dE[0];

    double etmp = 0.;
    double dE2 = 0.;
    double sqdE = 0.;
    double tmp = 0.;
    double ddE2;
    double sqddE;
    for(int ii = 0; ii < nSec - 1; ii++)
    {
        ddE2  = dE[ii] * dE[ii];
        sqddE = sqrt(dE[ii]);
        dE2  += ddE2;
        etmp += ((double)(2 * ii + 1)) * ddE2;
        tmp  += ((double)(2 * ii + 1)) * sqddE;
        sqdE += sqddE;
    }
    if(dE2  == 0.) return false;
    if(sqdE == 0.) return false;

    etmp = 0.5 * depth * etmp / dE2;
    tmp  = 0.5 * depth * tmp  / sqdE;

    ic_path  = 5.0 * (ztmp - 0.25 * (etmp + tmp - 2.0 * ztmp));
    ic_range = pow(dEr * (ic_path - 400.0), 0.282830) * 400.0;

    return true;
}
//**************************************************************************************
