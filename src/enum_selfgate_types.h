#ifndef ENUM_SELFGATE_TYPES_H
#define ENUM_SELFGATE_TYPES_H


#error "Nie uzywac tego pliku"


#include <string>
using namespace std;


////////////////////////////////////////////////////////////////////////////////////
enum class  Type_of_selfgate {   // NEVER CHANGE THIS ORDER Or VALUES
    not_available,
    german_crystal,
    cluster_addback,
    hector,
    miniball,
    agata_psa           // Agata puls shape analysis
};
//*********************************************************************************
inline     string give_selfgate_type_name(Type_of_selfgate co)
{
    switch(co)
    {
        default:
        case Type_of_selfgate::not_available: return " X ";
        case Type_of_selfgate::german_crystal: return "german_crystal";
        case Type_of_selfgate::cluster_addback: return "cluster_addback";
        case Type_of_selfgate::hector: return "hector";
        case Type_of_selfgate::miniball: return "miniball";
        case Type_of_selfgate::agata_psa: return "agata_psa";

    }
}
//*********************************************************************************
inline string give_selfgate_type_extension(Type_of_selfgate co)
{
    switch(co)
    {
        default:
        case Type_of_selfgate::not_available: return ".not_available";
        case Type_of_selfgate::german_crystal: return ".self_gate_ger_crystal";
        case Type_of_selfgate::cluster_addback: return ".self_gate_ger_addback_cluster";
        case Type_of_selfgate::hector: return ".self_gate_hec_crystal";
        case Type_of_selfgate::miniball: return ".self_gate_mib_crystal";
        case Type_of_selfgate::agata_psa: return ".self_gate_agata_psa";
    }
}
#endif // ENUM_SELFGATE_TYPES_H



