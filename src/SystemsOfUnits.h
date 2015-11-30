//////////////////////////////
//// Taken from A.Latina   ///
//////////////////////////////
#ifndef SYSTEMS_OF_UNITS_HPP
#define SYSTEMS_OF_UNITS_HPP

namespace
{

//
// Angle
//

const double      pi  = 3.14159265358979323846;
const double  two_pi  = 2 * pi;
const double four_pi  = 4 * pi;
const double half_pi  = pi / 2;
const double      pi2 = pi * pi;

const double radian      = 1;
const double milliradian = 1e-3 * radian;
const double degree      = (3.14159265358979323846 / 180.0) * radian;

const double steradian       = 1;

// symbols

const double rad  = radian;
const double mrad = milliradian;
const double sr   = steradian;
const double deg  = degree;

//
// Amount of substance
//

const double mole = 1;

//
// Miscellaneous
//

const double per_cent     = 0.01;
const double per_thousand = 0.001;
const double per_million  = 0.000001;
}

namespace InternationalSystem
{

// Lenght [L]

const double meter  = 1.;
const double meter2 = meter * meter;
const double meter3 = meter * meter * meter;

const double millimeter  = 1e-3 * meter;
const double millimeter2 = millimeter * millimeter;
const double millimeter3 = millimeter * millimeter * millimeter;

const double centimeter  = 1e-2 * meter;
const double centimeter2 = centimeter * centimeter;
const double centimeter3 = centimeter * centimeter * centimeter;

const double kilometer  = 1000 * meter;
const double kilometer2 = kilometer * kilometer;
const double kilometer3 = kilometer * kilometer * kilometer;

const double micrometer = 1e-6 * meter;
const double nanometer  = 1e-9 * meter;
const double fermi      = 1e-15 * meter;

const double barn      = 1e-28 * meter2;
const double millibarn = 1e-3 * barn;
const double microbarn = 1e-6 * barn;
const double nanobarn  = 1e-9 * barn;

// symbols

const double um  = micrometer;
const double um2 = micrometer * micrometer;
const double um3 = micrometer * micrometer * micrometer;

const double mm  = millimeter;
const double mm2 = millimeter2;
const double mm3 = millimeter3;

const double cm  = centimeter;
const double cm2 = centimeter2;
const double cm3 = centimeter3;

const double m  = meter;
const double m2 = meter2;
const double m3 = meter3;

const double km  = kilometer;
const double km2 = kilometer2;
const double km3 = kilometer3;

//
// Time [T]
//

const double second      = 1;
const double millisecond = 1e-3 * second;
const double microsecond = 1e-6 * second;
const double nanosecond  = 1e-9 * second;
const double picosecond    = 1e-12 * second;

const double hertz     = 1 / second;
const double kilohertz = 1e+3 * hertz;
const double megahertz = 1e+6 * hertz;

// symbols

const double  s = second;
const double ms = millisecond;
const double us = microsecond;
const double ns = nanosecond;
const double ps = picosecond;

//
// Mass [E][T^2][L^-2]
//

const double  kilogram = 1;
const double      gram = 1e-3 * kilogram;
const double milligram = 1e-3 * gram;

const double  kilogramforce = 1 / 9.80665 / kilogram;
const double      gramforce = 1e-3 * kilogramforce;
const double milligramforce = 1e-3 * gramforce;

// symbols

const double  kg = kilogram;
const double   g = gram;
const double  mg = milligram;

const double  kgf = kilogramforce;
const double   gf = gramforce;
const double  mgf = milligramforce;

//
// Force [E][L^-1]
//

const double newton = meter * kilogram / second / second;
const double dine   = 1e-5 * newton;

//
// Pressure [E][L^-3]
//

const double pascal     = newton / m2;     // pascal = 6.24150 e+3 * MeV / mm3
const double bar        = 100000 * pascal; // bar    = 6.24150 e+8 * MeV / mm3
const double atmosphere = 101325 * pascal; // atm    = 6.32420 e+8 * MeV / mm3

//
// Energy [E]
//

const double joule = newton * meter;

//
// Electric charge [Q]
//

const double coulomb = 1;                     // coulomb = 6.24150 e+18 * eplus
const double eplus   = 1.60217733e-19;      // positron charge
const double e_SI    = 1.60217733e-19;      // positron charge in coulomb

//
// Electric potential [E][Q^-1]
//

const double     volt = joule / coulomb;
const double kilovolt = 1e+3 * volt;
const double megavolt = 1e+6 * volt;

//
// Energy
//

const double     electronvolt = eplus * volt;
const double megaelectronvolt = 1e+6 * electronvolt;
const double kiloelectronvolt = 1e-3 * megaelectronvolt;
const double gigaelectronvolt = 1e+3 * megaelectronvolt;
const double teraelectronvolt = 1e+6 * megaelectronvolt;

// symbols

const double MeV = megaelectronvolt;
const double  eV = electronvolt;
const double keV = kiloelectronvolt;
const double GeV = gigaelectronvolt;
const double TeV = teraelectronvolt;

//
// Power [E][T^-1]
//

const double watt = joule / second;

//
// Electric current [Q][T^-1]
//

const double      ampere = coulomb / second; // ampere = 6.24150 e+9 * eplus / ns
const double milliampere = 1e-3 * ampere;
const double microampere = 1e-6 * ampere;
const double  nanoampere = 1e-9 * ampere;

//
// Electric resistance [E][T][Q^-2]
//

const double ohm         = volt / ampere;    // ohm = 1.60217e-16 * (MeV / eplus) / (eplus / ns)

//
// Electric capacitance [Q^2][E^-1]
//

const double farad      = coulomb / volt;   // farad = 6.24150e+24 * eplus / Megavolt
const double millifarad = 1e-3 * farad;
const double microfarad = 1e-6 * farad;
const double nanofarad  = 1e-9 * farad;
const double picofarad  = 1e-12 * farad;

//
// Magnetic Flux [T][E][Q^-1]
//

const double weber = volt * second;          // weber = 1000 * megavolt * ns

//
// Magnetic Field [T][E][Q^-1][L^-2]
//

const double tesla     = volt * second / meter2;  // tesla = 0.001 * megavolt * ns / mm2
const double gauss     = 1e-4 * tesla;
const double kilogauss = 1e-1 * tesla;

//
// Inductance [T^2][E][Q^-2]
//

const double henry = weber / ampere;    // henry = 1.60217e-7 * MeV * (ns / eplus) *  * 2

//
// Temperature
//

const double kelvin = 1;

//
// Activity [T^-1]
//

const double becquerel = 1 / second;
const double curie     = 3.7e+10 * becquerel;

//
// Absorbed dose [L^2][T^-2]
//

const double gray = joule / kilogram;

//
// Luminous intensity [I]
//

const double candela = 1;

//
// Luminous flux [I]
//

const double lumen = candela * steradian;

//
// Illuminance [I][L^-2]
//

const double lux = lumen / meter2;

}

