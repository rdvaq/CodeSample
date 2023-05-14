// List of all opcodes, parsers, and unparsers

// You'll develop this list from module 2 onward.  Every time
// you add a new instruction, you'll add an entry here.
// You'll also define the opcode in file opcodes.h,
// and you'll add a case to your `vmrun` function.

#include "iformat.h"
#include "name.h"
#include "itable.h"

#include <stdio.h>

#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

instruction_info instructions[] = {

  { "print", Print, parseR1, "print rX" },
  { "println", Println, parseR1, "println rX" },
  { "printu", Printu, parseR1, "printu rX" },
  { "if", If, parseR1, "if rX" },
  { "error", Error, parseR1, "error rX" },
  { "add", Add, parseR3, "rX := rY + rZ"},
  { "+", Add, parseR3, "rX := rY + rZ"},
  { "cons", Cons, parseR3, "rX := rY cons rZ" },
  { "mul", Mul, parseR3, "rX := rY * rZ" },
  { "*", Mul, parseR3, "rX := rY * rZ" },
  { "/", DDiv, parseR3, "rX := rY /d rZ" },
  { "div", DDiv, parseR3, "rX := rY /d rZ" },
  { "-", Minus, parseR3, "rX := rY - rZ" },
  { "idiv", IDiv, parseR3, "rX := rY /i rZ" },
  { ">", Greater, parseR3, "rX := rY > rZ" },
  { "<", Less, parseR3, "rX := rY < rZ" },
  { "==", Equal, parseR3, "rX := rY == rZ" },
  { "=", Equal, parseR3, "rX := rY == rZ" },
  { "not", Not, parseR2, "rX := ! rY"},
  { "!", Not, parseR2, "rX := ! rY"},
  { "pbool", PBool, parseR2, "rX := pbool rY"},
  { "car", Car, parseR2, "rX := car rY"},
  { "cdr", Cdr, parseR2, "rX := cdr rY"},
  { "hash", Hash, parseR2, "rX := hash rY"},
  { "function?", isFun, parseR2, "rX := function? rY"},
  { "pair?", isPair, parseR2, "rX := pair? rY"},
  { "symbol?", isSym, parseR2, "rX := symbol? rY"},
  { "number?", isNum, parseR2, "rX := number? rY"},
  { "boolean?", isBool, parseR2, "rX := boolean? rY"},
  { "null?", isNull, parseR2, "rX := null? rY"},
  { "nil?",  isNil, parseR2, "rX := nil? rY"},
  { "copy",  Copy, parseR2, "rX := rY"},
  { "check", Check, parseR1LIT, "check rX LIT" },
  { "expect", Expect, parseR1LIT, "expect rX LIT" },
  { "check-assert", CAssert, parseR1LIT, "check-assert rX LIT" },
  { "loadliteral", LoadLiteral, parseR1LIT, "rX := LIT" },
  { "getglobal", Getglobal, parseR1GLO, "rX := GLOBAL" },
  { "setglobal", Setglobal, parseR1GLO, "GLOBAL := rX" },
  { "goto", Goto, parseR0I24, "goto iXYZ" },
  { "halt", Halt, parseR0, "halt" },
  { "mkclosure", Mkclosure, parseR2U8, "rX := closure[rY, Z]"}, 
  { "getclslot", Getclslot, parseR2U8, "rX := rY.Z"}, 
  { "setclslot", Setclslot, parseR2U8, "rX.Z := rY"}, 
  { "call", Call, parseR3, "rX := call rY (rY+1, ..., rZ)"},
  { "return", Return, parseR1, "return rX"},
  { "tailcall", Tailcall, parseR2, "tailcall rX (rX+1, ..., rY)"},
  { "gc", GC, parseR0, "gc"}
};

int number_of_instructions = sizeof(instructions) / sizeof(instructions[0]);

bool isgetglobal(Opcode code) {
  return code == Getglobal; 
}

bool op_istailcall(Opcode code) {
  return code == Tailcall;
}