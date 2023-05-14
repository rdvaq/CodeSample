// Implementations of the error functions

// Modules < 7: Interesting only if you want to know how to write 
// a function that acts like printf.

// Module 7 onward: useful to extend with a stack trace.

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "print.h"
#include "value.h"
#include "vmerror.h"

#ifndef no_source_location
    #define no_source_location 0 
#endif

static inline void
output_error(FILE *fp, VMState vm, int err_site, struct VMFunction* fun) {
    uint32_t srcloc = fun->instructions[err_site + fun->num_insturctions];
    if (srcloc == no_source_location) {
        fprintf(fp, "no source code location available\n");
        return;
    }
    int line_number = getLinenum(srcloc);
    Value file_name = vm->literals[getIndex(srcloc)];
    fprintf(fp, "\tat %s:%d by %s\n", AS_CSTRING(vm, file_name, -1), // -1 as place holder
                                      line_number, nametostr(fun->funname));
}

static inline void stack_trace(FILE *fp, VMState vm, int pc) {
    fprintf(fp, "Stack Trace:\n");
    int num_activations = vm->num_activations;
    fprintf(fp, "num activations: %d\n", num_activations);
    struct Activation *stack = vm->stack;
    // first, print where the error occured with pc 
    output_error(fp, vm, pc, stack[num_activations - 1].fun);

    for (int i = --num_activations; i > 0; i--) {
        struct VMFunction *caller = stack[i-1].fun;
        int call_site = stack[i].return_to - 1; // pc of call site, -1 from return
        if (istailcall(stack[i].tailcall_info)) {
            fprintf(fp, "\t   tailcall detected, number of tailcalls(capped at 127): %u\n",
            gettailcalls(stack[i].tailcall_info));
        }
        output_error(fp, vm, call_site, caller);
    }
}

void runerror(VMState state, int pc, const char *format, ...) {
  (void)state;
  fprintf(stderr, "Run-time error:\n    ");
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fprintf(stderr, "\n");
  if (STACKTRACE) stack_trace(stderr, state, pc);
  abort();
}

void runerror_p(VMState state, int pc, const char *format, ...) {
  (void) state;
  fprintf(stderr, "Run-time error:\n    ");
  assert(format);
  va_list_box box;
  va_start(box.ap, format);
  vfprint(stderr, format, &box);
  va_end(box.ap);
  fprintf(stderr, "\n");
  if (STACKTRACE) stack_trace(stderr, state, pc);
  abort();
}

void typeerror(VMState state, const char *expected, Value got, int pc, const char *file, int line) {
  (void)state;
  fprintf(stderr, "Run-time error: expected %s, but got %s.\n"
          "\t(Internal source at %s, line %d)\n",
          expected, tagnames[got.tag], file, line);
  if (STACKTRACE) stack_trace(stderr, state, pc);
  abort();
}
