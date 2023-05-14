(set x 2)
(define square (x) (* x x))
(if (< 4 0) (print 1) (print 2))
(check-expect (square 4) 16)