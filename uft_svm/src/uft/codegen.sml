(* Generates abstract assembly code from KNormal-form VScheme *)

(* You'll write this file *)

structure Codegen
  :>
sig 
  type reg = ObjectCode.reg
  type instruction = AssemblyCode.instr
  val forEffect : reg KNormalForm.exp -> instruction list
end
  =
struct
  structure A = AsmGen
  structure K = KNormalForm
  structure P = Primitive

  type reg = ObjectCode.reg
  type instruction = AssemblyCode.instr

  (********* Join lists, John Hughes (1986) style *********)

  type 'a hughes_list = 'a list -> 'a list
    (* append these lists using `o` *)

  (* don't look at these implementations; look at the types below! *)
  fun empty tail = tail
  fun S e  tail = e :: tail
  fun L es tail = es @ tail

  val _ = empty : 'a hughes_list
  val _ = S     : 'a      -> 'a hughes_list   (* singleton *)
  val _ = L     : 'a list -> 'a hughes_list   (* conversion *)

  val hconcat : 'a hughes_list list -> 'a hughes_list
    = fn xs => foldr op o empty xs

  (************** the code generator ******************)

  (* three contexts for code generation: to put into a register,
     to run for side effect, or (in module 8) to return. *)
  fun translateif reg e1 e2 f = 
        let 
            val l = A.newlabel ()
            val l' = A.newlabel ()
        in
            S (A.ifgoto reg l) o f e2 o L [A.goto l', A.deflabel l] o f e1 o S (A.deflabel l')
        end  
  
 fun lastreg r []       = r
   | lastreg r (x::xs)  = lastreg x xs

 fun translatecall callfun reg reglist  =
     if A.areConsecutive (reg::reglist) then
        S (callfun reg (lastreg reg reglist))
     else
        Impossible.impossible "Non consecutive parameters to functon, bug in uft"

 fun transOpArity p args dest succ = 
     if args = P.arity p then 
       S (succ)
     else 
       S (A.loadlit dest (K.STRING ("wrong number of args to " ^ P.name p))) o 
       S (A.effect P.error [dest])

  val reg0 = 0

  fun mapi f xs =  (* missing from mosml *)
  let fun go k [] = []
        | go k (x::xs) = f (k, x) :: go (k + 1) xs
  in  go 0 xs
  end
  
  fun genClosure f dest formals body cap = 
    let fun currySet (i, x) = A.setclslot dest i x 
    in
        f dest (K.FUNCODE ("(anonymous_fun)", formals, body)) o 
        S (A.mkclosure dest dest (List.length cap)) o 
        L (mapi currySet cap)
    end

  fun letrec gen (bindings, body) =
    let val _ = letrec : (reg K.exp -> instruction hughes_list)
                        -> (reg * reg K.closure) list * reg K.exp
                        -> instruction hughes_list
        (* one helper function to allocate and another to initialize *)
        fun alloc (f_i, closure as (funcode as (formals, body), captures)) =
            toReg' f_i (K.FUNCODE ("(anonymous_fun)", formals, body)) o S (A.mkclosure f_i f_i (List.length captures))   
        fun init  (f_i, closure as (funcode as (formals, body), captures)) = 
            L (mapi (fn (i, x) => A.setclslot f_i i x) captures)
    in  hconcat (map alloc bindings) o hconcat (map init bindings) o gen body
    end
  and toReg' (dest : reg) (e : reg KNormalForm.exp) : instruction hughes_list =
      (case e of K.LITERAL l => S (A.loadlit dest l)
               | K.NAME r => S (A.copyreg dest r)
               | K.VMOP (P.SETS_REGISTER b, reglist) => 
                   transOpArity (P.SETS_REGISTER b) (List.length reglist) dest 
                   (A.setreg dest (P.SETS_REGISTER b) reglist)
               | K.VMOP _ => forEffect' e
               | K.VMOPL (P.SETS_REGISTER b, reglist, lit) => 
                   transOpArity (P.SETS_REGISTER b) (List.length reglist + 1) dest 
                   (A.setregLit dest (P.SETS_REGISTER b) reglist lit)
               | K.VMOPL _ => forEffect' e
               | K.FUNCALL (reg, reglist) => translatecall (A.call dest) reg reglist
               | K.IFELSE (reg, e1, e2) => translateif reg e1 e2 (toReg' dest)
               | K.LET (reg, e1, e2) => toReg' reg e1 o toReg' dest e2
               | K.BEGIN (e1, e2) => forEffect' e1 o toReg' dest e2
               | K.ASSIGNMENT (reg, e) => toReg' reg e o S (A.copyreg dest reg) 
               | K.WHILE _ => forEffect' e o S (A.loadlit dest (ObjectCode.BOOL false))
               | K.FUNCODE (name, reglist, e) => S (A.loadfunc dest (List.length reglist) name (toReturn' e []))
               | K.CLOSURE ((formals, body), cap) => genClosure toReg' dest formals body cap
               | K.CAPTURED i => S (A.captured dest i)
               | K.LETREC e => letrec (toReg' dest) e
               | K.MVMOP (loc, P.SETS_REGISTER b, reglist) => 
                    transOpArity (P.SETS_REGISTER b) (List.length reglist) dest 
                    (A.msetreg loc dest (P.SETS_REGISTER b) reglist)
               | K.MVMOPL (loc, P.SETS_REGISTER b, reglist, lit) => 
                   transOpArity (P.SETS_REGISTER b) (List.length reglist + 1) dest 
                   (A.msetregLit loc dest (P.SETS_REGISTER b) reglist lit)               
               | K.MVMOP _ => forEffect' e
               | K.MVMOPL _ => forEffect' e
               | K.MFUNCALL (loc, reg, reglist) => translatecall (A.mcall loc dest) reg reglist)
  and forEffect' (e: reg KNormalForm.exp) : instruction hughes_list  =
      (case e of K.LITERAL _ => empty
               | K.NAME _ => empty
               | K.VMOP (P.HAS_EFFECT b, reglist) => 
                   transOpArity (P.HAS_EFFECT b) (List.length reglist) reg0 
                   (A.effect (P.HAS_EFFECT b) reglist) 
               | K.VMOP _ => empty
               | K.VMOPL (P.HAS_EFFECT b, reglist, lit) => 
                   transOpArity (P.HAS_EFFECT b) (List.length reglist + 1) reg0 
                   (A.effectLit (P.HAS_EFFECT b) reglist lit)
               | K.VMOPL _ => empty
               | K.FUNCALL (reg, reglist) => translatecall (A.call reg) reg reglist
               | K.IFELSE (reg, e1, e2) => translateif reg e1 e2 forEffect' (*extra instruction here, eliminate!!!!*)
               | K.LET (reg, e1, e2) => toReg' reg e1 o forEffect' e2 
               | K.BEGIN (e1, e2) => forEffect' e1 o forEffect' e2
               | K.ASSIGNMENT (reg, e) => toReg' reg e
               | K.WHILE (reg, e1, e2) => 
                 let 
                    val l = A.newlabel ()
                    val l' = A.newlabel ()
                 in 
                    L [A.goto l, A.deflabel l'] o forEffect' e2 o S (A.deflabel l) o toReg' reg e1 o S (A.ifgoto reg l')
                 end
               | K.FUNCODE _ => empty
               | K.CLOSURE _ => empty
               | K.CAPTURED i => empty 
               | K.LETREC e => letrec forEffect' e
               | K.MVMOP (loc, P.HAS_EFFECT b, reglist) => 
                   transOpArity (P.HAS_EFFECT b) (List.length reglist) reg0 
                   (A.meffect loc (P.HAS_EFFECT b) reglist)
               | K.MVMOPL (loc, P.HAS_EFFECT b, reglist, lit) => 
                   transOpArity (P.HAS_EFFECT b) (List.length reglist + 1) reg0 
                   (A.meffectLit loc (P.HAS_EFFECT b) reglist lit)               
               | K.MVMOP _ => empty
               | K.MVMOPL _ => empty
               | K.MFUNCALL (loc, reg, reglist) => translatecall (A.mcall loc reg) reg reglist)
  and toReturn' (e:  reg KNormalForm.exp) : instruction hughes_list  =
      (case e of K.LITERAL _ => toReg' reg0 e o S (A.return reg0) 
               | K.NAME reg => S (A.return reg)
               | K.VMOP _ => toReg' reg0 e o S (A.return reg0)
               | K.VMOPL _ => toReg' reg0 e o S (A.return reg0)
               | K.FUNCALL (reg, reglist) => translatecall A.tailcall reg reglist
               | K.IFELSE (reg, e1, e2) => translateif reg e1 e2 toReturn'
               | K.LET (reg, e1, e2) => toReg' reg e1 o (toReturn' e2)
               | K.BEGIN (e1, e2) => forEffect' e1 o (toReturn' e2)
               | K.ASSIGNMENT (reg, e) => toReg' reg e o S (A.return reg)
               | K.WHILE _ => toReg' reg0 e o S (A.return reg0)
               | K.FUNCODE _ => toReg' reg0 e o S (A.return reg0)
               | K.CLOSURE _ => toReg' reg0 e o S (A.return reg0)
               | K.CAPTURED _ => toReg' reg0 e o S (A.return reg0)
               | K.LETREC e => letrec toReturn' e
               | K.MVMOP _ => toReg' reg0 e o S (A.return reg0)
               | K.MVMOPL _ => toReg' reg0 e o S (A.return reg0)
               | K.MFUNCALL (loc, reg, reglist) => translatecall (A.mtailcall loc) reg reglist)
    

  val _ = forEffect' :        reg KNormalForm.exp -> instruction hughes_list
  val _ = toReg'     : reg -> reg KNormalForm.exp -> instruction hughes_list

  fun forEffect e = forEffect' e []

end
