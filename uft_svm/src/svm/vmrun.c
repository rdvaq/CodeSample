// Heart of the VM: runs instructions until told to halt

// You'll write a small `vmrun` function in module 1.  You'll pay
// some attention to performance, but you'll implement only a few 
// instructions.  You'll add other instructions as needed in future modules.

#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#include "check-expect.h"
#include "iformat.h"
#include "value.h"
#include "vmstate.h"
#include "vmrun.h"
#include "print.h"
#include "vmerror.h"
#include "vmheap.h"
#include "vmstring.h"
#include "vmsizes.h"
#include "svmdebug.h"
#include "disasm.h"
#include "vmstack.h"

#define CANDUMP 1

#define VMSAVE(STATE, REGWD, CURRFUN) \
    do {STATE->reg_window=REGWD; STATE->curr_prog=CURRFUN;} while (0)
#define VMLOAD(STATE, CURRFUN, INS) \
    do {CURRFUN=STATE->curr_prog; INS=STATE->curr_prog->instructions;} while (0)
#define GC(STATE, REGWD, CURRFUN, INS) \
    do {VMSAVE(STATE, REGWD, CURRFUN); \
        gc(vm);                        \
        VMLOAD(STATE, CURRFUN, INS);   \
    } while (0)

void vmrun(VMState vm, struct VMFunction *fun) {
    int pc = 0;
    vm->program_counter = pc;
    Instruction* instructions = fun->instructions;
    // INVARIANT: reg0 always points to the start of the current reg window
    Value* reg0 = &vm->registers[0]; 
    // INVARIANT: curr_fun always represent the current function that's being run
    struct VMFunction *curr_fun = fun;
    // INVARIANT: current function is always at the "top" of the stack
    struct Activation new_Act = {-1, reg0, pc+1, curr_fun, 0};
    vm->stack[vm->num_activations++] = new_Act;

    const char *dump_decode = svmdebug_value("decode");
    const char *dump_call   = svmdebug_value("call");
    // const char *debug_tail   = svmdebug_value("debug-tailcall");

    (void) dump_call;  // make it OK not to use `dump_call`

    while (1) { 
        // assert(pc >= 0 && pc < curr_fun->num_insturctions);
        Instruction ins = instructions[pc];
        if (CANDUMP && dump_decode) {
            idump(stderr, vm, pc, ins, 
            reg0-vm->registers,
            reg0+uX(ins), reg0+uY(ins), reg0+uZ(ins));
        }
        Opcode op = opcode(ins);
        switch (op) {
            case Halt:
                /* take off the loaded module, granted every function returns 
                and never falls through to the halt, should be ensured in codegen*/
                vm->num_activations--; 
                vm->program_counter = pc;
                return;
            case Print:  
                print("%v", reg0[uX(ins)]);
                break;
            case Println:  
                print("%v\n", reg0[uX(ins)]);
                break;
            case Printu:  
                print("%v\n", reg0[uX(ins)]);
                break;   
            case Error:
                print("%v\n", reg0[uX(ins)]); 
                return;     
            case Check: 
                check(vm, AS_CSTRING(vm, (vm->literals)[uYZ(ins)], pc), 
                                     reg0[uX(ins)]);
                break;
            case Expect: 
                expect(vm, AS_CSTRING(vm, (vm->literals)[uYZ(ins)], pc), 
                                      reg0[uX(ins)]);
                break;
            case CAssert:
                check_assert(AS_CSTRING(vm, vm->literals[uYZ(ins)], pc), reg0[uX(ins)]);
                break;
            case Add: 
                reg0[uX(ins)] = mkNumberValue (AS_NUMBER(vm, reg0[uY(ins)], pc) + 
                                               AS_NUMBER(vm, reg0[uZ(ins)], pc));
                break;
            case Minus: 
                reg0[uX(ins)] = mkNumberValue (AS_NUMBER(vm, reg0[uY(ins)], pc) - 
                                               AS_NUMBER(vm, reg0[uZ(ins)], pc));
                break;                
            case Mul:
                reg0[uX(ins)] = mkNumberValue (AS_NUMBER(vm, reg0[uY(ins)], pc) * 
                                               AS_NUMBER(vm, reg0[uZ(ins)], pc));               
                break;
            case Greater:
                reg0[uX(ins)] = mkBooleanValue (AS_NUMBER(vm, reg0[uY(ins)], pc) > 
                                                AS_NUMBER(vm, reg0[uZ(ins)], pc));               
                break; 
            case Less: 
                reg0[uX(ins)] = mkBooleanValue (AS_NUMBER(vm, reg0[uY(ins)], pc) < 
                                                AS_NUMBER(vm, reg0[uZ(ins)], pc));  
                break; 
            case DDiv: {// double division
                Number_T divisor = AS_NUMBER(vm, reg0[uZ(ins)], pc);
                if (divisor == 0) runerror(vm, pc, "dividing by 0");
                reg0[uX(ins)] = mkNumberValue (AS_NUMBER(vm, reg0[uY(ins)], pc) / 
                                               divisor);               
                break;}                         
            case Not:
                reg0[uX(ins)] = mkBooleanValue(!AS_BOOL(vm, reg0[uY(ins)], pc));
                break;
            case PBool:
                reg0[uX(ins)] = mkBooleanValue(AS_BOOL(vm, reg0[uY(ins)], pc));
                break;
            case Goto: // have conditionals in the specific cases check here
                if (iXYZ(ins) < 0 && gc_needed) { 
                    GC(vm, reg0, curr_fun, instructions);
                } 
                pc += iXYZ(ins); 
                break;
            case If:
                if (!AS_BOOL(vm, reg0[uX(ins)], pc)) {
                    pc++;
                }
                break;
            case Cons:;
                VMNEW(struct VMBlock*, block, vmsize_cons());
                GCVALIDATE(block)->nslots = 2;
                block->slots[0] = reg0[uY(ins)];
                block->slots[1] = reg0[uZ(ins)];
                reg0[uX(ins)] = mkConsValue(block);
                break;
            case LoadLiteral:
                reg0[uX(ins)] = vm->literals[uYZ(ins)];
                break;
            case Getglobal:
                reg0[uX(ins)] = vm->globals[uYZ(ins)];
                break;
            case Setglobal:
                vm->globals[uYZ(ins)] = reg0[uX(ins)];
                break;
            case IDiv: 
                reg0[uX(ins)] = mkNumberValue ((int)AS_NUMBER(vm, reg0[uY(ins)], pc) / 
                                               (int)AS_NUMBER(vm, reg0[uZ(ins)], pc)); 
                break;  
            case Equal:  
                reg0[uX(ins)] = mkBooleanValue(eqvalue(reg0[uY(ins)], reg0[uZ(ins)]));
                break;  
            case Car:  
                reg0[uX(ins)] = AS_CONS_CELL(vm, reg0[uY(ins)], pc)->slots[0];
                break;
            case Cdr:  
                reg0[uX(ins)] = AS_CONS_CELL(vm, reg0[uY(ins)], pc)->slots[1];
                break;
            case Hash:  
                fprintf(stderr, "hash not implemented\n");
                exit(1);
                break;
            case isFun:
                reg0[uX(ins)] = mkBooleanValue((reg0[uY(ins)].tag == VMFunction) |
                                               (reg0[uY(ins)].tag == VMClosure)); 
                break;
            case isPair: 
                reg0[uX(ins)] = mkBooleanValue(reg0[uY(ins)].tag == ConsCell); //def problems here  
                break;   
            case isSym:  
                reg0[uX(ins)] = mkBooleanValue(reg0[uY(ins)].tag == String);
                break; 
            case isNum: 
                reg0[uX(ins)] = mkBooleanValue(reg0[uY(ins)].tag == Number);
                break;                                                                
            case isBool:  
                reg0[uX(ins)] = mkBooleanValue(reg0[uY(ins)].tag == Boolean);
                break; 
            case isNil:  
                reg0[uX(ins)] = mkBooleanValue(reg0[uY(ins)].tag == Nil);
                break; 
            case isNull:  
                reg0[uX(ins)] = mkBooleanValue(reg0[uY(ins)].tag == Emptylist);
                break;
            case Copy:
                reg0[uX(ins)] = reg0[uY(ins)];
                break;
            case Mkclosure:;
                VMNEW(struct VMClosure*, closure, vmsize_closure(uZ(ins)));
                GCVALIDATE(closure)->f = AS_VMFUNCTION(vm, reg0[uY(ins)], pc);
                closure->nslots = uZ(ins);
                reg0[uX(ins)] = mkClosureValue(closure);
                break; 
            case Getclslot:
                {int index = uZ(ins);
                 struct VMClosure* cl = AS_CLOSURE(vm, reg0[uY(ins)], pc);
                 if (index >= cl->nslots) {
                    runerror(vm, pc, "getcl: %dth index of closure DNE", index);
                 }
                 reg0[uX(ins)] = cl->captured[index];
                }
                break;
            case Setclslot:
                {int index = uZ(ins);
                 struct VMClosure* cl = AS_CLOSURE(vm, reg0[uX(ins)], pc);
                 if (index >= cl->nslots) {
                    runerror(vm, pc, "setcl: %dth index of closure DNE", index);
                 }
                 cl->captured[index] = reg0[uY(ins)];
                }
                break; 
            case Call:{
                if (gc_needed) GC(vm, reg0, curr_fun, instructions);
                int destreg = uX(ins);
                int funreg = uY(ins);
                int lastreg = uZ(ins);
                Value callee = reg0[funreg];
                struct VMFunction* newfun;

                if (callee.tag == VMFunction) {
                    newfun = GCVALIDATE(callee.f);
                } else if (callee.tag == VMClosure) {
                    newfun = GCVALIDATE(callee.hof)->f;
                } else { 
                    const char* funname = lastglobalset(vm, funreg, curr_fun, 
                                                        &instructions[pc]); 
                    if (funname != NULL) {
                        runerror(vm, pc, "Tried to call nil; maybe global %s "
                                     "is undefined?", funname);
                    } else {
                        runerror_p(vm, pc, "Tried to call %v as a function", reg0[funreg]);
                    }    
                }

                int arity = newfun->arity;
                int num_args = lastreg - funreg;
                if (num_args != arity) {
                    runerror(vm, pc, "fun %s expected %d args, but got %d args", 
                                nametostr(newfun->funname), arity, num_args);                       
                }

                if (vm->num_activations == STACK_SIZE) {                  
                    runerror(vm, pc, "stack overflow");
                }
                // push the new func on stack, maintain invaraint since we're running it
                struct Activation new_Act = {destreg, reg0, pc + 1, newfun, 0};
                // struct Activation new_Act = {destreg, reg0, pc + 1, curr_fun};
                vm->stack[vm->num_activations++] = new_Act;

                curr_fun = newfun;
                instructions = newfun->instructions;
                reg0 = reg0 + funreg;
                if (reg0 - &vm->registers[0] + newfun->nregs > REG_SIZE) {           
                    runerror(vm, pc, "reg overflow");
                }
                pc = 0;
            }
                continue;
            case Return:{
               if (vm->num_activations == 0) {          
                    runerror(vm, pc, "trying to pop an emptystack");
                }
                /* stacktrace updates*/
                Value return_val = reg0[uX(ins)]; //callee regwindow
                struct Activation callee_Act = vm->stack[--vm->num_activations];
                struct Activation caller_Act = vm->stack[(vm->num_activations - 1)];
                curr_fun = caller_Act.fun;  //restore curr fun, get it from caller's act
                instructions = caller_Act.fun->instructions; 

                reg0 = callee_Act.window_start;      // restore caller reg window
                reg0[callee_Act.dest] = return_val; 
                pc = callee_Act.return_to;                             
            }
                continue;
            case Tailcall:{ //repeated code, needs clean
                if (gc_needed) GC(vm, reg0, curr_fun, instructions);
                int funreg = uX(ins);
                int lastreg = uY(ins);
                Value callee = reg0[funreg];
                struct VMFunction* newfun;

                if (callee.tag == VMFunction) {
                    newfun = GCVALIDATE(callee.f);
                } else if (callee.tag == VMClosure) {
                    newfun = GCVALIDATE(callee.hof)->f;
                } else { 
                    const char* funname = lastglobalset(vm, funreg, curr_fun, 
                                                        &instructions[pc]); 
                     
                    if (funname != NULL) {
                        runerror(vm, pc, "Tried to call nil; maybe global %s "
                                     "is undefined?", funname);
                    } else {
                        runerror_p(vm, pc, "Tried to call %v as a function", reg0[funreg]);
                    }    
                }

                int arity = newfun->arity;
                int num_args = lastreg - funreg;
                if (num_args != arity) {
                    runerror(vm, pc, "fun %s expected %d args, but got %d args", 
                                nametostr(newfun->funname), arity, num_args);                       
                }
                memmove(reg0, reg0+funreg, (num_args + 1) * sizeof(Value));
                /* updating fun while we go, maintain invariant */
                struct Activation top_act = vm->stack[(vm->num_activations - 1)];
                top_act.fun = newfun; 
                if (!istailcall(top_act.tailcall_info)) 
                    top_act.tailcall_info = settailcall();
            
                top_act.tailcall_info = incrementtailcall(top_act.tailcall_info);
                vm->stack[(vm->num_activations - 1)] = top_act;
                curr_fun = newfun;
                instructions = newfun->instructions;
                pc = 0;
            }
                continue;  
            case GC:
                GC(vm, reg0, curr_fun, instructions);
                break;                                                                                        
            default: // not recognized opcode 
                runerror(vm, pc, "opcode %d not recognized", op);
                break;
        }
        pc++;
    }

  // Run code from `fun` until it executes a Halt instruction.
  // Then return.
  return;
}
