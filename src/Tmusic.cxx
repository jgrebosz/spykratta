#include "Tmusic.h"

#include "Tfrs.h"
//***********************************************************************
//  constructor
Tmusic::Tmusic(string _name,
               string name_of_related_focus_point
#ifdef TPC42_PRESENT
               ,   string  name_of_nearest_tpc_focal_plane
#endif
              )
    : Tfrs_element(_name)
{
    related_focus = dynamic_cast<Tfocal_plane*>(owner->address_of_device(name_of_related_focus_point)) ;
#ifdef TPC42_PRESENT
    related_tpc_focus = dynamic_cast<Tfocal_plane*>(owner->address_of_device(name_of_nearest_tpc_focal_plane)) ;
#endif


    //create_my_spectra();
}

//**********************************************************************
// void Tmusic::analyse_current_event()   // <pure virtual again
//**************************************************************************
//************************************************************************

