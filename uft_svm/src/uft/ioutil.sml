(* Utility functions for handling text input and output for UFT *)

(* You can ignore this *)

structure IOUtil :> sig
  (**** code location support ****)
  type srcloc = string * int
  type 'a located = 'a * srcloc
  val lines  : TextIO.instream -> string list
  val locatedLines  : string -> TextIO.instream -> string located list
  val output : TextIO.outstream -> string -> unit
  val outln  : TextIO.outstream -> string -> unit   (* adds newline *)
end
  =
struct
  type srcloc = string * int
  type 'a located = 'a * srcloc
  fun locatedLines filename fd =
    let fun go prev' n =
          case TextIO.inputLine fd
            of NONE => rev prev'
             | SOME l => go ((l, (filename, n)) :: prev') (n + 1)
    in  go [] 1
    end
  fun lines fd =
    let fun go prev' =
          case TextIO.inputLine fd
            of NONE => rev prev'
             | SOME l => go (l :: prev')
    in  go []
    end

  fun output out s = TextIO.output (out, s)
  fun outln out s = app (output out) [s, "\n"]
end
  