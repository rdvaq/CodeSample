(define g (x) (+ x 1))

(define f (x)
    (g x))

(check-expect (f 2) 3)