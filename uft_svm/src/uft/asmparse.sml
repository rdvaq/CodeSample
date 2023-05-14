(* A parser for assembly language *)

(* You'll get a partially complete version of this file, 
  which you'll need to complete. *)

structure AsmParse :>
  sig
    type line = string (* one line of assembly code *)
    val parse   : AsmLex.token list  list -> AssemblyCode.instr list Error.error
    val unparse : AssemblyCode.instr list -> line list
  end
  =
struct
  (* visualize list of tokens using Unicode middle dot as separator *)
  fun showTokens ts = "[" ^ String.concatWith "\194\183" (map AsmLex.unparse ts) ^ "]"

  structure P = MkListProducer (val species = "parser"
                                type input = AsmLex.token
                                val show = showTokens
                               )
      (* P for parser; builds a module that takes AsmLex.token as input *)

  structure L = AsmLex
  structure A = AssemblyCode
  structure O = ObjectCode

  type line = string (* one line of assembly code *)
  type srcloc = string * int
  type 'a located = 'a * srcloc
  (* Operations on producers: Wishing for Modula-style FROM IMPORT here ... *)
  infix 3 <*>      val op <*> = P.<*>
  infixr 4 <$>     val op <$> = P.<$>
  infix 3 <~>      val op <~> = P.<~>
  infix 1 <|>      val op <|> = P.<|>
  infix 3 >>        val op >> = P.>>

  val succeed = P.succeed
  val curry = P.curry
  val curry3 = P.curry3
  val id = P.id
  val fst = P.fst
  val snd = P.snd
  val many = P.many
  val many1 = P.many1
  val sat = P.sat
  val one = P.one
  val notFollowedBy = P.notFollowedBy
  val eos = P.eos
  fun flip f x y = f y x
  fun pair x y = (x, y)
  fun trip x y z = (x, y, z)

  (* utilities *)

  fun eprint s = TextIO.output (TextIO.stdErr, s)

  type 'a parser = 'a P.producer

  (* always-error parser; useful for messages *)
  fun expected what =
    let fun bads ts = Error.ERROR ("looking for " ^ what ^
                                   ", got this input: " ^ showTokens ts)
    in  P.check ( bads <$> many one )
    end

  (* always-succeed parser; prints msg when run *)
  fun debug msg =
      P.ofFunction (fn ts => (app eprint ["@> ", msg, "\n"]; SOME (Error.OK (), ts)))

  (* make another parser chatter on entry *)
  val verbose : string -> 'a parser -> 'a parser
   = (fn msg => fn p => debug msg >> p)

  val veryVerbose : string -> 'a parser -> 'a parser
      = (fn what => fn p =>
           let fun shout s = app eprint ["looking for ", what, s, "\n"]
           in  P.ofFunction (fn ts =>
                                let val _ = shout "..."
                                    val answer = P.asFunction p ts
                                    val _ =
                                        case answer
                                          of NONE => shout ": failed"
                                           | SOME (Error.ERROR _, _) => shout ": errored"
                                           | SOME (Error.OK _, _) => shout ": succeeded"
                                in  answer
                                end)
           end)

  (****************************************************************************)

  (**** parsers for common tokens ****)

      (* These are your workhorse parsers---the analog of the `get`
         functions from the `tokens.h` interface in the SVM *)

  val int       = P.maybe (fn (L.INT   n)    => SOME n  | _ => NONE) one
  val name      = P.maybe (fn (L.NAME  n)    => SOME n  | _ => NONE) one
  val string    = P.maybe (fn (L.STRING s)   => SOME s  | _ => NONE) one
  val reg       = P.maybe (fn (L.REGISTER n) => SOME n  | _ => NONE) one

  fun token t = sat (P.eq t) one >> succeed () (* parse any token *)
  val eol = token L.EOL
  (* turn any single-token string into a parser for that token *)
  fun the "\n" = eol
    | the s =
        case AsmLex.tokenize s
          of Error.OK [t, AsmLex.EOL] => sat (P.eq t) one >> succeed ()
           | _ => (app eprint ["fail: `", s, "`\n"]; Impossible.impossible "non-token in assembler parser")

  val commaSep : 'a parser -> 'a list parser
    = (fn p => curry (op ::) <$> p <*> many (p <~> the ",") <|> succeed [])

  (***** instruction-building functions for parsers ****)

  fun regs operator operands = A.OBJECT_CODE (O.REGS (operator, operands))
  fun regslit operator registers literal = 
    A.OBJECT_CODE (O.REGSLIT (operator, registers, literal)) 
  fun regint operator reg reg2 value =
    A.OBJECT_CODE (O.REGINT (operator, reg, reg2, value))
  fun setglob operator global regs = A.OBJECT_CODE (O.REGSLIT (operator, regs, global))
  fun ifgoto reg label = A.IF_GOTO_LABEL (reg, label)
  (* srcloc support *)
  fun mregs operator operands loc = A.OBJECT_CODE (O.MREGS (loc, operator, operands))
  fun mregslit operator registers literal loc= 
    A.OBJECT_CODE (O.MREGSLIT (loc, operator, registers, literal))
     (* curried instruction builder *)

  fun eR0 operator          = regs operator []
  fun eR1 operator r1       = regs operator [r1]
  fun eR2 operator r1 r2    = regs operator [r1, r2]
  fun eR3 operator r1 r2 r3 = regs operator [r1, r2, r3]
  fun eR1U16 operator r lit = regslit operator [r] lit
  fun esg operator glo r    = setglob operator glo [r]

  (* hella jank as kresten would say *)
  fun callnoarg operator r1 r2 mloc = 
    case mloc 
    of SOME srcloc => mregs operator [r1, r2, r2] srcloc
    | NONE => regs operator [r1, r2, r2]
  fun tailcallnoarg operator r1 mloc = 
    case mloc 
    of SOME srcloc => mregs operator [r1, r1] srcloc
    | NONE => regs operator [r1, r1]    
  fun setcl operator r1 v r2 = regint operator r1 r2 v

  (* srcloc support *)
  val location = P.optional (pair <$> (the "(" >> name <~> the "-") <*> (int <~> the ")"))
  fun meR3 operator r1 r2 r3 mloc =
    case mloc 
      of SOME srcloc => mregs operator [r1, r2, r3] srcloc
       | NONE => regs operator [r1, r2, r3]   
  fun meR2 operator r1 r2 mloc =
    case mloc 
      of SOME srcloc => mregs operator [r1, r2] srcloc
       | NONE => regs operator [r1, r2]  
  fun meR1 operator r1 mloc =
    case mloc 
      of SOME srcloc => mregs operator [r1] srcloc
       | NONE => regs operator [r1]                       
  (***** Example parser for you to extend *****)

  (* The example parser includes "passthrough" syntax and three
     demonstration instructions:

        - Add two registers, put result in a third

        - Add a small immediate constaint to a register,
          or subtract a small immediate constant from a register,
          put result in a third.

        - Swap the contents of two registers.

     The latter two demonstrations show some more sophisticated
     parsing techniques.
   *)
                      

  (* Swap uses standard multiple-assignment syntax:  $r9, $r33 := $r33, $r9
     Helper function `swap` ensures that register numbers't match.
   *)
  fun swap r1 r2 r3 r4 =
    if r1 = r4 andalso r2 = r3 then
        Error.OK (eR2 "swap" r1 r2)
    else
        Error.ERROR "multiple assignment is allowed only for register swaps"

  (* The add-immediate instruction uses "offset coding": a byte
     in the range 0..255 is converted to signed by subtracting 128.
   *)
  val offset_code : int -> int Error.error =
    fn n => if n >= ~128 andalso n < 128 then Error.OK (n + 128)
            else Error.ERROR ("small integer " ^ Int.toString n ^
                              " out of range -128..127")

  (* To use offset coding, I define a version of <$> that checks for errors *)
  infixr 4 <$>!
  fun f <$>! p = P.check (f <$> p)

  (* Example parser: reads an instruction /without/ reading end of line *)
  val parse_litnames = P.maybe
      (fn (L.NAME n) => (case n of "true" => (SOME (O.BOOL true))
                                 | "false" => (SOME (O.BOOL false))
                                 | "emptylist" => (SOME (O.EMPTYLIST))
                                 | "nil" => (SOME (O.NIL))
                                 | _ => NONE)
       | _           => NONE)  
      one
  val glob_parser = O.STRING <$> (curry3 String.extract <$> 
      sat (String.isPrefix "%") name <*> succeed 1 <*> succeed NONE) (* ASK hella jank *)
  val lit_parser = O.INT <$> int <|> O.STRING <$> string <|> parse_litnames 
  val arg_parser = the "(" >> reg >> the "," >> the "..." >> the "," >> reg <~> the ")"
                

  fun three_reg_assign operator = meR3 operator <$> reg <~> the ":=" <*> reg <~> the operator <*> reg <*> location
  fun two_reg_assign operator = meR2 operator <$> reg <~> the ":=" <~> the operator <*> reg <*> location
  fun one_reg_effect operator = meR1 operator <$> (the operator >> reg) <*> location


  val binop = ["+", "*", "-", "/", "cons", "idiv", ">", "<", "==", "="];

  val unop  = ["pbool", "!", "not", "car", "cdr", "hash", "function?", "pair?", 
               "symbol?", "number?", "boolean?", "null?", "nil?"];

  val eftop = ["print", "println", "printu", "if", "error", "return"];

  val one_line_instr : A.instr parser
     =  the "@" >> regs <$> name <*> many int  (* "passthrough" syntax *)
    <|> foldl (op <|>) P.pzero (map three_reg_assign binop)
    <|> meR3 "+imm" <$> reg <~> the ":=" <*> reg <~> the "+" <*> (offset_code <$>! int) <*> location
    <|> meR3 "+imm" <$> reg <~> the ":=" <*>
                       reg <~> the "-" <*> ((offset_code o ~) <$>! int) <*> location
                                           (* the ~ is ML's unary minus (negation) *)
    <|> P.check
        (swap <$> reg <~> the "," <*> reg <~> the ":=" <*> reg <~> the "," <*> reg)
    <|> eR1U16 "loadliteral" <$> reg <~> the ":=" <*> lit_parser
    <|> eR1U16 "getglobal" <$> reg <~> the ":=" <*> glob_parser
    <|> esg "setglobal" <$> glob_parser <~> the ":=" <*> reg
    <|> eR1U16 "check" <$> (the "check" >> reg) <*> (O.STRING <$> string)
    <|> eR1U16 "expect" <$> (the "expect" >> reg) <*> (O.STRING <$> string)
    <|> eR1U16 "check-assert" <$> (the "check-assert" >> reg) <*> (O.STRING <$> string)

    <|> regint "mkclosure" <$> reg <~> the ":=" <~> the "closure" <~> the "["
        <*> reg <~> the "," <*> int <~> the "]"
    <|> regint "getclslot" <$> reg <~> the ":=" <*> reg <~> the "{" <*> int <~> the "}"
    <|> setcl "setclslot" <$> reg <~> the "{" <*> int <~> the "}" <~> the ":=" <*> reg 

    <|> foldl (op <|>) P.pzero (map two_reg_assign unop)
    <|> meR2 "copy" <$> reg <~> the ":=" <*> reg <*> location
    
    <|> ifgoto <$> (the "if" >> reg) <*> (the "goto" >> name <~> the ":")

    <|> foldl (op <|>) P.pzero (map one_reg_effect eftop)

    <|> A.DEFLABEL <$> (name <~> the ":")
    <|> A.GOTO_LABEL <$> (the "goto" >> name <~> the ":")
    <|> eR0 <$> sat (P.eq "halt") name
    <|> eR0 <$> sat (P.eq "gc") name

    <|> meR3 "call" <$> reg <~> the ":=" <~> the "call" <*> reg <*> arg_parser <*> location
    <|> callnoarg "call" <$> reg <~> the ":=" <~> the "call" <*> reg <~> many (the "(" >> the ")") <*> location

    <|> meR2 "tailcall" <$> (the "tailcall" >> reg) <*> arg_parser <*> location
    <|> tailcallnoarg "tailcall" <$> (the "tailcall" >> reg) <*> location

   (**** recursive parser that handles end-of-line and function loading ****)

   (* Parsers for start and end of "load function", for you to write.
      Designing syntax so each one terminates with `eol` is recommended. *)

   fun addbrackets name = "(" ^ name ^ ")" (*give brackets back to anonymous_fun (idempotent)*)
   val funname = name <|> addbrackets <$> (the "(" >> name <~> the ")")
   
   fun loadfunc (reg, arity, name) body = A.LOADFUNC (reg, arity, name, body)
   val loadfunStart : (int * int * string) parser = (* fill in with (reg * arity) parser *)
        trip <$> (the ".loadfun" >> reg) <*> int <*> (funname <~> many1 eol) 
   val loadfunEnd : unit parser =
        the "el" <~> (many1 eol)

   (* grammar :   <instruction> ::= <one_line_instruction> EOL
                                 | <loadfunStart> {<instruction>} <loadfunEnd> *)

   (* simple parser with no error detection *)
   val instruction : A.instr Error.error parser
     = Error.OK <$>
       P.fix (fn instruction : A.instr parser =>
                   one_line_instr <~> many1 eol
               <|> loadfunc <$> loadfunStart <*> many instruction <~> loadfunEnd)

   (* A better parser is juiced up with extra error detection *)

   fun badTokens ts = Error.ERROR ("unrecognized assembly line: " ^ showTokens ts)
   val nonEOL = sat (curry op <> L.EOL) one  (* any token except EOL *)

   val instruction : A.instr Error.error parser
     = P.fix
       (fn instruction : A.instr Error.error parser =>
              Error.OK <$> one_line_instr <~> many1 eol
          <|> Error.OK <$>
              (loadfunc <$> loadfunStart <*> 
                            P.check (Error.list <$> many instruction) <~>
                            loadfunEnd)
          <|> P.notFollowedBy loadfunEnd >>
              (* gobble to end of line, then succeed by producing error message: *)
              badTokens <$> many nonEOL <~> many1 eol  
       )


  val parse = Error.join o P.produce (Error.list <$> (many eol >> many instruction)) o List.concat
            

  (*************************** unparsing *****************************)

  val int = Int.toString
  fun reg r = "r" ^ int r
  val spaceSep = String.concatWith " "

  fun unparse_lit (O.STRING s) = ["\"" ^ (String.toString s) ^ "\""]
    | unparse_lit l            = (ObjectUnparser.literal l)

  fun unparse_glob (O.STRING s) = ["%" ^ s]
    | unparse_glob _            = Impossible.impossible "impossible global val"

  fun up_three_reg_assign operator x y z = spaceSep [reg x, ":=", reg y, operator, reg z]
  fun up_two_reg_assign operator x y  = spaceSep [reg x, ":=", operator, reg y]
  fun up_one_reg_effect operator x = spaceSep [operator, reg x]
  (* move spacesep *)
  fun up_loc (file, line) = " (" ^ file ^ " - " ^ Int.toString line ^ ")"

  fun unparse1 (A.OBJECT_CODE (O.REGS (operator, [x, y, z]))) =
        (case operator of "+imm" => 
                            let val n = z - 128
                            in  if n < 0 then
                                    spaceSep [reg x, ":=", reg y, "-",  int (~n)]
                                else
                                    spaceSep [reg x, ":=", reg y, "+",  int n]
                            end
                        |  "call" => if y = z then 
                                        spaceSep [reg x, ":= call", reg y]
                                     else 
                                        spaceSep [reg x, ":= call", reg y, 
                                        "(r" ^ int (y + 1) ^ ",", "...,", reg z ^ ")"]
                        |  sop => up_three_reg_assign sop x y z)

    | unparse1 (A.OBJECT_CODE (O.REGS (operator, [x, y]))) = 
        (case operator of "swap" => spaceSep [reg x, ",", reg y, ":=", reg y, ",", reg x]
                        | "copy" => spaceSep [reg x, ":=", reg y]
                        |  "tailcall" => if x = y then 
                                            spaceSep ["tailcall", reg x]
                                         else 
                                            spaceSep ["tailcall", reg x, 
                                            "(r" ^ int (x + 1) ^ ",", "...,", reg y ^ ")"]
                        |  sop   => up_two_reg_assign sop x y)

    | unparse1 (A.OBJECT_CODE (O.REGS (operator, [x]))) = 
        up_one_reg_effect operator x

    | unparse1 (A.OBJECT_CODE (O.REGSLIT (opcode, [x], value))) =
        (case opcode of "loadliteral" => spaceSep ([reg x, ":="] @ unparse_lit value) 
                      | "getglobal"   => spaceSep ([reg x, ":="] @ unparse_glob value)
                      | "setglobal"   => spaceSep (unparse_glob value @ [":=", reg x])
                      | "check"       => spaceSep (["check", reg x] @ unparse_lit value)
                      | "expect"      => spaceSep (["expect", reg x] @ unparse_lit value)
                      | "check-assert"=> spaceSep (["check-assert", reg x] @ unparse_lit value)
                      | _             => opcode ^ " not recognized")
    | unparse1 (A.OBJECT_CODE (O.REGINT (opcode, r1, r2, value))) =
        (case opcode of "mkclosure" => spaceSep [reg r1, ":=", "closure[" ^ reg r2 ^ ",", Int.toString value ^ "]"]
                      | "getclslot" => spaceSep [reg r1, ":=", reg r2 ^ "{" ^ Int.toString value ^ "}"]
                      | "setclslot" => spaceSep [reg r1 ^ "{" ^ Int.toString value ^ "}", ":=", reg r2]
                      | _ => opcode ^ " not recognized")
    | unparse1 (A.GOTO_LABEL (label)) =
        spaceSep ["goto", label ^ ":"]      
    | unparse1 (A.IF_GOTO_LABEL (x, label)) =
        spaceSep ["if", reg x, "goto", label ^ ":"]
    | unparse1 (A.DEFLABEL (label)) =
        spaceSep [label ^ ":"] 
    | unparse1 (A.OBJECT_CODE (O.GOTO (k))) =
        spaceSep ["goto", int k]
    | unparse1 (A.OBJECT_CODE (O.REGS ("halt", []))) =
        "halt"
    | unparse1 (A.OBJECT_CODE (O.REGS ("gc", []))) =
        "gc"
    | unparse1 (A.OBJECT_CODE (O.MREGS (loc, p, reglist))) =
        unparse1 (A.OBJECT_CODE (O.REGS (p, reglist))) ^ (up_loc loc)
    | unparse1 (A.OBJECT_CODE (O.MREGSLIT (loc, opcode, regslit, value))) =
        unparse1 (A.OBJECT_CODE (O.REGSLIT (opcode, regslit, value))) ^ (up_loc loc) 
    | unparse1 _ = Impossible.impossible "an unknown assembly-code instruction" (*in production*)

  
  fun unparse ((A.LOADFUNC (r, k, n, body))::ins) =
        spaceSep [".loadfun", reg r, int k, n] :: (List.map (curry (op ^) "\t") (unparse body)) @ ["el"] @ unparse ins
    | unparse ((A.OBJECT_CODE (O.LOADFUNC (r, k, n, body)))::ins) = 
        unparse (A.LOADFUNC (r, k, n, (List.map A.OBJECT_CODE body))::ins)
    | unparse (i::is) = unparse1 i :: unparse is
    | unparse [] = []

        (* Note: When unparsed, the body of LOADFUNC should be indented *)


end
