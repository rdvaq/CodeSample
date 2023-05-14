// The VM loader: read virtual object code and update VM state

// This is the focus of module 2.  In future modules, we'll continue
// to use it, but we won't revise it except for an optional depth goal.
//
// You'll write the core of the loader: function `loadfun`.  This
// function loads a single VM function from a .vo file.  It is called
// by `loadmodule`, which load a single module, and which I provide.
// (A module is just a function that takes no parameters).  I also
// provide `loadmodules`, which loads a list of modules.  Finally, I
// provide `parse_instruction`, which uses the parsing table in file
// instructions.c to parse each single instruction.



#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "loader.h"
#include "itable.h"
#include "tokens.h"
#include "svmdebug.h"
#include "vmstate.h"
#include "vmheap.h"
#include "vmsizes.h"
#include "vmstring.h"
#include "value.h"


//// Variables and utility functions local to this module

static bool saw_a_bad_opcode = false;
const uint32_t no_source_location = 0;   
#ifndef STACKTRACE_T
    #define STACKTRACE_T 0
#endif

static Instruction
  parse_instruction(VMState vm, Name opcode, Tokens operands, unsigned *maxregp);
  // Parse `operands` according to the parser for the given `opcode`.
  // Return the parsed instruction.  If the instruction mentions any
  // register, update `*maxregp` to be at least as large as any
  // mentioned register (and no smaller than it was).
  //
  // If the opcode is not recognized, set `saw_a_bad_opcode` and return 0.

static char *buffer; // for use by getline(3), shared by functions
size_t bufsize;      // `loadmodule` and `get_instruction`



//// The functions you will write

static struct VMFunction *loadfun(VMState vm, Name name, int arity, int count, FILE *input);
  // Allocates and returns a new function using `vmalloc`, with all fields
  // correctly set:
  //   - The arity is given by parameter `arity`.
  //   - The number of instructions is given by parameter `count`.
  //   - The instructions themselves are read from `input`.
  //   - The `nregs` field must be set to one more than the largest
  //     register mentioned in any instruction.

static uint64_t get_instruction(VMState vm, FILE *vofile, unsigned *maxregp);
  // Consumes one <instruction> nonterminal from `vofile`,
  // encodes the resulting instruction `i`, and returns it.
  // STACKTRACE EDIT: now returns a 64 bit word where first 32 bit is the ins
  // and the last 32bit is a potential location
  // Also has the following side effects:
  //    - On return, every literal mentioned in the instruction `i`
  //      is present in vm's literal pool.
  //    - For each register X, Y, or Z mentioned in `i`, `*maxregp`
  //      is set to the larger itself and X, Y, or Z.
  // May overwrite `buffer` and `bufsize` by calling `getline`.

static uint64_t get_instruction(VMState vm, FILE *vofile, unsigned *maxregp) {
    Instruction ins;
    uint32_t srcloc;
    static Name dotloadname, functiontag, dotatname;
    if (dotloadname == NULL) {
        dotloadname = strtoname(".load");
        functiontag = strtoname("function");
        dotatname   = strtoname(".at");
    }

    if (getline(&buffer, &bufsize, vofile) < 0) {
        assert(false); 
    }

    Tokens alltokens = tokens(buffer);
    Tokens tokens_left = alltokens;

  
    Name n = tokens_get_name(&tokens_left, buffer);
    if (n == dotloadname) {
        uint32_t reg = tokens_get_int(&tokens_left, buffer);
        assert(reg < 256);
        Name ftag = tokens_get_name(&tokens_left, buffer);
        assert(ftag == functiontag);
        Name funname = tokens_get_name(&tokens_left, buffer); // get the function's name here
        uint32_t arity = tokens_get_int(&tokens_left, buffer);
        uint32_t length = tokens_get_int(&tokens_left, buffer);
        assert(tokens_left == NULL);
        struct VMFunction *fun = loadfun(vm, funname, arity, length, vofile); 
        uint32_t index = literal_slot(vm, mkVMFunctionValue(fun));
        assert(index == (uint16_t) index);
        if (reg > *maxregp) *maxregp = reg; // update maxreg
        ins = eR1U16(LoadLiteral, reg, index);
        srcloc = no_source_location;
    } else if (n == dotatname) {
        Name filename = tokens_get_name(&tokens_left, buffer);
        struct VMString* fn_string = Vmstring_newc(nametostr(filename));
        uint32_t index = literal_slot(vm, mkStringValue(fn_string));
        assert(index == (uint16_t) index);
        uint32_t line = tokens_get_int(&tokens_left, buffer);
        assert(line == (uint16_t) line);
        srcloc = pLoc(index, line); 
        // now get the actual instruction
        Name ins_name = tokens_get_name(&tokens_left, buffer);
        ins = parse_instruction(vm, ins_name, tokens_left, maxregp);
    } else {
        ins = parse_instruction(vm, n, tokens_left, maxregp);
        srcloc = no_source_location;
    }

    free_tokens(&alltokens);

    uint64_t ans = pInsLoc(ins, srcloc);
    return ans;
}

