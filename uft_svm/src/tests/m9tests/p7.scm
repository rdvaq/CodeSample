(define changex (x) (begin (set x 5) (+ x 1)))
;; (define caller (x) (begin (changex x)))
(check-expect (changex 3) 6)

;; (let ([z 2]) (begin (changex z) (println z)))