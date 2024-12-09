void rotate(double x, double y, double &xr, double &yr);
bool prepare_rotation();
double sinus_omega;
double cosinus_omega;
bool flag_rotation_possible {false};
double fast_vs_slow_rotated_x;
double fast_vs_slow_rotated_y;
bool flag_fast_vs_slow_rotated_ok;
double fast_vs_slow_rotated_x_cal;
double fast_vs_slow_rotated_y_cal;

###########################################################


//*****************************************************************
template  <typename TOwnerClass>
void Thector_BaF<TOwnerClass>::rotate(double x, double y, double &xr, double &yr)
{
    if(flag_rotation_possible)
    {
        xr = (x * cosinus_omega) - (y * sinus_omega);
        yr = (x * sinus_omega) + ( y * cosinus_omega);
        flag_fast_vs_slow_rotated_ok = true;
    }
    else {
        xr = 0;
        yr = 0;
        //cout << "Rotation impossible" << endl;
    }
}
//*****************************************************************
template  <typename TOwnerClass>
bool Thector_BaF<TOwnerClass>::prepare_rotation()
{
    // find polygon for the fast_vs_slow_cal matrix

    TjurekPolyCond   *polygon = nullptr;
    string name =   name_of_this_element + "_fast_vs_slow_polygon_rotation.poly" ;
    flag_rotation_possible = read_banana(
                name,
                &polygon);
    if(!flag_rotation_possible)
    {
        // do it new


        // creating it ----------------------------------------
        ofstream plik(("polygons/" + name).c_str());
        if(!plik)         exit(1);



        double points[8] =
        {
            876.126,		326,		 // x, y of the point nr 0	 polygon: rotation
            1260.15,		1701,		 // x, y of the point nr 1	 polygon: rotation
            1204.76,		396,		 // x, y of the point nr 2	 polygon: rotation
            1042.29,		216		 // x, y of the point nr 3	 polygon: rotation
        };

        for(int n = 0 ; n < 8 ; n += 2)
            plik << points[n]  << "\t" << points[n + 1]  << "\n";

        plik.close();
        // once more we try something what really exit
        if(! read_banana(name, &polygon))
        {
            cout << "Could not create the polygon " << name << " for you " << endl;
            exit(125);
        }

        flag_LaBr_polygon_possible = true;

    } // end if flag rotation


    // find first vertext and calculate tangens omega = y/x
    vector<double> vx;
    vector<double> vy;
    polygon->give_point_vectors(vx, vy); // references
    if(vx.size()<2 || vy.size() < 2) {
        flag_rotation_possible = false;
        return false;
    }

    double x = vx[1] - vx[0];
    double y = vy[1] - vy[0];
    double omega = atan(x / y);

    sinus_omega = sin(omega);
    cosinus_omega = cos(omega);
    return true;
}

========================================

