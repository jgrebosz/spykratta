#ifndef _RANDOMIZER_H_
#define _RANDOMIZER_H_

// base class for all elements which need to
// randomize data for detectors

#include <time.h>


/////////////////////////////////////////////////////////////////////////////
class randomizer
{
protected:

    // member functions -------------------
    // constructor-------

    randomizer()
    {
        randomizer_word = time(0) / 512;  // null
    }

    static int randomizer_word  ;

public:

    static double randomek()
    {
        if(++randomizer_word > 511) randomizer_word = 0 ;
        return randomizer_word / 512.0 ; // 0-0.999999... (devided into 512 parts)
    }
};

#endif // _RANDOMIZER_H_