static struct 
VMFunction *loadfun_notail(VMState vm, Name name, int arity, int count, FILE *vofile) {
    Instruction* temp_ins = calloc(1, count * sizeof(*temp_ins));
    Instruction* temp_loc = calloc(1, count * sizeof(*temp_ins));
    int ins_size = 0; //number of instructions after replacing tail w/ call + ret
    int nregs = 0;
    for (int i = 0; i < count; i++) {
        uint64_t word = get_instruction(vm, vofile, (unsigned*)&(nregs));
        temp_ins[i] = getIns(word);
        temp_loc[i] = getLoc(word); 
        ins_size++;
        if (op_istailcall(opcode(getIns(word)))) ins_size++;
    }
    VMNEW(struct VMFunction*, fun, (vmsize_fun(ins_size + 1))); 
    fun->funname = name;
    fun->arity = arity;
    fun->num_insturctions = ins_size + 1;
    fun->nregs = (nregs == 0) ? nregs : ++nregs;

    int old_ins_it = 0;
    for (int i = 0; i < ins_size; i++) {
        Instruction oldi = temp_ins[old_ins_it];
        Instruction newi = oldi;
        if (op_istailcall(opcode(oldi))) {
            uint8_t funreg = uX(oldi);
            uint8_t lastreg = uY(oldi);
            newi = eR3(Call, funreg, funreg, lastreg);
            fun->instructions[i] = newi;
            fun->instructions[i + ins_size + 1] = temp_loc[old_ins_it];
            fun->instructions[++i] = eR1(Return, funreg);
            fun->instructions[i + ins_size + 1] = no_source_location;
        } else {
            fun->instructions[i] = newi;
            fun->instructions[i + ins_size + 1] = temp_loc[old_ins_it];
        }
        old_ins_it++;
    }
    fun->instructions[ins_size] = eR0(Halt);
    fun->instructions[2*ins_size+1] = no_source_location;
    free(temp_ins);
    free(temp_loc);
    assert(old_ins_it == (count--));
    return fun;
}

static struct VMFunction *loadfun(VMState vm, Name name, int arity, int count, FILE *vofile) {
    if (STACKTRACE_T && svmdebug_value("debug-tailcall")) {
        return loadfun_notail(vm, name, arity, count, vofile);
    } else {
        VMNEW(struct VMFunction*, fun, vmsize_fun(count + 1)); 
        fun->funname = name;
        fun->arity = arity;
        fun->num_insturctions = count + 1;
        fun->nregs = 0; // set to lowest val initially 
        for (int i = 0; i < count; i++) {
            uint64_t word = get_instruction(vm, vofile, (unsigned*)&(fun->nregs));
            fun->instructions[i] = getIns(word);
            fun->instructions[i+count+1] = getLoc(word); //add extra one for halt
        }
        fun->instructions[count] = eR0(Halt); //add a halt at the end 
        fun->instructions[2 * count + 1] = no_source_location; // loc for halt
        if (fun->nregs != 0) fun->nregs++;
        return fun;
    }
}


//// module loading, with helper function

static bool has_input(FILE *fd) {
  int c = getc(fd);
  if (c == EOF) {
    return false;
  } else {
    ungetc(c, fd);
    return true;
  }
}


struct VMFunction *loadmodule(VMState vm, FILE *vofile) {
  // precondition: `vofile` has input remaining

  static Name dotloadname, modulename;
  if (dotloadname == NULL) {
    dotloadname = strtoname(".load");
    modulename  = strtoname("module");
  }

  if (!has_input(vofile))
    return NULL;

  // read a line from `vofile` and tokenize it
  if (getline(&buffer, &bufsize, vofile) < 0) {
    // end of file, spec calls for NULL to be returned
    assert(false);
  }
  Tokens alltokens = tokens(buffer);
  Tokens tokens_left = alltokens;

  // parse the tokens; expecting ".load module <count>"
  Name n;
  n = tokens_get_name(&tokens_left, buffer); // removes token from tokens_left
  assert(n == dotloadname);
  n = tokens_get_name(&tokens_left, buffer);
  assert(n == modulename);
  uint32_t count = tokens_get_int(&tokens_left, buffer);
  assert(tokens_left == NULL); // that must be the last token

  struct VMFunction *module = loadfun(vm, strtoname("loaded module"), 0, count, vofile); // read the remaining instructions

  if (buffer) {
    free(buffer);
    buffer = NULL;
  }
  free_tokens(&alltokens);

  return module;
}


///// utility functions

static Instruction
parse_instruction(VMState vm, Name opcode, Tokens operands, unsigned *maxregp) {
  instruction_info *info = itable_entry(opcode);
  if (info) {
    return info->parser(vm, info->opcode, operands, maxregp);
  } else {
    fprintf(stderr, "No opcode for %s.\n", nametostr(opcode));
    saw_a_bad_opcode = true;
    return 0;
  }
}
