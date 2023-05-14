;; ;; F.LITERAL, F.CHECK_EXPECT, F.CHECK_ASSERT
;; ;;
;; (check-expect (number? 3) #t)            
;; (check-expect (number? 'really?) #f)
;; (check-assert (symbol? 'really?))

;; ;; F.GLOBAL, F.SETGLOBAL, F.VAl
;; ;;
;; (check-expect (number? (set wow #t)) #f)
;; (check-expect (set wow 10) 10)
;; (set wow2 15)
;; (check-assert (number? wow2))
;; (val x 5)
;; (check-expect x 5)

;; ;; F.BEGIN (implicitly tests F.EXP)
;; ;;
;; (check-expect (begin) #f)
;; (check-assert (begin (symbol? 'a)))
;; (check-assert (begin (symbol? 1) (symbol? 'b)))
;; (check-expect (begin (symbol? 1) (symbol? 'b) (symbol? 2)) #f)

;; ;; F.IFX F.WHILEX
;; ;;
;; (check-expect (if (symbol? 'a) (set wow2 1) (symbol? 1)) 1)
;; (check-expect (while (number? wow2) (set wow2 'a)) #f)

;; ;; F.DEFINE
;; ;;
;; (define fun1 (x) (number? x))
;; (define fun2 (x y z) (number? x))
;; (check-assert (function? fun1))
;; (check-assert (function? fun2))

;; ;; F.PRIMCALL
;; ;;
;; (check-assert (symbol? 'a))
;; (check-expect (+ 1 2) 3)
;; (check-expect (* 10 11) 110)

;; ;; F.FUNCALL 
;; ;;
;; (define square (x) (* x x))
;; (define sum_squares (x y) (+ (square x) (square y)))
;; (check-expect (square 6) 36)
;; (check-expect (sum_squares 1 2) 5)

;; ;; F.LOCAL, F.SETLOCAL
;; ;;
;; (define sum (u v w x y z) (+ u (+ v (+ w (+ x (+ y z))))))
;; (check-expect (sum 1 2 3 4 5 6) 21)

;; ;; F.LET
;; (check-assert (let ([x y] [y x]) (nil? x)))
;; (define fun (x y) (let ([x y] [y x]) (nil? x)))
;; (check-expect (let ([x 1] [y 2]) (+ x y)) 3)
;; (check-expect (let ([x 1] [y 2] [z (* 12 2)]) (+ x (* z y))) 49)


;; (val n 3)
;; (while (< n 5) (begin (print n) (set n (+ n 1))))

;; (define f ()
;;    (let* ([x 1]
;;           [z x])
;;      (begin (set z 2)
;;             x)))
;; (check-expect (f) 1)


;; (define id (x) (let ([y x] [x (+ x 2)]) y)) (check-expect (id 10) 10)
(define id (x) (let* ([y x] [x (set x 2)]) y)) (check-expect (id 10) 10)