(* Label elimination: translate assembly code into virtual object code *)

(* You'll complete this file *)

structure Assembler :>
  sig
    val translate : AssemblyCode.instr list -> ObjectCode.instr list Error.error
      (* What can go wrong: An undefined or multiply-defined label *)

  end
  =
struct

  structure A = AssemblyCode
  structure E = Env
  structure O = ObjectCode

  type 'a error = 'a Error.error
  val (succeed, <*>, <$>, >=>) = (Error.succeed, Error.<*>, Error.<$>, Error.>=>)
  val (>>=) = Error.>>=
  infixr 4 <$>
  infix 3  <*>
  infix 2  >=>
  infix 0  >>=
  val fail = Error.ERROR

  fun curry f x y = f (x, y)
  fun curry3 f x y z = f (x, y, z)
  fun curry4 f w x y z = f (w, x, y, z)
  fun flip f x y  = f y x
  fun cons x xs = x :: xs

  (* A "translation" that cannot handle any labels.  You will define a better one *)
  fun old_translate instrs =
    let fun cvt (A.OBJECT_CODE instr)       = Error.OK instr
          | cvt (A.LOADFUNC (r, k, n, instrs)) = curry4 O.LOADFUNC r k n <$> old_translate instrs
          | cvt _                           = Error.ERROR "assembler not implemented"
    in  Error.list (map cvt instrs)
    end

  (* In lab, define `fold`, `lift`, `labelEnv`, `labelElim`, and `translate` here *)
    fun foldrr f acc [] n = acc
    | foldrr f acc (i::is) n = (case i of (A.DEFLABEL _) => f (n, i, (foldrr f acc is n))
                                        | (A.IF_GOTO_LABEL _) => f (n, i, (foldrr f acc is (n + 2)))
                                        | _ => f (n, i, (foldrr f acc is (n + 1))))                                  

  fun fold f acc instrs = foldrr f acc instrs 0
  val _ = fold : (int * AssemblyCode.instr * 'a -> 'a) -> 'a -> AssemblyCode.instr list -> 'a

  (* fun lift f (a, b, (Error.OK v)) = f (a, b, v)
    | lift f (a, b, (Error.ERROR s)) = fail s *)

  fun lift f (a, b, c) = c >>= curry3 f a b
  val _ = lift : ('a * 'b * 'c -> 'c error) -> ('a * 'b * 'c error -> 'c error)                                                             
  

  fun labelEnv instrs = fold (lift (fn (n, i, lenv) => 
                           case i of (A.DEFLABEL l) => 
                            if E.binds (lenv, l) then 
                              fail "duplicate label" 
                            else 
                              Error.OK (E.bind (l, n, lenv)) 
                                   | _              => Error.OK lenv))
                     (Error.OK E.empty)
                     instrs
  val _ = labelEnv : AssemblyCode.instr list -> int E.env error
  
  fun calcGoto n l lenv = succeed ((E.find (l, lenv)) - n - 1) handle (E.NotFound n) => fail (n ^ "not defined")
  fun consGoto n l lenv ilist = curry (op ::) <$> (O.GOTO <$> calcGoto n l lenv) <*> ilist 
  (* move goto constructor maybe *)

  fun translate instrs = 
    let fun labelElim instrs lenv = fold (fn (n, i, ilist) => 
                case i of (A.OBJECT_CODE i) => curry (op ::) <$> succeed i <*> ilist
                        | (A.DEFLABEL l)      => ilist
                        | (A.GOTO_LABEL l)    => consGoto n l lenv ilist
                        | (A.IF_GOTO_LABEL (reg, l)) => curry (op ::) <$> succeed (O.REGS ("if", [reg])) <*> consGoto (n + 1) l lenv ilist
                        | (A.LOADFUNC (r, k, n, instrs)) => curry (op ::) <$> (curry4 O.LOADFUNC r k n <$> translate instrs) <*> ilist)
                (succeed [])
                instrs
    in 
      (labelEnv >=> labelElim instrs) instrs
    end
    
    
end
