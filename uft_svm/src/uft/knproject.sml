(* Project disambiguated VScheme into KNormal representation. 
    Note that this can fail if the disambiguated VScheme is not already 
    written in KNormal-form. *)

(* You'll complete this file *)

structure KNProject :> sig
  val value : UnambiguousVScheme.value -> KNormalForm.literal
  val def   : UnambiguousVScheme.def -> string KNormalForm.exp Error.error
end 
  = 
struct
  structure K  = KNormalForm
  structure KU = KNormalUtil
  structure P  = Primitive
  structure X  = UnambiguousVScheme

  infix  3 <*>  val op <*> = Error.<*>
  infixr 4 <$>  val op <$> = Error.<$>
  val succeed = Error.succeed
  val error = Error.ERROR
  val errorList = Error.list

  fun curry  f x y   = f (x, y)
  fun curry3 f x y z = f (x, y, z)

  fun checky p = P.name p = "check" orelse P.name p = "expect"

  val asName : X.exp -> X.name Error.error
         (* project into a name; used where KNF expects a name *)
    = fn X.LOCAL x => succeed x 
       | e => error ("expected a local variable but instead got " ^ (X.whatIs e))

  fun value (X.SYM n) = K.STRING n 
    | value (X.INT i) = K.INT i 
    | value (X.REAL r) = K.REAL r  
    | value (X.BOOLV b) = K.BOOL b
    | value (X.EMPTYLIST) = K.EMPTYLIST

  (* val exp   : UnambiguousVScheme.exp -> string KNormalForm.exp Error.error *)
  fun exp (X.LITERAL v) = succeed (K.LITERAL (value v))
    | exp (X.LOCAL n) = succeed (K.NAME n)
    | exp (X.GLOBAL n) = succeed (KU.getglobal n)
    | exp (X.SETLOCAL (n, e)) = curry K.ASSIGNMENT n <$> exp e
    | exp (X.SETGLOBAL (n, e)) = (curry K.BEGIN o curry KU.setglobal n) <$> asName e <*> exp e 
    (* | exp (X.SETGLOBAL (n, e)) = curry K.BEGIN <$> (curry KU.setglobal n <$> asName e) <*> exp e *)
    | exp (X.IFX (e1, e2, e3)) = curry3 K.IFELSE <$> asName e1 <*> exp e2 <*> exp e3
    | exp (X.WHILEX (X.LETX (X.LET, [(n, e')], X.LOCAL n'), e2)) = 
           if n = n' then 
              curry3 K.WHILE n <$> exp e' <*> exp e2
           else 
              error ("In while let " ^ n ^ " and " ^ n' ^ " is different")
    | exp (X.BEGIN [e1, e2]) = curry K.BEGIN <$> exp e1 <*> exp e2
    | exp (X.FUNCALL (e, exps)) = curry K.FUNCALL <$> asName e <*> errorList (map asName exps)
    | exp (X.PRIMCALL (p, exps)) = 
           if checky p then
             case exps of [X.LOCAL reg, X.LITERAL l] => succeed (K.VMOPL (p, [reg], value l))
                        | _ => error ("malformed check or expect")
           else 
            curry K.VMOP p <$> errorList (map asName exps)
    | exp (X.LETX (X.LET, [(n, e)], e2)) = curry3 K.LET n <$> exp e <*> exp e2
    | exp (X.LAMBDA (ns, e)) = curry3 K.FUNCODE "(anonymous_fun)" ns <$> exp e (*since users cannot name a funtion '(...)' *)
    | exp e = error ("cant convert " ^ (X.whatIs e) ^ " into KN form")


  fun def (X.EXP e) = exp e 
    | def (X.DEFINE (n, (ns, e))) = (curry3 K.LET "r0" o curry3 K.FUNCODE n ns) <$> exp e <*> exp (X.SETGLOBAL (n, X.LOCAL "r0"))
    | def _ = error ("def not in KN form")

end


