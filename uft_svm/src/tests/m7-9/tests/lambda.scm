(define l () (lambda (x) (+ x 1)))

(check-expect ((l) 1) 2)