(define tfact (n acc) 
    (if (= n 0)
        acc
        (tfact (- n 1) (* n acc))))

(check-expect (tfact 10 1) 3628800)