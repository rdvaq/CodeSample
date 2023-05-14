# Stack Trace (Module 7 - depth point #9)
 - Stack trace [3 points]. When a run-time error occurs, your code produces 
a stack trace. To make the stack trace useful, you extend the representation 
of FUNCODE to include a string that encodes the function’s name (if known) 
and source-code location (if known). The extension is supported by 
your K-normal form, assembly code (including parser), object code, SVM loader, 
and vmrun. 


## Section 1: Function Name

#### - UFT
- In knf, `FUNCODE` now includes a string that encodes the function's name: 
    -  a new type: `type name = string` (this type is propagated downstream)
    - `FUNCODE of name * 'a list * 'a exp`
    -  The name is obtained from a top-level `DEFINE` form clscheme `(C.DEFINE (name, (ns, e)))`
    -  `FUNCODE` that does not appear at top level is treated as anonymous, and the name field is populated with the place holder `"anonymous_fun"` (users should not define a function with `anonymous_fun` as the name)
- In vs, a new `LOADFUNC` instruction format appears to accommodate the new name field:
    - `LOADFUNC of ObjectCode.reg * int * name * instr list` 
    - The grammar changes accordingly: 
        - `.loadfun <reg> integer {<instr>} el` -> `.loadfun <reg> integer funname {<instr>} el`
    - The parser and unparser also changes (changes can be found in asmparse.sml line 268 & 389)
    - Example:  
        - ` (define square (x) (* x x)) ` generates:
        ``` 
        .loadfun r0 1 square
            r0 := r1 * r1 (loc.scm - 2)
            return r0
        el
- In vo, a similar change takes place:
    - `LOADFUNC of reg * int * name * instr list` 
    - similarly, the grammar changes:
        - `.load reg function arity num_instructions` -> `.load reg function funname arity num_instructions`   
#### - SVM
- The `VMFunction` value gets a new field
    -  ```
        struct VMFunction {
            GCMETA(VMFunction)
            Name funname;  
            ...
        }; 
    -  This value of the function name is obtained at loadtime with an additional `token_get_name` when loading functions
- With this new field, the function name can be retrieved easily 
    - i.e. When a function fails arity check, we don't have to look through the global table for its name
        - ```
            if (num_args != arity) {
                runerror(vm, pc, "fun %s expected %d args, but got %d args", 
                            nametostr(newfun->funname), arity, num_args);                       
            }
## Section 2: Source Code location
#### - UFT
- In the uft, source code location gets its own type `type srcloc = string * int` where the string represents the filename and the int represents the line number.
- Expressions with a source code location now get the type `type 'a located = 'a * srcloc`
- The get the filename, `translate` in `uft.sml` now takes in an extra filename parameter, when `UFT.transalte` is called in 
`main.sml`, the filename is passed in.
- The line number is counted in `IOUtil.sml`. This is also where `srcloc` and `located` is defined. 
    - A new function `locatedlines` is defined and has the signature:
    `val locatedLines  : string (*filename*)-> TextIO.instream -> string located list`
        - Ripples:
            - S-expression parser: `val parse : string located list -> Sx.sx list Error.error`
            - In `UFT.sml`, `schemeOfFile` gets the updated signiture: 
            `string -> instream -> VScheme.def list error`  
            - In `UFT.sml`, all the `LAN_of LAN` functions take in filename as a parameter to pass to the parsing functions
- To work with the new srcloc, a new S-expression is defined in `sx.sml`:
    - `Sx.sx` to acquire value constructor `MARK of sx located`
        ```
        datatype sx
            = INT  of int
            | BOOL of bool
            ...
            | MARK of sx located  <----- New Form
    -  A new helper function `val tokWithLoc : string located -> L.token located list Error.error ` is implemented 
    for `Sx.parse` to keep using the Lexer's token pipeline while satisfying the updated type
- vscheme and vscheme parser are altered to work with `MARK`
    - For now, marks are only used at call sites, and thus, `vscheme` gets the new expression:
        ```
        datatype exp = ...
                    | MAPPLY  of srcloc * exp * exp list
    - To work with this representation, the parser ignores the location in `Sx.MARK` by default (done by updating the `one` parser), and only gets the location for the `MAPPLY` expression
        - This is achieved with 2 helpers:
            - ` val firstMark : srcloc option parser`
            - `val _ = maybeMarkApply : srcloc option -> VScheme.exp -> VScheme.exp list -> VScheme.exp`
        - Resulting in `maybeMarkApply <$> firstMark <*> exp <*> many exp`
- The vscheme `MAPPLY  of srcloc * exp * exp list` generates ripples downstream 
    - In `UnambiguousVScheme` & `ClosedScheme`
        - ```
          datatype exp = ...
                       | MFUNCALL  of srcloc * exp * exp list
                       | MPRIMCALL of srcloc * primitive * exp list 
    - In `KNormalForm`
        - ```
          datatype exp = ...
                       | MVMOP of srcloc * vmop * 'a list
                       | MVMOPL of srcloc * vmop * 'a list * literal
                       | MFUNCALL  of srcloc * exp * exp list
    - In `AssemblyCode` & `ObjectCode`
        - ```
          datatype instr = REGS     of operator * reg list
                         | MREGS    of srcloc * operator * reg list
                         | MREGSLIT of srcloc * operator * reg list * literal
                         ...
    - The related passes between the languages are updated accordingly 
