(* KNormalizer from First-Order Scheme to KNormal-form Scheme. 
    This is where register allocation happens! *)

(* You'll fill out the missing code in this file *)

structure KNormalize :> sig
  type reg = int  (* register *)
  type regset     (* set of registers *)
  val regname : reg -> string
  val exp : reg Env.env -> regset -> ClosedScheme.exp -> reg KNormalForm.exp
  val def :                          ClosedScheme.def -> reg KNormalForm.exp
end 
  =
struct 
  structure K  = KNormalForm
  structure C  = ClosedScheme
  structure E  = Env
  structure P  = Primitive

  fun fst (x, y) = x
  fun member x = List.exists (fn y => x = y)

  fun eprint s = TextIO.output (TextIO.stdErr, s)

  (************ Register and regset operations ************)

  type reg = int
  fun regname r = "$r" ^ Int.toString r

  datatype regset = RS of int (* RS n represents { r | r >= n } *)
  fun smallest (RS i) = i
  fun op -- (RS i, r) = if (r < i) then RS i else RS (r + 1)
  infix 6 --

  (************ K-normalization ************)

  type exp = reg K.exp
  type policy = regset -> exp -> (reg -> exp) -> exp
    (* puts the expression in an register, continues *)

  fun bindSmallest A e k = 
        let 
            val reg = smallest A
        in 
            K.LET (reg, e, k reg)
        end
  val _ = bindSmallest : regset -> exp -> (reg -> exp) -> exp

  fun bindAnyReg A (K.NAME n) k = k n
    | bindAnyReg A e k = bindSmallest A e k
  val _ =  bindAnyReg : regset -> exp -> (reg -> exp) -> exp

  type 'a normalizer = regset -> 'a -> exp

  fun nbRegsWith normalize bind A xs k =
      (case xs 
         of [] => k []
          | (e::es) => bind A (normalize A e) 
                       (fn t => nbRegsWith normalize bind (A--t) es (fn ts => k (t::ts))))

  val nbRegsWith : 'a normalizer -> policy -> regset -> 'a list -> (reg list -> exp) -> exp
    = nbRegsWith

  fun consVMOPL p lit reg = K.VMOPL (p, [reg], lit)

  fun mutLocal ns e =
  let fun nfoundIn [] n = false
        | nfoundIn (n'::ns) n  = n = n' orelse nfoundIn ns n
      val _ = mutLocal : string list -> C.exp -> bool
  in (case e 
        of (C.LITERAL _) => false
         | (C.LOCAL _) => false
         | (C.GLOBAL _) => false
         | (C.IFX (e1, e2, e3)) => mutLocal ns e1 orelse mutLocal ns e2 orelse mutLocal ns e3
         | (C.PRIMCALL (p, es)) => List.exists (mutLocal ns) es
         | (C.MPRIMCALL (_, p, es)) => List.exists (mutLocal ns) es
         | (C.FUNCALL (e, es)) => List.exists (mutLocal ns) (e::es)
         | (C.MFUNCALL (_, e, es)) => List.exists (mutLocal ns) (e::es)
         | (C.LET (binds, e)) => 
            let val (letNames, es) = ListPair.unzip binds
                val rebound = List.exists (nfoundIn ns) letNames
            in rebound orelse List.exists (mutLocal ns) (e::es)
            end
         | (C.BEGIN es) => List.exists (mutLocal ns) es
         | (C.SETLOCAL (n, e)) => nfoundIn ns n orelse mutLocal ns e
         | (C.SETGLOBAL (n, e)) => mutLocal ns e 
         | (C.WHILEX (e1, e2)) => mutLocal ns e1 orelse mutLocal ns e2
         | (C.CAPTURED _ ) => false
         | (C.CLOSURE _) => false  (* closure should not change captured variables *)
         | (C.LETREC _) => false)
    end

  val inLocalVar : regset -> (reg -> exp) -> exp =
  fn rs => fn k => let val t = smallest rs in K.ASSIGNMENT (t, k t) end

  fun exp rho A e =
    let val exp : reg Env.env -> regset -> ClosedScheme.exp -> exp = exp
        val nbRegs = nbRegsWith (exp rho)   (* normalize and bind in _this_ environment *)
        fun funcode (ns, e) = 
          let
            val (j, nenv) = foldl (fn (n, (i, env)) => (i + 1, E.bind (n, i, env))) 
                            (1, rho) ns 
            val body = exp nenv (RS j) e
          in (map (fn n => E.find (n, nenv)) ns, body)
          end
        val _ = funcode : C.funcode -> reg K.funcode
    in  case e
          of C.PRIMCALL (p, es) => nbRegs bindAnyReg A es (fn ts => K.VMOP (p, ts))
           | C.MPRIMCALL (loc, p, es) => nbRegs bindAnyReg A es (fn ts => K.MVMOP (loc, p, ts))
           | C.LITERAL v => K.LITERAL v
           | C.LOCAL n => K.NAME (E.find (n, rho))
           | C.SETLOCAL (n, e) => K.ASSIGNMENT (E.find (n, rho), exp rho A e)
           | C.GLOBAL n => K.VMOPL (P.getglobal, [], (K.STRING n))  
           | C.SETGLOBAL (n, e) => bindAnyReg A (exp rho A e) 
             (fn t => K.BEGIN (consVMOPL P.setglobal (K.STRING n) t, K.NAME t))
           | C.BEGIN es => 
             let 
               fun nBegin [] = K.LITERAL (K.BOOL false)
                 | nBegin [e] = exp rho A e 
                 | nBegin (e::es) = K.BEGIN (exp rho A e, nBegin es)
             in nBegin es
             end
           | C.IFX (e, e1, e2) => bindAnyReg A (exp rho A e) 
             (fn t => K.IFELSE (t, (exp rho A e1), (exp rho A e2)))
           | C.WHILEX (e, e1) =>
             K.WHILE (smallest A, (exp rho A e), (exp rho A e1))
           | C.FUNCALL (e, es) => bindSmallest A (exp rho A e) 
             (fn t => nbRegs bindSmallest (A--t) es (fn ts => K.FUNCALL (t, ts)))
           | C.MFUNCALL (loc, e, es) => bindSmallest A (exp rho A e) 
             (fn t => nbRegs bindSmallest (A--t) es (fn ts => K.MFUNCALL (loc, t, ts)))
           | C.LET (nelist, e) => 
             let 
                val (ns, es) = ListPair.unzip nelist
                val palias = ListPair.foldl 
                             (fn (n, e, acc) => (case e of C.LOCAL ln => [n, ln] @ acc
                                                         | _ => acc))
                             []
                             (ns, es)
                val muted = true
                (* val muted = mutLocal palias e  *)
                fun rRegs A [] = A 
                  | rRegs A (t::ts) = rRegs (A--t) ts 
                fun nbLetWithBind bind = 
                nbRegs bind A es  
                (fn ts => exp 
                          (ListPair.foldl (fn (n, t, env) => E.bind (n, t, env)) rho (ns, ts)) 
                          (rRegs A ts) 
                          e)
             in
                if muted then nbLetWithBind bindSmallest else nbLetWithBind bindAnyReg
             end
           | (C.CAPTURED i) => K.CAPTURED i 
           | (C.CLOSURE ((formals, body), [])) => 
             let
                val (reglist, regexps) = funcode (formals, body)
             in K.FUNCODE ("(anonymous_fun)", reglist, regexps)
             end
           | (C.CLOSURE ((formals, body), captured)) =>
              inLocalVar A (fn t => nbRegs bindAnyReg (A--t) captured (fn ts =>
                             K.CLOSURE ((funcode (formals, body)), ts)))
           | (C.LETREC  (bs, body)) => 
             let val (ns, cls) = ListPair.unzip bs
                 fun getregs (RS i) 0 = []
                   | getregs (RS i) n = (i :: getregs (RS (i + 1)) (n - 1))
                 val ts = getregs A (List.length bs)
                 val A' = foldl (fn (i, acc) => acc--i) A ts
                 val rho' = ListPair.foldlEq E.bind rho (ns, ts)
                 fun closure ((formals, body), cap) k =
                     nbRegsWith (exp rho') bindAnyReg A' cap (fn ts => k ((funcode (formals, body)), ts))
                 val _ = closure : C.closure -> (reg K.closure -> exp) -> exp
                 fun map' f' [] k = k []
                   | map' f' (x :: xs) k = f' x (fn y => map' f' xs (fn ys => k (y :: ys)))   
             in  map' closure cls (fn kcls => K.LETREC (ListPair.zip (ts, kcls), exp rho' A' body))
             end

    end
  
  fun ntVMOPL e p str = 
      bindAnyReg (RS 0) (exp Env.empty (RS 0) e) (consVMOPL p (K.STRING str))

  fun def (C.EXP e) = exp Env.empty (RS 0) e
    | def (C.CHECK_EXPECT (s1, e1, s2, e2)) = 
      K.BEGIN (ntVMOPL e1 P.check s1, ntVMOPL e2 P.expect s2)
    | def (C.CHECK_ASSERT (s, e)) = ntVMOPL e P.check_assert s
    | def (C.VAL (n, e)) = exp Env.empty (RS 0) (C.SETGLOBAL (n, e))
    | def (C.DEFINE (n, (ns, e))) = 
      let
        val (i, nenv) = foldl (fn (n, (i, env)) => (i + 1, E.bind (n, i, env))) 
                       (0, Env.empty) (n::ns) 
        val body = (exp nenv (RS i) e)
      in 
        bindAnyReg (RS 0) (K.FUNCODE (n, map (fn n => E.find (n, nenv)) ns, body))
        (fn t => consVMOPL P.setglobal (K.STRING n) t)
      end
end
