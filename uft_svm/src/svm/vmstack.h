#ifndef VMSTACK_INCLUDED
#define VMSTACK_INCLUDED

#include "value.h"

/* Update for STACKTRACE:
Change to Activation record
dest, window_start, return_to still belongs to the caller
fun is now the caller function, not the callee function

TLDR: an Activation record stores the caller's register window, the destination
register and prog counter relative to the caller, but the callee's function
*/

/*
uint8_t tailcall_info:
first bit determines if the current record "experienced" tailcalls.
rest of the bits record number of tailcalls, not including the curr function
*/

struct Activation {
    /*caller*/
    int dest; 
    Value* window_start; 
    int return_to; 
    /*callee*/
    struct VMFunction *fun; 
    /*tailcall identifier*/
    uint8_t tailcall_info;
};

static inline bool istailcall(uint8_t info);
static inline uint8_t settailcall();
static inline unsigned gettailcalls(uint8_t info);
/*assumes the tailcall bit is aleady set to 1*/
static inline uint8_t incrementtailcall(uint8_t info);





/* implementation of tailcall related functions */
static inline bool istailcall (uint8_t info) { return (info >> 7); }
static inline uint8_t settailcall () { return 0x80; }
static inline unsigned gettailcalls(uint8_t info) { return (info & 0x7f); }
static inline uint8_t incrementtailcall (uint8_t info) { 
    assert(istailcall(info)); //production only
    if (info == 0xff) return info;
    
    uint8_t mask = 1;
    while (mask & info) {
        info &= ~mask;
        mask <<= 1;
    }
    return (info | mask);
}

#endif