namespace HighEnergyPhysicsSystem
{

const double millimeter  = 1;
const double millimeter2 = millimeter * millimeter;
const double millimeter3 = millimeter * millimeter * millimeter;

const double centimeter  = 10 * millimeter;
const double centimeter2 = centimeter * centimeter;
const double centimeter3 = centimeter * centimeter * centimeter;

const double meter  = 1000 * millimeter;
const double meter2 = meter * meter;
const double meter3 = meter * meter * meter;

const double kilometer  = 1000 * meter;
const double kilometer2 = kilometer * kilometer;
const double kilometer3 = kilometer * kilometer * kilometer;

const double micrometer = 1e-6 * meter;
const double nanometer  = 1e-9 * meter;
const double fermi      = 1e-15 * meter;

const double barn      = 1e-28 * meter2;
const double millibarn = 1e-3 * barn;
const double microbarn = 1e-6 * barn;
const double nanobarn  = 1e-9 * barn;

// symbols

const double um  = micrometer;
const double um2 = micrometer * micrometer;
const double um3 = micrometer * micrometer * micrometer;

const double mm  = millimeter;
const double mm2 = millimeter2;
const double mm3 = millimeter3;

const double cm  = centimeter;
const double cm2 = centimeter2;
const double cm3 = centimeter3;

const double m  = meter;
const double m2 = meter2;
const double m3 = meter3;

const double km  = kilometer;
const double km2 = kilometer2;
const double km3 = kilometer3;

//
// Time [T]
//

const double nanosecond  = 1;
const double second      = 1e+9 * nanosecond;
const double millisecond = 1e-3 * second;
const double microsecond = 1e-6 * second;
const double picosecond    = 1e-12 * second;

const double hertz     = 1 / second;
const double kilohertz = 1e+3 * hertz;
const double megahertz = 1e+6 * hertz;

// symbols

const double  s = second;
const double ms = millisecond;
const double us = microsecond;
const double ns = nanosecond;
const double ps = picosecond;

//
// Electric charge [Q]
//

const double eplus   = 1;                    // positron charge
const double e_SI    = 1.60217733e-19;       // positron charge in coulomb
const double coulomb = eplus / e_SI;         // coulomb = 6.24150 e+18 * eplus

//
// Energy [E]
//

const double megaelectronvolt = 1;
const double     electronvolt = 1e-6 * megaelectronvolt;
const double kiloelectronvolt = 1e-3 * megaelectronvolt;
const double gigaelectronvolt = 1e+3 * megaelectronvolt;
const double teraelectronvolt = 1e+6 * megaelectronvolt;

const double joule = electronvolt / e_SI;    // joule = 6.24150 e+12 * MeV

// symbols

const double MeV = megaelectronvolt;
const double  eV = electronvolt;
const double keV = kiloelectronvolt;
const double GeV = gigaelectronvolt;
const double TeV = teraelectronvolt;

//
// Mass [E][T^2][L^-2]
//

const double  kilogram = joule * second * second / (meter * meter);
const double      gram = 1e-3 * kilogram;
const double milligram = 1e-3 * gram;

const double  kilogramforce = 1 / 9.80665 / kilogram;
const double      gramforce = 1e-3 * kilogramforce;
const double milligramforce = 1e-3 * gramforce;

// symbols

const double  kg = kilogram;
const double   g = gram;
const double  mg = milligram;

const double  kgf = kilogramforce;
const double   gf = gramforce;
const double  mgf = milligramforce;

//
// Power [E][T^-1]
//

const double watt = joule / second;

//
// Force [E][L^-1]
//

const double newton = joule / meter;
const double dine   = 1e-5 * newton;

//
// Pressure [E][L^-3]
//

const double pascal     = newton / m2;     // pascal = 6.24150 e+3 * MeV / mm3
const double bar        = 100000 * pascal; // bar    = 6.24150 e+8 * MeV / mm3
const double atmosphere = 101325 * pascal; // atm    = 6.32420 e+8 * MeV / mm3

//
// Electric current [Q][T^-1]
//

const double      ampere = coulomb / second; // ampere = 6.24150 e+9 * eplus / ns
const double milliampere = 1e-3 * ampere;
const double microampere = 1e-6 * ampere;
const double  nanoampere = 1e-9 * ampere;

//
// Electric potential [E][Q^-1]
//

const double megavolt = megaelectronvolt / eplus;
const double kilovolt = 1e-3 * megavolt;
const double     volt = 1e-6 * megavolt;

//
// Electric resistance [E][T][Q^-2]
//

const double ohm         = volt / ampere;    // ohm = 1.60217e-16 * (MeV / eplus) / (eplus / ns)

//
// Electric capacitance [Q^2][E^-1]
//

const double farad      = coulomb / volt;   // farad = 6.24150e+24 * eplus / Megavolt
const double millifarad = 1e-3 * farad;
const double microfarad = 1e-6 * farad;
const double nanofarad  = 1e-9 * farad;
const double picofarad  = 1e-12 * farad;

//
// Magnetic Flux [T][E][Q^-1]
//

const double weber = volt * second;          // weber = 1000 * megavolt * ns

//
// Magnetic Field [T][E][Q^-1][L^-2]
//

const double tesla     = volt * second / meter2;  // tesla = 0.001 * megavolt * ns / mm2
const double gauss     = 1e-4 * tesla;
const double kilogauss = 1e-1 * tesla;

//
// Inductance [T^2][E][Q^-2]
//

const double henry = weber / ampere;    // henry = 1.60217e-7 * MeV * (ns / eplus) *  * 2

//
// Temperature
//

const double kelvin = 1;

//
// Activity [T^-1]
//

const double becquerel = 1 / second;
const double curie     = 3.7e+10 * becquerel;

//
// Absorbed dose [L^2][T^-2]
//

const double gray = joule / kilogram;

//
// Luminous intensity [I]
//

const double candela = 1;

//
// Luminous flux [I]
//

const double lumen = candela * steradian;

//
// Illuminance [I][L^-2]
//

const double lux = lumen / meter2;

}

