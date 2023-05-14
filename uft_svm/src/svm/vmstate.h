// State of a VM, and functions to allocate, deallocate, add a literal

// This one's the essential part of module 1.
// You'll define the key representation, `struct VMState`,
// and you'll use it in your `vmrun` function.

#ifndef VMSTATE_INCLUDED
#define VMSTATE_INCLUDED

#include <stdint.h>
#include "stable.h"
#include "name.h"
#include "stable.h"
#include "name.h"
#include "value.h"
#include "vmstack.h"
#include "vtable.h"


#define REG_SIZE 20000
#define LIT_SIZE 4096
// #define LIT_SIZE 256
#define GLOB_SIZE 256
#define STACK_SIZE 2000

typedef struct VMState *VMState;

struct VMState {
    struct VMFunction* curr_prog;
    int program_counter;
    int num_lits;
    int num_globs;
    int num_activations;
    Value* reg_window;
    Value registers[REG_SIZE];
    Value literals[LIT_SIZE];
    Value globals[GLOB_SIZE];
    struct Name* global_names[GLOB_SIZE];
    struct Activation stack[STACK_SIZE];
    Value awaiting_expect;
};
// ... define the struct type here ...

VMState newstate(void);       // allocate and initialize (to empty)
void freestatep(VMState *sp); // deallocate


// The remaining functions won't be needed until module 2, but
// The remaining functions won't be needed until module 2, but
// they are worth thinking about now---and possibly writing.

int literal_slot(VMState state, Value literal);
  // return any index of literal in `literals`, adding if needed

int global_slot(VMState state, Value name);
  // return the unique index of `name` in `globals`, adding if needed.
  // The `name` parameter must be a VM string or the result is
  // a checked run-time error.

void initialize_global(VMState state, Value name, Value initial_value);
  // Equivalent to a `val` declaration; used for setting `argv`


// The last three functions are used only for disassembly.

Value literal_value(VMState state, unsigned index);
  // Return the value at the given index. *Not* intended 
  // for use in `vmrun`, in which you don't want to pay the 
  // overhead of a function call.

int literal_count(VMState state);
  // Returns N, the number of index values for which it
  // is ok to call `literal_value` (range 0 to N-1)

const char *global_name(VMState state, unsigned index);
  // Return the name of the global at the given index.

#endif /* VMSTATE_INCLUDED */
