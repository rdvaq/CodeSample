(* Parser for S-expressions *)

(* You can ignore this file *)

structure Sx = struct
  (* symbolic expressions *)
  type srcloc = IOUtil.srcloc
  type 'a located = 'a IOUtil.located 
  datatype sx
    = INT  of int
    | BOOL of bool
    | SYM  of string
    | REAL of real
    | LIST of sx list
    | MARK of sx located
end

(****************************************************************)

structure SxUtil :> sig
  val whatis : Sx.sx -> string
end
  =
struct
  fun whatis (Sx.INT _) = "an integer"
    | whatis (Sx.BOOL _) = "a Boolean"
    | whatis (Sx.SYM _)  = "a symbol"
    | whatis (Sx.REAL _) = "a real number"
    | whatis (Sx.LIST _) = "a list"
    | whatis (Sx.MARK _) = "an exp with location"
end


structure SxParse :> sig
  type srcloc = IOUtil.srcloc
  type 'a located = 'a IOUtil.located 
  (* val parse : string list -> Sx.sx list Error.error   input list of _lines_ *)
  val parse : string located list -> Sx.sx list Error.error
end
  = 
struct
  type srcloc = IOUtil.srcloc
  type 'a located = 'a IOUtil.located 
  structure L = SxLex
  structure P = MkListProducer (val species = "parser"
                                type input = L.token located
                                fun show ts = Int.toString (length ts) ^ " token(s)"
                               )


  (* wishing for Modula-style FROM IMPORT here ... *)
  infix 3 <*>      val op <*> = P.<*>
  infixr 4 <$>     val op <$> = P.<$>
  infix 1 <|>      val op <|> = P.<|>
  infix 6 <~> >>   val op <~> = P.<~>  val op >> = P.>>

  fun fst (x, y) = x;
  fun snd (x, y) = y;
  fun pair x y = (x, y)

  val succeed = P.succeed
  val id = P.id
  val many = P.many
  val sat = P.sat
  val one = fst <$> P.one
  val loc = snd <$> P.peek
  type 'a parser = 'a P.producer

  infix 4 <!>
  fun p <!> msg =
    P.ofFunction (fn tokens => (case P.asFunction p tokens
                    of SOME (Error.OK _, unread) => SOME (Error.ERROR msg, unread)
                     | _ => NONE))
  val op <!> : 'a parser * string -> 'b parser = op <!>


  val quote     = P.maybe (fn (L.QUOTE)     => SOME () | _ => NONE) one
  val int       = P.maybe (fn (L.INT   n)   => SOME n  | _ => NONE) one
  val right     = P.maybe (fn (L.RIGHT s)   => SOME s  | _ => NONE) one
  val left      = P.maybe (fn (L.LEFT  s)   => SOME s  | _ => NONE) one
  val booltok   = P.maybe (fn (L.SHARP b)   => SOME b  | _ => NONE) one
  val name      = P.maybe (fn (L.NAME  n)   => SOME n  | _ => NONE) one

  val right : L.bracket_shape parser = right

  fun notCurly L.CURLY = false
    | notCurly _     = true

  val leftCurly = sat (not o notCurly) left

  (************** bracket parsers ******)

  datatype right_result
    = FOUND_RIGHT      of L.bracket_shape
    | SCANNED_TO_RIGHT (* location where scanning started *)
    | NO_RIGHT

  (* val scanToClose = P.ofFunction (fn tokens =>
    let fun scan lpcount tokens =
          (* lpcount is the number of unmatched left parentheses *)
          case tokens
            of L.LEFT  t :: tokens => scan (lpcount+1) tokens
             | L.RIGHT t :: tokens => if lpcount = 0 then
                                      P.asFunction (succeed SCANNED_TO_RIGHT) tokens
                                    else
                                      scan (lpcount-1) tokens
             | _  :: tokens => scan lpcount tokens
             | []           => P.asFunction (succeed NO_RIGHT) tokens
    in  scan 0 tokens
    end) *)

  val scanToClose = P.ofFunction (fn input =>
    let fun scan lpcount locatedTokens =
          (* lpcount is the number of unmatched left parentheses *)
          case locatedTokens
            of ((L.LEFT  t, _) :: ltokens)  => scan (lpcount+1) ltokens
             | ((L.RIGHT t, _) :: ltokens) => if lpcount = 0 then
                                      P.asFunction (succeed SCANNED_TO_RIGHT) ltokens
                                    else
                                      scan (lpcount-1) ltokens
             | _  :: ltokens => scan lpcount ltokens
             | []           => P.asFunction (succeed NO_RIGHT) locatedTokens
    in  scan 0 input
    end)
    

  val matchingRight = FOUND_RIGHT <$> right <|> scanToClose

  fun matchBrackets _ left _ NO_RIGHT =
        Error.ERROR ("unmatched " ^ L.leftString left)
    | matchBrackets e left _ SCANNED_TO_RIGHT =
        Error.ERROR ("expected " ^ e) 
    | matchBrackets _ left a (FOUND_RIGHT right) =
        if left = right then
          Error.OK a
        else
          Error.ERROR (L.rightString right ^ " does not match " ^ L.leftString left)

  fun liberalBracket (expected, p) =
    P.check (matchBrackets expected <$> sat notCurly left <*> p <*> matchingRight)

  fun scanChars [] = NONE
    | scanChars (c::cs) = SOME (c, cs)

  fun realOfString s = case Real.scan scanChars (explode s)
                         of SOME (x, []) => SOME x
                          | _ => NONE

  fun readName "." = Error.ERROR "this reader cannot handle . in S-expressions"
    | readName s   =
        Error.OK (case realOfString s of SOME x => Sx.REAL x | NONE => Sx.SYM s)

  fun eprint s = TextIO.output (TextIO.stdErr, s)

  fun wrap what f =
    fn x => (app eprint [what, "\n"]; TextIO.flushOut TextIO.stdErr; f x)
            before app eprint ["finished ", what, "\n"]


  (* fun sexp tokens =
    let val sexp = P.ofFunction sexp
        val parser = 

       P.check (readName <$> name)
   <|> Sx.INT     <$> int
   <|> Sx.BOOL    <$> booltok
   <|> leftCurly  >> P.perror "curly brackets may not be used in S-expressions"
   <|> Sx.LIST    <$> liberalBracket ("list of S-expressions", many sexp)
   <|> (fn v => Sx.LIST [Sx.SYM "quote", v]) <$> (quote >> sexp)
    in  P.asFunction parser tokens 
    end
  val sexp = P.ofFunction sexp : Sx.sx parser *)

  val sexp = P.fix (fn unmarked =>
    let fun wrap start sx = Sx.MARK (sx, start)
        val sexp = wrap <$> loc <*> unmarked  (* avoid end-of-stream issues *)
        val parser = 

       P.check (readName <$> name)
   <|> Sx.INT     <$> int
   <|> Sx.BOOL    <$> booltok
   <|> leftCurly  >> P.perror "curly brackets may not be used in S-expressions"
   <|> Sx.LIST    <$> liberalBracket ("list of S-expressions", many sexp)
   <|> (fn v => Sx.LIST [Sx.SYM "quote", v]) <$> (quote >> sexp)
    in  parser
    end)

  local
    infix 0 >> >=> >>=   
    fun f >> g = fn x => g (f x)         (* function composition, Elm style *)
    val op >=> = Error.>=>
    val ! = Error.map
    val op >>= = Error.>>=
    val succeed = Error.succeed
    fun emap f = map f >> Error.list
  in            
    (*| token list list error*)  (*token list error *)
    (* val parse = emap L.tokenize_line >> ! List.concat >=> P.produce (many sexp) *)
                     (*^string -> token list Error.error*)
    

      (* val tokenize_line : string -> token list Error.error *)
      (* val list : 'a error list -> 'a list error *)

    fun tokWithLoc (line, loc) = 
      let fun mktlpair tks = succeed (map (fn t => (t, loc)) tks)
      in 
        L.tokenize_line line >>= mktlpair
      end
    val _ = tokWithLoc : string located -> L.token located list Error.error 

    val parse = emap tokWithLoc >> ! List.concat >=> P.produce (many sexp)
                    (* token located list error *)
  end              

end
