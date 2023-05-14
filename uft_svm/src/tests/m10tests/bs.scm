(define o (f g) (lambda (x) (f (g x))))

(define f1 (x) (+ x 1))
(define f2 (x) (+ x 1))


(check-expect ((o f1 f2) 1) 3)