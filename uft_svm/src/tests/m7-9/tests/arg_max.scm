(define arg-max (f xs)
    (if (null? (cdr xs))
        (car xs)
        (let ([sub-max (arg-max f (cdr xs))])
            (if (< (f sub-max) (f (car xs)) )
                (car xs)
                sub-max))))

        ;; replace next line with good check-expect or check-assert tests
        (define square (a) (* a a))
        (check-expect (arg-max square '(1)) 1)
        (check-expect (arg-max square '(5 4 3 2 1)) 5)
        (check-expect (arg-max square '(1 2 3)) 3)