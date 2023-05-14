// Memory management and literal addition for VMState

// You'll complete this file as part of module 1


#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "vmstate.h"
#include "vtable.h"
#include "value.h"

void freestatep(VMState *sp) {
    assert(sp && *sp);
    VMState vm = *sp;
    free(vm);
    sp = NULL;
}

VMState newstate(void) {
    // allocate, initialize, and return a new state
    VMState newstate = calloc(1, sizeof(struct VMState));
    // put &gamma at slot 0 of the globals array
    const char gamma[] = "&gamma"; 
    newstate->global_names[0] = strtoname(gamma);
    newstate->num_globs++;
    assert(newstate);
    return newstate;
}

int literal_slot(VMState state, Value literal) {
    // Return a slot containing the literal, updating literal pool if needed.
    // For module 1, you can get away with putting the literal in slot 0
    // and returning 0.  For module 2, you'll need something slightly
    // more sophisticated.
    int idx = state->num_lits++;
    // fprintf(stderr, "idx is: %d\n", idx);
    assert(idx < LIT_SIZE);
    state->literals[idx] = literal;
    return idx;
}

// these are for module 2 and beyond

Value literal_value(VMState state, unsigned index) {
    return state->literals[index];
}

int literal_count(VMState state) {
    return state->num_lits;
}

const char *global_name(VMState state, unsigned index) {
    return nametostr(state->global_names[index]);
}

void initialize_global(VMState vm, Value name, Value v) {
    int slot = global_slot(vm, name);
    vm->globals[slot] = v;
}

int global_slot(VMState state, Value name) {
    Name n = strtoname(AS_CSTRING(state, name, -1));
    int idx;
    int num_globs = state->num_globs;
    for (idx = 0; idx < num_globs; idx++) {
      if (state->global_names[idx] == n)
        return idx;
    }
    idx = state->num_globs++;
    assert(idx < GLOB_SIZE);
    state->global_names[idx] = n;
    return idx;
}