;;loadliteral
(let ([r2 #t])
  (check r2 'r2-is-true))
(let* ([r2 #t])
  (expect r2 'true))

;;set and load global
(let* ([r1 'hello] [r2 (set greetings r1)] [r3 greetings] [r4 (println r3)]) 
    (check r3 'greetings))
(let* ([r3 'hello]) 
    (expect r3 'hello))

;; assignment (copy)
(let* ([r2 2] [r1 r2])
  (check r1 'r1-equals-r2))
(let* ([r1 2])
  (expect r1 'two))

;; vmop
(let* ([r0 2] [r1 3] [r0 (+ r0 r1)])
  (check r0 'two-plus-three))
(let* ([r0 5])
  (expect r0 'five))

(let* ([r1 emptylist] [r2 2] [r3 (cons r2 r1)] [r4 (pair? r3)]) 
    (check r4 'true))
(let* ([r4 #t]) 
    (expect r4 'true))

(let* ([r0 2] [r0 (println r0)]) ;; with [r0 (println r0)], the dest reg is disregarded 
  (check r0 'print-r0))
(let* ([r0 2])
  (expect r0 'two))

;;IFELSE
(let* ([r0 #t] [r1 1] [r2 2] [r5 (if r0 (+ r1 r2) (- r2 r1))]) 
   (check r5 'if-else-r0))
(let* ([r5 3])
  (expect r5 'three))

(let* ([r0 #f] [r1 1] [r2 2] [r5 (if r0 (+ r1 r2) (- r2 r1))]) 
   (check r5 'if-else-r0))
(let* ([r5 1])
  (expect r5 'one))

;; begin 
(let* ([r1 2] [r2 5] [r3 (begin (- r1 r2) (+ r1 r2))]) 
    (check r3 'begin-r3))
(let* ([r3 7])
  (expect r3 'seven)) 

;;while (kn invariant violated)
(let* ([r2 1] [r3 3] [r0 (while (let ([r1 (< r2 r3)]) r1) (let ([r2 5]) r2))]) 
    (check r0 'while-r3))
(let* ([r0 #f])
  (expect r0 'false)) 

(let* ([r2 1] [r3 3] [r0 (while (let ([r1 (< r2 r3)]) r1) (set r2 5))]) 
    (check r2 'r2-is-five))
(let* ([r2 5])
  (expect r2 'five)) 

;; funcode (helped by stephanie & vivian)
(let* ([r2 1][r0 (lambda (r5) (+ r5 r5))] [r1 (function? r0)]) 
    (check r1 'funcode-r1))
(let* ([r1 #t]) 
    (expect r1 'true))

;; set and load global with lambda
(let* ([r1 (lambda (r0) (* r0 r0))] [r2 (set square r1)] [r3 square] [r4 (function? r3)]) 
    (check r4 'funcode-r4))
(let* ([r4 #t]) 
    (expect r4 'true))

