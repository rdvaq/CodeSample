(* This is the universal forward translator. As you build the different VScheme 
    representations and the translations between them, you'll chain together 
    these translations here. It implements the actual translations *)

(* You'll get a partially complete version of this file, 
    which you'll need to complete *)

structure UFT :> sig
  type language = Languages.language
  exception NotForward of language * language
  val translate : language * language * string -> TextIO.instream * TextIO.outstream -> unit Error.error
end
  =
struct
  type 'a located = 'a IOUtil.located
  (**** I/O functions and types ****)

  type instream = TextIO.instream
  val lines  = IOUtil.lines : instream -> string list
  val locatedLines  = IOUtil.locatedLines : string -> instream -> string located list
  val output = IOUtil.output 
  val outln  = IOUtil.outln


  (**** function composition, including errors ****)

  type 'a error = 'a Error.error

  infix 0 >>> >=>
  fun f >>> g = fn x => g (f x)         (* function composition, Elm style *)
  val op >=> = Error.>=>

  fun liftMap f xs = Error.list (map f xs)  (* liftMap f == map f >>> Error.list *)

  (**** Reader functions ****)

  (* val schemeOfFile : instream -> VScheme.def list error = *)
  fun schemeOfFile filename =
    locatedLines filename                   (*string located list *)
    >>>  SxParse.parse                      (* sx list error *)     
    >=>  Error.mapList VSchemeParsers.defs  (* def list list error *)
    >>>  Error.map List.concat              (* def list error *)
    >>>  Error.map VSchemeTests.delay
  val _ = schemeOfFile : string -> instream -> VScheme.def list error 
    
  fun schemexOfFile filename =
    schemeOfFile filename >>>
    Error.map (map Disambiguate.disambiguate)
  val _ = schemexOfFile : string -> instream -> UnambiguousVScheme.def list error

  val VS_of_file : instream -> AssemblyCode.instr list error =
    lines                    (* line list *)
    >>> map AsmLex.tokenize  (* token list error list *)
    >>> Error.list           (* token list list error *)
    >=> AsmParse.parse       (* instr list error *)    

  fun KN_of_file filename =  
    schemexOfFile filename >=> 
    Error.mapList KNProject.def 
  val _ = KN_of_file : string -> instream -> string KNormalForm.exp list error 

  fun FO_of_file filename =
    schemexOfFile filename >=>
    Error.mapList FOUtil.project
  val _ = FO_of_file : string -> instream -> FirstOrderScheme.def list error 

  (**** Support for materialization ****)
  
  exception Backward  (* for internal use only *)
                      (* raised if someone commands a backward translation *)

  datatype language = datatype Languages.language (* imports value constructors *)
  exception NoTranslationTo of language  (* used internally *)

  val ! = Error.map  (* useful abbreviation for materializers and `translate` *)

  (**** Materializer functions ****) 
  fun HOX_of HOX  = schemexOfFile
    | HOX_of _    = raise Backward

  fun HO_of HO filename = schemexOfFile filename >=> (Error.mapList Mutability.detect)
    | HO_of HOX _ = Impossible.unimp "imperative features (HOX to HO)"
    | HO_of _ _   = raise Backward

  fun FO_of FO     = FO_of_file
    | FO_of inLang = raise Backward

  fun CL_of CL filename     = CL_of FO  filename (* really *)
    | CL_of HO filename     = HO_of HO  filename  >>> ! (map ClosureConvert.close)
    | CL_of HOX filename    = HO_of HOX filename    >>> ! (map ClosureConvert.close)
    | CL_of inLang filename = FO_of inLang filename >>> ! (map FOCLUtil.embed)

  val KN_reg_of_KN_string : string KNormalForm.exp list -> ObjectCode.reg KNormalForm.exp list error
      =  Error.mapList (KNRename.mapx KNRename.regOfName)

  fun KN_reg_of KN filename   = KN_of_file filename >=> KN_reg_of_KN_string
    | KN_reg_of inLang filename = CL_of inLang filename >>> ! (List.map KNormalize.def)

  fun KN_text_of KN filename    = KN_of_file filename
    | KN_text_of inLang filename = KN_reg_of inLang filename >=> Error.mapList (KNRename.mapx KNRename.nameOfReg)

  val VS_of_KN : ObjectCode.reg KNormalForm.exp list -> AssemblyCode.instr list
      = List.concat o (List.map Codegen.forEffect)

  fun VS_of VS _  = VS_of_file  (*has its own lines parser*)
    | VS_of inLang filename = KN_reg_of inLang filename >>> ! VS_of_KN

  fun VO_of VO _    = (fn _ => Error.ERROR "There is no reader for .vo")
    | VO_of inLang filename = VS_of inLang filename >=> Assembler.translate


  (**** Emitter functions ****)

  val width =  (* parameter to prettyprinter *)
    case Option.mapPartial Int.fromString (OS.Process.getEnv "WIDTH")
      of SOME n => n
       | NONE => 72

  fun emitVO outfile = app (outln outfile) o ObjectUnparser.module
  fun emitVS outfile = app (outln outfile) o AsmParse.unparse

  fun emitScheme outfile = Wppx.toOutStream width outfile o WppScheme.pp

  fun emitHO outfile = app (emitScheme outfile o Disambiguate.ambiguate)

  fun emitKN outfile = app (emitScheme outfile o KNEmbed.def) 

  fun emitFO outfile = app (emitScheme outfile o FOUtil.embed)

  fun emitCL outfile = app (emitScheme outfile o CSUtil.embed)

  (**** The Universal Forward Translator ****)

  exception NotForward of language * language  (* for external consumption *)

  fun translate (inLang, outLang, filename) (infile, outfile) =
    (case outLang
       of VO => VO_of      inLang filename >>> ! (emitVO outfile)
        | VS => VS_of      inLang filename >>> ! (emitVS outfile)
        | KN => KN_text_of inLang filename >>> ! (emitKN outfile)
        | FO => FO_of      inLang filename >>> ! (emitFO outfile)
        | CL => CL_of      inLang filename >>> ! (emitCL outfile)
        | HO => HO_of      inLang filename >>> ! (emitHO outfile)
        | HOX => HOX_of    inLang filename >>> ! (emitHO outfile)
    ) infile
    handle Backward => raise NotForward (inLang, outLang)
         | NoTranslationTo outLang => raise NotForward (inLang, outLang)
end
