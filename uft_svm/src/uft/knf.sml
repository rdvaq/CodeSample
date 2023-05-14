(* Representation of KNormal-form, with some utility functions *)

(* You'll define the representation of `'a exp`, and you'll write
   the utility functions. *)

structure KNormalForm = struct
  
  datatype literal = datatype ObjectCode.literal

  type vmop = Primitive.primitive
  type name = string
  type srcloc = string * int

  datatype 'a exp      (* type parameter 'a is a _name_, typically
                          instantiated as `string` or `ObjectCode.reg` *)
    = LITERAL of literal
    | NAME of 'a
    | VMOP of vmop * 'a list
    | MVMOP of srcloc * vmop * 'a list
    | VMOPL of vmop * 'a list * literal
    | MVMOPL of srcloc * vmop * 'a list * literal
    | FUNCALL of 'a * 'a list
    | MFUNCALL of srcloc * 'a * 'a list
    | IFELSE of 'a * 'a exp * 'a exp
    | LET of 'a * 'a exp * 'a exp
    | BEGIN of 'a exp * 'a exp
    | ASSIGNMENT of 'a * 'a exp
    | WHILE of 'a * 'a exp * 'a exp
    | FUNCODE of name * 'a list * 'a exp
    | CLOSURE of 'a closure
    | CAPTURED of int
    | LETREC of ('a * 'a closure) list * 'a exp
    withtype 'a closure = ('a list * 'a exp) * 'a list
                (* the value of every captured variable is now in a register *)
    type 'a funcode = 'a list * 'a exp
end

structure KNormalUtil :> sig
  type name = string
  val setglobal : name * 'a -> 'a KNormalForm.exp
  val getglobal : name -> 'a KNormalForm.exp

   (* create these @(x,...x, v) forms:
         setglobal(register, name-of-global)
         getglobal(name-of-global)
    *)

  (* you could consider adding similar functions for `check`, `expect`,
     and `check-assert` *)
end
  =
struct
  structure K = KNormalForm
  type name = string

  fun setglobal (x, register) = K.VMOPL (Primitive.setglobal, [register], K.STRING x)
  fun getglobal x             = K.VMOPL (Primitive.getglobal, [], K.STRING x)

end
