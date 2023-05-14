(* Interface to assembly language for the convenience of other translations *)

(* You'll use this interface periodically in modules 6 to 10:

    - You'll use the interface to write your code generator
    - You'll implement `copyreg` in a way that works for your SVM
    - In module 8, you'll add support for function calls
    - In module 10, you'll add support for closures 

 *)


structure AsmGen :> sig
  type reg = ObjectCode.reg          
  type instruction = AssemblyCode.instr
  type label
  type srcloc = string * int

  type vmop = Primitive.primitive      (* identifies a VM operator, like + or / *)
  type literal = ObjectCode.literal    (* we care only about numbers and strings *)

  val newlabel : unit -> label  (* fresh every time *)

  (********* instructions that can be generated ******)

  (* simple instructions like cons, +, <; destination register on left *)
  val copyreg   : reg -> reg -> instruction   (* assignment/copy/move *)
  val setreg    : reg -> vmop -> reg list -> instruction
  val setregLit : reg -> vmop -> reg list -> literal -> instruction

  (* effects like setglobal, check, and expect *)
  val effect    : vmop -> reg list -> instruction
  val effectLit : vmop -> reg list -> literal -> instruction
    
  (* location support *)
  val msetreg    : srcloc -> reg -> vmop -> reg list -> instruction
  val msetregLit : srcloc -> reg -> vmop -> reg list -> literal -> instruction 
  val meffect    : srcloc -> vmop -> reg list -> instruction
  val meffectLit : srcloc -> vmop -> reg list -> literal -> instruction
  val mcall      : srcloc -> reg -> reg -> reg -> instruction
  val mtailcall  : srcloc -> reg -> reg -> instruction
  (* control-flow instructions *)
  val goto     : label -> instruction
  val ifgoto   : reg -> label -> instruction
  val deflabel : label -> instruction  (* labels the next instruction *)

  (* function loading *)
  val loadfunc : reg -> int -> string -> instruction list -> instruction

  (* special cases of the code above *)
  val loadlit   : reg -> literal -> instruction   (* rX := LIT     *)
  val getglobal : reg -> literal -> instruction   (* rX := global LIT *)
  val setglobal : literal -> reg -> instruction   (* global LIT := rX *)

  (* call instruction assumes fun and argument in consecutive registers *)
  val call : reg -> reg -> reg -> instruction
                     (* dest, fun, last *)
  val areConsecutive : reg list -> bool
  val return : reg -> instruction
  val tailcall :  reg -> reg -> instruction
                     (* fun, last *) 

  (* closures  *)
  val mkclosure : reg -> reg -> int -> instruction
    (* x := new closure with k slots; x‑>f := y; *)
  val setclslot : reg -> int -> reg -> instruction
    (* x.k := y *)
  val getclslot : reg -> reg -> int -> instruction
    (* x := y.k *)
  val captured : reg -> int -> instruction
end
  =
struct
  structure A = AssemblyCode
  structure O = ObjectCode
  structure P = Primitive

  type reg = ObjectCode.reg
  type instruction = AssemblyCode.instr
  type label = string            
  type vmop = Primitive.primitive
  type literal = ObjectCode.literal
  type srcloc = string * int


  (***** label generation ****)

  local
    val labelsSupplied = ref 0
  in
    fun newlabel () =
      let val n = 1 + !labelsSupplied
          val () = labelsSupplied := n
      in  "L" ^ (Int.toString n)
      end
  end

  exception InternalError of string
  fun fail s = raise InternalError s

  (***** sanity checking for correct use of primitives ****)

  fun asValue p =
    case p
      of P.HAS_EFFECT _ => fail ("primitive " ^ P.name p ^ " used for value")
       | P.SETS_REGISTER _ => P.name p

  fun asEffect p =
    case p
      of P.SETS_REGISTER _ => fail ("primitive " ^ P.name p ^ " used for effect")
       | P.HAS_EFFECT _ => P.name p


  fun i con arg = A.OBJECT_CODE (con arg)
  fun regs opr rs = i O.REGS (opr, rs)


  fun setreg    dest operator args   = i O.REGS    (asValue operator, dest::args)
  fun setregLit dest operator args v = i O.REGSLIT (asValue operator, dest::args, v)
  fun effect         operator args   = i O.REGS    (asEffect operator, args)
  fun effectLit      operator args v = i O.REGSLIT (asEffect operator, args, v)

  fun msetreg    loc dest operator args   = i O.MREGS    (loc, asValue operator, dest::args)
  fun msetregLit loc dest operator args v = i O.MREGSLIT (loc, asValue operator, dest::args, v)
  fun meffect    loc operator args        = i O.MREGS    (loc, asEffect operator, args)
  fun meffectLit loc operator args v      = i O.MREGSLIT (loc, asEffect operator, args, v)

  fun goto label = A.GOTO_LABEL label
  fun deflabel l = A.DEFLABEL l
  fun ifgoto reg l = A.IF_GOTO_LABEL (reg, l)

  fun loadfunc r k n body = A.LOADFUNC (r, k, n, body)

  fun loadlit r v = setregLit r P.loadliteral [] v

  fun getglobal dest  name  = setregLit dest P.getglobal []    name
  fun setglobal name  reg   = effectLit      P.setglobal [reg] name
  
  fun copyreg dest src = regs "copy" [dest, src]

  fun call dest function lastarg = regs "call" [dest, function, lastarg]
  fun tailcall  function lastarg = regs "tailcall" [function, lastarg]
  fun return r   = i O.REGS ("return", [r])

  fun mcall loc dest function lastarg = i O.MREGS (loc, "call", [dest, function, lastarg])
  fun mtailcall loc function lastarg  = i O.MREGS (loc, "tailcall", [function, lastarg])

  val rec areConsecutive : ObjectCode.reg list -> bool
    = fn []  => true
       | [n] => true
       | n :: m :: ms => m = n + 1 andalso areConsecutive (m :: ms)
 
  fun regint operator r1 r2 n = i O.REGINT (operator, r1, r2, n)

  fun mkclosure dest func len = regint "mkclosure" dest func len
  fun setclslot cl index value = regint "setclslot" cl value index
  fun getclslot dest cl index = regint "getclslot" dest cl index

  fun captured dest index = getclslot dest 0 index
end
