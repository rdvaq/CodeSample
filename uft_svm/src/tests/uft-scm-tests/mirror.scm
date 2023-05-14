(define or  (b c) (if b  b  c))
(define list1 (x)     (cons x '()))
(define atom? (x) (or (symbol? x) (or (number? x) (or (boolean? x) (null? x)))))
(define append (xs ys)
  (if (null? xs)
     ys
     (cons (car xs) (append (cdr xs) ys))))


(define mirror (xs)
    (if (null? xs)
        xs
        (if (atom? (car xs))
            (append (mirror (cdr xs)) (list1 (car xs)))
            (append (mirror (cdr xs)) (list1 (mirror (car xs)))))))

        ;;
        ;; (check-expect (mirror '()) '())
        (check-expect (mirror '(1 2)) '(2 1))
        ;; (check-expect (mirror '((a (b 5)) (c d) e)) '(e (d c) ((5 b) a)))