- Changes in  `AssemblyCode` and  `ObjectCode` syntax:
    - In `vs`, the location of an expression follows the expression itself 
        - i.e. `r0 := call r0 (r1, ..., r1) (loc.scm - 4)`
    - In `vo`, the location of an expression precedes the expression and gets a `.at` tag 
        - i.e. `.at loc.scm 4 call 0 0 1`

#### SVM
###### General idea: Assume that the `loader` can parse the new `vo` syntax and get the instruction and its corresponding location. To make good use of the location metadata, we want to know the corresponding location of an instruction that errored at runtime. The location also has to be somewhere on the stack/state, so that when an error occurs, the stack-tracing function can get all the information it needs with just the `vmstate` and the errored instruction. To me, the most intuitive way to store the locations is to store them inside a `VMFunction` where their corresponding instructions are.
- Changes in `VMFunction` value
    - To store the locations of expressions inside the function, the size of the `instructions` array 
    is doubled. The first half (more accurately (half + 1) for the inserted halt) of the instructions array still stores the regular function instructions. The second half stores the corresponding locations. Thus, given an `index` to an instruction, its location will be at `index + size / 2 + 1`.
- Changes to the `loader`
    - Since a single instruction may or may not contain its location, the loader distinguishes between a regular instruction and a located instruction the same way it distinguishes between a `.load` and a regular instruction, by checking the first `Name` returned by `tokens_get_name`. The `get_instruction` function in the loader is extended with a new `dotatname`.
    - Additionally, `get_instruction` now returns a 64 bit word where first 32 bit is the instruction and the last 32bit is a potential location. 
    - location representation in the SVM
        - location is represented as a 32 bit word where the first 16 bits represent an index into the vm's literal pool and the last 16 bits represent the line number
        - when a file name is obtained from parsing the `vo`, it is loaded into the literal pool and its index is packed into a location word
        - `0` represents that no source code location was available, a `no_source_location` constant is defined accordingly. If an instruction is not accompanied by its location, then the `no_source_location` constant is used as the placeholder.
    - The return value of `get_instruction` is unpacked, giving us the instruction and its location. The two values are then placed into the target Instructions array at their respective indexes.
    - All the bitpacking functions related to the loader can be found in `iformat.h`
###### Now that we have the locations stored in the vmfunctions, we can use it during stack trace. The idea is simple, figure out on which instruction the current function errored, get its location, and report it. Then, trace up the stack, find all the call sites of the functions on the stack, and report them. 

- Changes to `Activation` and the call stack
    - To make stack tracing easier, I implement a new invariant in vmrun:
    `INVARIANT: current function is always at the "top" of the stack`
    With this, I don't need to flush the current function to vmstate every time I make a call to `runerror`. This does lead to changes in the `Activation` struct:
        - ```
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
        As documented, the function stored in the activation stack is the callee's function, but the other fields still belong to the caller. This led to changes in `Return`, `Call`, and `Tailcall` implementations.
- Getting the errored instruction and callsites:
    - To get the instruction that caused the error, `typeerror` and `runerror` in `vmerror.h` now take in the program counter as an extra parameter. The change to `typeerror` led to a change in all the projection functions in `value.h`. Now, all the projection functions also take in the program counter as a parameter.
    - To get the call sites of the functions on the stack, the `return_to` parameter of the activation stack is useful. Since we always return to the instruction following the `call` instruction, the call site of the callee is just `return_to - 1`.
- With all the above in mind, functions `void stack_trace(FILE *fp, VMState vm, int pc)` and its helper 
  `output_error(FILE *fp, VMState vm, int error_site, struct VMFunction* fun)` are created. 
    - `output_error` uses the location of the error site to get the location data inside the function passed in. It then prints the `fp` the source code location of the error and the function's name
    - `stack_trace` will first call `output_error` with `pc` and the function on the top of the stack. It will then loops through all the functions on the call stack and can call `output_error` with the call site of the respective functions. 
- Tailcall support 
    - Tailcalls do not push activation records onto the stack, thus, stacktracing will not reveal all the functions that have 
    been called to get to the error site. This might not seem obvious to the user. Thus, the activation record stores an 
    extra 8 bits of information, `uint8_t tailcall_info`, for tailcall-related information.
        - The first bit of the `tailcall_info` word signifies whether a tailcall had occurred or not, and the rest of the bits are used to record the number of tailcalls that occurred.
        - The related bitpacking functions can be found in `vmstack.h`
        - While running `stack_trace`, the function will check the tailcall bit. If it is set, then it will tell the user 
        number of tailcalls that had occurred. 

#### TESTING
- To ensure that the `vs-vs` pass of the new syntax works, one can run `sh runvstest.sh` in `src/dps/m7-9/tests`
    - The test foler contains a host of scheme programs. The script will run the `ho-vs` pass first and then pipe the output
    through the uft's `vs-vs` pass again. Any uft erros encountered will be outputted.
    - To ensure that the `vo` generated from `vs` still works, the output of `ho-vs` makes another pass through the uft (`vs-vo`), and the result is piped to the svm for correctness check
- To ensure that the `uft` & `svm` still works after all the stacktrace modifications, one can run `sh runhotest.sh` in the same directory. This will send the scheme programs through the `ho-vo` pass and piped into the `svm`. Failed tests will be reported.

#### Results 
- In `src/dps/m7-9/demos`, there are a host of files that should error when run. These include arity errors, global undefined errors, type errors, tailcall errors, etc. See the results of stacktrace by running `sh runerr.sh`
    - note: the stackoverflow error file `fact_overflow_ho.notrun` is not run by default, its output can be seen in `overflow_output` (can't turn off stacktrace by a button, the quality of life is not quite there...)






