(* In K-normal form, convert string names to register names *)

(* You'll write this file *)

structure KNRename :> sig
  val regOfName : string -> ObjectCode.reg Error.error
  val nameOfReg : ObjectCode.reg -> string Error.error
  val mapx : ('a -> 'b Error.error) ->
             ('a KNormalForm.exp -> 'b KNormalForm.exp Error.error)
end
  =
struct
  structure K = KNormalForm

  infix 3 <*>   val op <*> = Error.<*>
  infixr 4 <$>  val op <$> = Error.<$>

  val succeed = Error.succeed
  val errorList = Error.list

  fun curry  f x y   = f (x, y)
  fun curry3 f x y z = f (x, y, z)
  fun curry4 f v x y z = f (v, x, y, z)
  fun curryClosure f x y z = f ((x, y), z)
  fun curryClBody x y z = ((x, y), z)
  fun pair x y = (x, y)


  (* AsmLex.registerNum takes a string starting with "r" followed by a number n
     such that 0 <= n < 256, and returns n *)
  val regOfName = AsmLex.registerNum
  fun nameOfReg r = succeed ("r" ^ Int.toString r)

  fun clRename f ((formals, body), captured) = 
      curryClBody <$> Error.mapList f formals <*> mapx f body <*> Error.mapList f captured
  and mapx f (K.LITERAL l) = succeed (K.LITERAL l)
    | mapx f (K.NAME a)    = K.NAME <$> f a
    | mapx f (K.VMOP (vmop, alist)) = curry K.VMOP vmop <$> Error.mapList f alist
    | mapx f (K.VMOPL (vmop, alist, lit)) = curry3 K.VMOPL vmop <$> Error.mapList f alist <*> succeed lit
    | mapx f (K.FUNCALL (a, alist)) = curry K.FUNCALL <$> f a <*> Error.mapList f alist
    | mapx f (K.IFELSE (a, ae1, ae2)) = curry3 K.IFELSE <$> f a <*> mapx f ae1 <*> mapx f ae2
    | mapx f (K.LET (a, ae1, ae2)) = curry3 K.LET <$> f a <*> mapx f ae1 <*> mapx f ae2
    | mapx f (K.BEGIN (ae1, ae2)) = curry K.BEGIN <$> mapx f ae1 <*> mapx f ae2
    | mapx f (K.ASSIGNMENT (a, ae)) = curry K.ASSIGNMENT <$> f a <*> mapx f ae
    | mapx f (K.WHILE (a, ae1, ae2)) = curry3 K.WHILE <$> f a <*> mapx f ae1 <*> mapx f ae2
    | mapx f (K.FUNCODE (n, alist, ae)) = curry3 K.FUNCODE n <$> Error.mapList f alist <*> mapx f ae
    (* | mapx f (K.FUNCODE (alist, ae)) = curry K.FUNCODE <$> Error.mapList f alist <*> mapx f ae *)
    | mapx f (K.CLOSURE ae) = K.CLOSURE <$> clRename f ae
    | mapx f (K.CAPTURED i) = succeed (K.CAPTURED i)
    | mapx f (K.LETREC (binds, e)) = 
        let val rBinds = Error.mapList (fn (n, cl) => pair <$> f n <*> clRename f cl) binds
        in curry K.LETREC <$> rBinds <*> mapx f e
        end
    | mapx f (K.MVMOP (loc, vmop, alist)) = curry3 K.MVMOP loc vmop <$> Error.mapList f alist
    | mapx f (K.MVMOPL (loc, vmop, alist, lit)) = curry4 K.MVMOPL loc vmop <$> Error.mapList f alist <*> succeed lit
    | mapx f (K.MFUNCALL (loc, a, alist)) = curry3 K.MFUNCALL loc <$> f a <*> Error.mapList f alist
  val _ = mapx : ('a -> 'b Error.error) -> ('a KNormalForm.exp -> 'b KNormalForm.exp Error.error)
  
end
