// Defines all the opcodes used in the VM

// When you're thinking about new instructions, define them here first.
// It's OK for an opcode to be defined here even if it is not implemented 
// anywhere.  But if you want to *run* an instruction (module 1) or *load*
// an instruction (module 2), the opcode has to be defined here first.

#ifndef OPCODE_INCLUDED
#define OPCODE_INCLUDED

#include <stdbool.h>

typedef enum opcode { 
                      Print, Println, Printu, If, Error, // R1
                      Add, Cons, Mul, DDiv, Minus, IDiv, Greater, Less, Equal,
                      Not, PBool, Car, Cdr, Hash, isFun, isPair, isSym, isNum, isBool, isNull, isNil, Copy,
                      Check, Expect, CAssert, LoadLiteral, Getglobal, Setglobal, Goto, Halt,
                      Mkclosure, Getclslot, Setclslot,
                      Call, Return, Tailcall,
                      GC,
                      Unimp,
                      Unimp2, // stand-in for opcodes not yet implemented
} Opcode;

bool isgetglobal(Opcode code); // update this for your SVM, in instructions.c
bool op_istailcall(Opcode code);
#endif
