(* Embeds KNormal-form Scheme into VScheme. This cannot fail. *)

(* You'll complete this file *)

structure KNEmbed :> sig
  val value : KNormalForm.literal -> VScheme.value
  val def   : VScheme.name KNormalForm.exp -> VScheme.def
end 
  = 
struct
  structure K  = KNormalForm
  structure KU = KNormalUtil
  structure S  = VScheme
  structure SU = VSchemeUtils
  structure P  = Primitive

  fun let' x e' e = S.LETX (S.LET, [(x, e')], e)   (* useful helper *)

  fun value (K.INT i) = S.INT i
    | value (K.REAL r) = S.REAL r 
    | value (K.STRING s) = S.SYM s
    | value (K.BOOL b) = S.BOOLV b
    | value K.EMPTYLIST = S.EMPTYLIST
    | value K.NIL = S.BOOLV false

  fun nameFrom (K.STRING s) = s 
    | nameFrom _ = Impossible.impossible "encountered non name"

  fun asSetGlobal (K.VMOPL (vmop, [r], K.STRING n), K.NAME r') = 
      (case (P.name vmop, r = r') 
         of ("setglobal", true) => SOME (n, r) 
         |  _                   => NONE)
    | asSetGlobal _ = NONE
  val _ = asSetGlobal : KU.name K.exp * KU.name K.exp -> (KU.name * KU.name) option

  fun exp (K.LITERAL l) = (S.LITERAL (value l))
    | exp (K.NAME n) = S.VAR n
    | exp (K.VMOP (vmop, alist)) = S.APPLY (S.VAR (P.name vmop), map S.VAR alist)
    | exp (K.MVMOP (_, vmop, alist)) = S.APPLY (S.VAR (P.name vmop), map S.VAR alist)
    | exp (K.VMOPL (vmop, alist, lit)) = 
          (case (P.name vmop, alist, lit)
             of ("getglobal", [], K.STRING n) => S.VAR n
              | ("setglobal", [srcreg], K.STRING n) => S.SET (n, S.VAR srcreg)
              | (name, _, _) => S.APPLY (S.VAR name, (map S.VAR alist) @ [S.LITERAL (value lit)]))
    | exp (K.MVMOPL (_, vmop, alist, lit)) = exp (K.VMOPL (vmop, alist, lit))
    | exp (K.FUNCALL (a, alist)) = S.APPLY (S.VAR a, map S.VAR alist)
    | exp (K.MFUNCALL (_, a, alist)) = S.APPLY (S.VAR a, map S.VAR alist)
    | exp (K.IFELSE (a, e1, e2)) = S.IFX (S.VAR a, exp e1, exp e2)
    | exp (K.LET (a, e1, e2)) = let' a (exp e1) (exp e2)
    | exp (K.BEGIN (e1, e2)) = 
          (case asSetGlobal (e1, e2) 
            of SOME (n, r) => S.SET (n, S.VAR r)
             | NONE => S.BEGIN [exp e1, exp e2])
    | exp (K.ASSIGNMENT (a, e)) = S.SET (a, exp e)
    | exp (K.WHILE (a, e1, e2)) = S.WHILEX (let' a (exp e1) (S.VAR a), exp e2)
    | exp (K.FUNCODE (n, alist, e)) = S.LAMBDA (alist, exp e)
    | exp (K.CLOSURE ((formals, body), captured)) = 
      S.APPLY (S.VAR "mkclosure", [S.LAMBDA (("$closure" :: formals), exp body), 
                                   SU.list (map (fn r => exp (K.NAME r)) captured)])
    | exp (K.CAPTURED i) = S.APPLY (S.VAR "CAPTURED-IN", [S.LITERAL (S.INT i), S.VAR "$closure"])
    | exp (K.LETREC (binds, e)) =
        let val (names, cls) = ListPair.unzip binds
            val embCls = map (fn cl => exp (K.CLOSURE cl)) cls
            val embBinds = ListPair.zip (names, embCls)
        in S.LETX (S.LETREC, embBinds, exp e)
        end

  val _ = exp: VScheme.name KNormalForm.exp -> VScheme.exp

  fun def e = S.EXP (exp e)
end
