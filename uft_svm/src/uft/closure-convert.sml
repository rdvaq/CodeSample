(* Closure conversion from unambiguous VScheme to Closed Scheme. 
    This is where we handle lambda and captured variables *)

(* You'll write this file *)

structure ClosureConvert :> sig
  val close : UnambiguousVScheme.def -> ClosedScheme.def
end 
= 
struct
  structure X = UnambiguousVScheme
  structure C = ClosedScheme
  structure S = Set

  fun literal (X.SYM x)   = C.STRING x
    | literal (X.INT i)   = C.INT i
    | literal (X.REAL i)  = C.REAL i
    | literal (X.BOOLV b) = C.BOOL b
    | literal X.EMPTYLIST = C.EMPTYLIST

  fun free exp = 
    let fun flist es = S.union' (foldl (fn (e, acc) => (free e)::acc) [] es)
        val _ = free : X.exp -> X.name S.set
    in
      case exp 
          of (X.LITERAL _) => S.empty
           | (X.LOCAL n) => S.ofList [n]
           | (X.GLOBAL _) => S.empty
           | (X.SETLOCAL (n, e)) => S.union' [S.ofList [n], free e]
           | (X.SETGLOBAL _) => S.empty
           | (X.IFX (e, e1, e2)) => S.union' [free e, free e1, free e2]
           | (X.WHILEX (e1, e2)) => S.union' [free e1, free e2]
           | (X.BEGIN es) => flist es
           | (X.FUNCALL (e, es)) => flist (e::es)
           | (X.MFUNCALL (_, e, es)) => flist (e::es)
           | (X.PRIMCALL (p, es)) => flist es
           | (X.MPRIMCALL (_, p, es)) => flist es
           | (X.LETX (lk, bs, e)) =>
               let val (ns, es) = ListPair.unzip bs
                   val bnames = S.ofList ns
                   val fbound = case lk of X.LET => flist es
                                         | X.LETREC => S.diff (flist es, bnames)
                   val fbody = S.diff (free e, bnames)
               in S.union' [fbound, fbody]
               end 
           | (X.LAMBDA (ns, e)) => S.diff (free e, S.ofList ns) 
    end

  fun indexOf x xs = 
    (* returns `SOME i`, where i is the position of `x` in `xs`,
       or if `x` does not appear in `xs`, returns `NONE` *)
    let fun find k []        = NONE 
          | find k (y :: ys) = if x = y then SOME k else find (k + 1) ys
    in  find 0 xs
    end

  fun closeExp captured e =
    (* Given an expression `e` in Unambiguous vScheme, plus a list
       of the free variables of that expression, return the closure-
       converted version of the expression in Closed Scheme *)
    let val _ = closeExp : X.name list -> X.exp -> C.exp

        (* I recommend internal function closure : X.lambda -> C.closure *)
        fun closure (xs, body) = 
            let val freeVars = S.elems (free (X.LAMBDA (xs, body)))
                val cap = map (fn v => closeExp captured (X.LOCAL v)) freeVars
            in
              ((xs, closeExp freeVars body), cap)         
            end
    
        val _ = closure : X.lambda -> C.closure

        fun fnWithContin n f g = case indexOf n captured
                                   of NONE => f
                                    | SOME i => g i
        (* I recommend internal function exp : X.exp -> C.exp *)
        fun exp e = 
            case e 
              of (X.LITERAL v) => C.LITERAL (literal v)
               | (X.LOCAL n) => fnWithContin n (C.LOCAL n) C.CAPTURED
               | (X.GLOBAL n) => C.GLOBAL n
               | (X.SETLOCAL (n, e)) => fnWithContin n (C.SETLOCAL (n, exp e)) 
                                        (fn i => Impossible.impossible "trying to set captured variable")    
               | (X.SETGLOBAL (n, e)) => C.SETGLOBAL (n, exp e)        
               | (X.IFX (e, e1, e2)) => C.IFX (exp e, exp e1, exp e2)
               | (X.WHILEX (e1, e2)) => C.WHILEX (exp e1, exp e2)
               | (X.BEGIN es) => C.BEGIN (map exp es) 
               | (X.FUNCALL (e, es)) => C.FUNCALL (exp e, map exp es)
               | (X.MFUNCALL (loc, e, es)) => C.MFUNCALL (loc, exp e, map exp es)
               | (X.PRIMCALL (p, es)) => C.PRIMCALL (p, map exp es)      
               | (X.MPRIMCALL (loc, p, es)) => C.MPRIMCALL(loc, p, map exp es)
               | (X.LETX (X.LET, bs, e)) => 
                 let val (ns, es) = ListPair.unzip bs
                 in C.LET (ListPair.zip (ns, map exp es), exp e)   
                 end
               | (X.LETX (X.LETREC, bs, e)) => 
                 let val (ns, es) = ListPair.unzip bs
                     fun unLambda (X.LAMBDA lambda) = lambda
                       | unLambda _ = Impossible.impossible "parser failed to insist on a lambda"
                     val lams = map unLambda es
                 in C.LETREC ((ListPair.zipEq (ns, map closure lams)), exp e)
                 end
               | (X.LAMBDA (ns, e)) => C.CLOSURE (closure (ns, e))
    in  exp e
    end

  fun tlConvert e = closeExp [] e
  fun close def = 
      case def 
        of (X.VAL (n, e)) => C.VAL (n, tlConvert e)
         | (X.DEFINE (n, (xs, e))) => C.DEFINE (n, (xs, tlConvert e))
         | (X.EXP e) => C.EXP (tlConvert e)
         | (X.CHECK_EXPECT (s1, e1, s2, e2)) => C.CHECK_EXPECT (s1, tlConvert e1, s2, tlConvert e2)
         | (X.CHECK_ASSERT (s, e)) => C.CHECK_ASSERT (s, tlConvert e)

end