namespace NuclearPhysicsSystem
{

const double meter  = 1;
const double meter2 = meter * meter;
const double meter3 = meter * meter * meter;

const double millimeter  = 1e-3 * meter;
const double millimeter2 = millimeter * millimeter;
const double millimeter3 = millimeter * millimeter * millimeter;

const double centimeter  = 10 * millimeter;
const double centimeter2 = centimeter * centimeter;
const double centimeter3 = centimeter * centimeter * centimeter;

const double kilometer  = 1000 * meter;
const double kilometer2 = kilometer * kilometer;
const double kilometer3 = kilometer * kilometer * kilometer;

const double micrometer = 1e-6 * meter;
const double nanometer  = 1e-9 * meter;
const double fermi      = 1e-15 * meter;

const double barn      = 1e-28 * meter2;
const double millibarn = 1e-3 * barn;
const double microbarn = 1e-6 * barn;
const double nanobarn  = 1e-9 * barn;

// symbols

const double um  = micrometer;
const double um2 = micrometer * micrometer;
const double um3 = micrometer * micrometer * micrometer;

const double mm  = millimeter;
const double mm2 = millimeter2;
const double mm3 = millimeter3;

const double cm  = centimeter;
const double cm2 = centimeter2;
const double cm3 = centimeter3;

const double m  = meter;
const double m2 = meter2;
const double m3 = meter3;

const double km  = kilometer;
const double km2 = kilometer2;
const double km3 = kilometer3;

//
// Time [T]
//

const double microsecond = 1;
const double second      = 1e+6 * microsecond;
const double millisecond = 1e-3 * second;
const double nanosecond  = 1e-9 * second;
const double picosecond    = 1e-12 * second;

const double hertz     = 1 / second;
const double kilohertz = 1e+3 * hertz;
const double megahertz = 1e+6 * hertz;

// symbols

const double  s = second;
const double ms = millisecond;
const double us = microsecond;
const double ns = nanosecond;
const double ps = picosecond;

//
// Electric charge [Q]
//

const double eplus   = 1;                    // positron charge
const double e_SI    = 1.60217733e-19;       // positron charge in coulomb
const double coulomb = eplus / e_SI;         // coulomb = 6.24150 e+18 * eplus

//
// Energy [E]
//

const double megaelectronvolt = 1;
const double     electronvolt = 1e-6 * megaelectronvolt;
const double kiloelectronvolt = 1e-3 * megaelectronvolt;
const double gigaelectronvolt = 1e+3 * megaelectronvolt;
const double teraelectronvolt = 1e+6 * megaelectronvolt;

const double joule = electronvolt / e_SI;    // joule = 6.24150 e+12 * MeV

// symbols

const double MeV = megaelectronvolt;
const double  eV = electronvolt;
const double keV = kiloelectronvolt;
const double GeV = gigaelectronvolt;
const double TeV = teraelectronvolt;

//
// Mass [E][T^2][L^-2]
//

const double  kilogram = joule * second * second / (meter * meter);
const double      gram = 1e-3 * kilogram;
const double milligram = 1e-3 * gram;

const double  kilogramforce = 1 / 9.80665 / kilogram;
const double      gramforce = 1e-3 * kilogramforce;
const double milligramforce = 1e-3 * gramforce;

// symbols

const double  kg = kilogram;
const double   g = gram;
const double  mg = milligram;

const double  kgf = kilogramforce;
const double   gf = gramforce;
const double  mgf = milligramforce;

//
// Power [E][T^-1]
//

const double watt = joule / second;

//
// Force [E][L^-1]
//

const double newton = joule / meter;
const double dine   = 1e-5 * newton;

//
// Pressure [E][L^-3]
//

const double pascal     = newton / m2;     // pascal = 6.24150 e+3 * MeV / mm3
const double bar        = 100000 * pascal; // bar    = 6.24150 e+8 * MeV / mm3
const double atmosphere = 101325 * pascal; // atm    = 6.32420 e+8 * MeV / mm3

//
// Electric current [Q][T^-1]
//

const double      ampere = coulomb / second; // ampere = 6.24150 e+9 * eplus / ns
const double milliampere = 1e-3 * ampere;
const double microampere = 1e-6 * ampere;
const double  nanoampere = 1e-9 * ampere;

//
// Electric potential [E][Q^-1]
//

const double megavolt = megaelectronvolt / eplus;
const double kilovolt = 1e-3 * megavolt;
const double     volt = 1e-6 * megavolt;

//
// Electric resistance [E][T][Q^-2]
//

const double ohm         = volt / ampere;    // ohm = 1.60217e-16 * (MeV / eplus) / (eplus / ns)

//
// Electric capacitance [Q^2][E^-1]
//

const double farad      = coulomb / volt;   // farad = 6.24150e+24 * eplus / Megavolt
const double millifarad = 1e-3 * farad;
const double microfarad = 1e-6 * farad;
const double nanofarad  = 1e-9 * farad;
const double picofarad  = 1e-12 * farad;

//
// Magnetic Flux [T][E][Q^-1]
//

const double weber = volt * second;          // weber = 1000 * megavolt * ns

//
// Magnetic Field [T][E][Q^-1][L^-2]
//

const double tesla     = volt * second / meter2;  // tesla = 0.001 * megavolt * ns / mm2
const double gauss     = 1e-4 * tesla;
const double kilogauss = 1e-1 * tesla;

//
// Inductance [T^2][E][Q^-2]
//

const double henry = weber / ampere;    // henry = 1.60217e-7 * MeV * (ns / eplus) *  * 2

//
// Temperature
//

const double kelvin = 1;

//
// Activity [T^-1]
//

const double becquerel = 1 / second;
const double curie     = 3.7e+10 * becquerel;

//
// Absorbed dose [L^2][T^-2]
//

const double gray = joule / kilogram;

//
// Luminous intensity [I]
//

const double candela = 1;

//
// Luminous flux [I]
//

const double lumen = candela * steradian;

//
// Illuminance [I][L^-2]
//

const double lux = lumen / meter2;

}

namespace SI    = InternationalSystem;
namespace HEP = HighEnergyPhysicsSystem;
namespace NPS = NuclearPhysicsSystem;

#endif /* SYSTEMS_OF_UNITS_HPP */
