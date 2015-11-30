#include "Tfrs_element.h"
#include "Tfrs.h"

#include <algorithm>

Tfrs *Tfrs_element::owner ;  // static it is only one frs
TIFJEvent * Tfrs_element::event_unpacked ;  // static,

// for spectra, they can contain a standard texts

// string noraw("Raw values are (very often) not defined as incrementers  - too many of them. You can try to find them as vme[][] incrementers (see the lookup table)");
string noraw("Raw values are (very often) not defined as incrementers  - too many of them. You can try to find them as vme incrementers (see the lookup table)");

string noinc =
    "No such incrementer defined. Ask Jurek if you really need it";

// Done incrementers list   for
// MW, Tscintill, Taoq_calculator, Tbonn_magnet, Tdegrader, Tmusic_8_anodes
// Ttof_measurement, Tfocal_plane

// Ommited:
// Tsi_detector
// Tscaler
// Tsi_pin_diode
// Ttarget
// Target scintillator, Ttwo signal module
// Tclover
//**************************************************************************
Tfrs_element::Tfrs_element(string name) : name_of_this_element(name)
{
    frs_spectra_ptr = owner->address_spectra_frs() ;
}
//**************************************************************************
