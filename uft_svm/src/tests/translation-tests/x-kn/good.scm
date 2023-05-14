#t
x
(let* ([x 1] [y 2]) (+ x y))
(define square (x) (* x x))
(let* ([x 1] [t square]) (t x))
(let ([x 1]) (if x 1 2))
(let ([x 1]) (- x x))
(let* ([x 1] [y 2] [f square]) (begin (f x) (f y)))
(let ([x 1]) (set g x))
(let* ([x 1] [y 2]) (set x y))
(while (let ([a 2]) a) 1)