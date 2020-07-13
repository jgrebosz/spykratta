#ifndef visitcard_h
#define visitcard_h
#include <stdint.h>


struct visitcard{
    char type[8];           // type of VME board
    char name[8];           // specific name of the board
    uint32_t hwaddr;        // VME address of the board
    uint32_t size;          // size of the data block, starting from type
    uint32_t data;          // Should be always last!
};

inline void ShowVisitcard(void * ptr){
    visitcard * v = (visitcard*)ptr;
   
    printf("%s board named %s at VME address 0x%08x data size %d\n", 
        v->type,
        v->name,
        v->hwaddr,
        v->size
        );
}


#endif
