;  predefined uScheme functions 96a 
(define caar (xs) (car (car xs)))
(define cadr (xs) (car (cdr xs)))
(define cdar (xs) (cdr (car xs)))
;  predefined uScheme functions ((elided)) (THIS CAN'T HAPPEN -- claimed code was not used) 
;  more predefined combinations of [[car]] and [[cdr]] S151b 
(define cddr  (sx) (cdr (cdr  sx)))
(define caaar (sx) (car (caar sx)))
(define caadr (sx) (car (cadr sx)))
(define cadar (sx) (car (cdar sx)))
(define caddr (sx) (car (cddr sx)))
(define cdaar (sx) (cdr (caar sx)))
(define cdadr (sx) (cdr (cadr sx)))
(define cddar (sx) (cdr (cdar sx)))
(define cdddr (sx) (cdr (cddr sx)))
;  more predefined combinations of [[car]] and [[cdr]] S151c 
(define caaaar (sx) (car (caaar sx)))
(define caaadr (sx) (car (caadr sx)))
(define caadar (sx) (car (cadar sx)))
(define caaddr (sx) (car (caddr sx)))
(define cadaar (sx) (car (cdaar sx)))
(define cadadr (sx) (car (cdadr sx)))
(define caddar (sx) (car (cddar sx)))
(define cadddr (sx) (car (cdddr sx)))
;  more predefined combinations of [[car]] and [[cdr]] S151d 
(define cdaaar (sx) (cdr (caaar sx)))
(define cdaadr (sx) (cdr (caadr sx)))
(define cdadar (sx) (cdr (cadar sx)))
(define cdaddr (sx) (cdr (caddr sx)))
(define cddaar (sx) (cdr (cdaar sx)))
(define cddadr (sx) (cdr (cdadr sx)))
(define cdddar (sx) (cdr (cddar sx)))
(define cddddr (sx) (cdr (cdddr sx)))
;  predefined uScheme functions 96b 
(define list1 (x)     (cons x '()))
(define list2 (x y)   (cons x (list1 y)))
(define list3 (x y z) (cons x (list2 y z)))
;  predefined uScheme functions 99b 
(define append (xs ys)
  (if (null? xs)
     ys
     (cons (car xs) (append (cdr xs) ys))))
;  predefined uScheme functions 100b 
(define revapp (xs ys) ; (reverse xs) followed by ys
  (if (null? xs)
     ys
     (revapp (cdr xs) (cons (car xs) ys))))
;  predefined uScheme functions 101a 
(define reverse (xs) (revapp xs '()))
;  predefined uScheme functions ((elided)) (THIS CAN'T HAPPEN -- claimed code was not used) 
(define nth (n xs)
  (if (= n 0)
    (car xs)
    (nth (- n 1) (cdr xs))))

(define CAPTURED-IN (i xs) (nth (+ i 1) xs))
;  definitions of predefined uScheme functions [[and]], [[or]], and [[not]] 97a 
(define and (b c) (if b  c  b))
(define or  (b c) (if b  b  c))
(define not (b)   (if b #f #t))
;  predefined uScheme functions 102c 
(define atom? (x) (or (symbol? x) (or (number? x) (or (boolean? x) (null? x)))))
;  predefined uScheme functions 103b 
(define equal? (sx1 sx2)
  (if (atom? sx1)
    (= sx1 sx2)
    (if (atom? sx2)
        #f
        (and (equal? (car sx1) (car sx2))
             (equal? (cdr sx1) (cdr sx2))))))
;  predefined uScheme functions 105c 
(define make-alist-pair (k a) (list2 k a))
(define alist-pair-key        (pair)  (car  pair))
(define alist-pair-attribute  (pair)  (cadr pair))
;  predefined uScheme functions 105d 
(define alist-first-key       (alist) (alist-pair-key       (car alist)))
(define alist-first-attribute (alist) (alist-pair-attribute (car alist)))
;  predefined uScheme functions 106a 
(define bind (k a alist)
  (if (null? alist)
    (list1 (make-alist-pair k a))
    (if (equal? k (alist-first-key alist))
      (cons (make-alist-pair k a) (cdr alist))
      (cons (car alist) (bind k a (cdr alist))))))
(define find (k alist)
  (if (null? alist)
    '()
    (if (equal? k (alist-first-key alist))
      (alist-first-attribute alist)
      (find k (cdr alist)))))
;  predefined uScheme functions 125a 
(define o (f g) (lambda (x) (f (g x))))          ; ((o f g) x) = (f (g x))
;  predefined uScheme functions 126b 
(define curry   (f) (lambda (x) (lambda (y) (f x y))))
(define uncurry (f) (lambda (x y) ((f x) y)))
;  predefined uScheme functions 129a 
(define filter (p? xs)
  (if (null? xs)
    '()
    (if (p? (car xs))
      (cons (car xs) (filter p? (cdr xs)))
      (filter p? (cdr xs)))))
;  predefined uScheme functions 129b 
(define map (f xs)
  (if (null? xs)
    '()
    (cons (f (car xs)) (map f (cdr xs)))))
;  predefined uScheme functions 130a 
(define app (f xs)
  (if (null? xs)
    #f
    (begin (f (car xs)) (app f (cdr xs)))))
;  predefined uScheme functions 130b 
(define exists? (p? xs)
  (if (null? xs)
    #f
    (if (p? (car xs)) 
      #t
      (exists? p? (cdr xs)))))
(define all? (p? xs)
  (if (null? xs)
    #t
    (if (p? (car xs))
      (all? p? (cdr xs))
      #f)))
;  predefined uScheme functions 130d 
(define foldr (op zero xs)
  (if (null? xs)
    zero
    (op (car xs) (foldr op zero (cdr xs)))))
(define foldl (op zero xs)
  (if (null? xs)
    zero
    (foldl op (op (car xs) zero) (cdr xs))))
;  predefined uScheme functions S150c 
(val newline      10)   (val left-round    40)
(val space        32)   (val right-round   41)
(val semicolon    59)   (val left-curly   123)
(val quotemark    39)   (val right-curly  125)
                        (val left-square   91)
                        (val right-square  93)
;  predefined uScheme functions S150d 
(define <= (x y) (not (> x y)))
(define >= (x y) (not (< x y)))
(define != (x y) (not (= x y)))
;  predefined uScheme functions S150e 
(define max (x y) (if (> x y) x y))
(define min (x y) (if (< x y) x y))
;  predefined uScheme functions S151a 
(define negated (n) (- 0 n))
(define mod (m n) (- m (* n (idiv m n))))
(define gcd (m n) (if (= n 0) m (gcd n (mod m n))))
(define lcm (m n) (if (= m 0) 0 (* m (idiv n (gcd m n)))))
;  predefined uScheme functions S151e 
(define list4 (x y z a)         (cons x (list3 y z a)))
(define list5 (x y z a b)       (cons x (list4 y z a b)))
(define list6 (x y z a b c)     (cons x (list5 y z a b c)))
(define list7 (x y z a b c d)   (cons x (list6 y z a b c d)))
(define list8 (x y z a b c d e) (cons x (list7 y z a b c d e)))
(define assoc (v pairs)
  (if (null? pairs)
      #f
      (let* ([first (car pairs)]
            [rest (cdr pairs)])
        (if (equal? v (car first))
            first
            (assoc v rest)))))

(define Table.new ()
  (cons nil '()))

(define Table.get (t k)
  (let ([pair (assoc k (cdr t))])
    (if pair
        (cdr pair)
        nil)))

(define Table.put (t k v)
  (let ([pair (assoc k (cdr t))])
    (if pair
        (set-cdr! pair v)
        (set-cdr! t (cons (cons k v) (cdr t))))))






;;;;;;;;;;;;;;;;;;; COMP 105 SCHEME ASSIGNMENT ;;;;;;;;;;;;;;;




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;
;;;;  Exercise 1

;; (match-first-list? xs ys) Takes two arguemnts xs, ys where xs and ys are in 
;; set LIST(ATOM), determines whether the n elements in xs mathces the first 
;; n elements of ys

(define match-first-list? (xs ys)
    (if (null? xs)
        #t 
        (if (null? ys)
            #f
            (if (= (car xs) (car ys))
                (match-first-list? (cdr xs) (cdr ys))
                #f))))

        ;; 
        (check-assert (match-first-list? '(a b c) '(a b c d)))
        (check-assert (not (match-first-list? '(1 2 3) '(1 2 4 3))))
        (check-assert (match-first-list? '() '(x y)))
        (check-assert (not (match-first-list? '(1 2) '())))



;; (contig-sublist? xs ys) Takes two arguemnts xs, ys where xs and ys are in 
;; set LIST(ATOM ), determines whether the first list is a contiguous 
;; subsequence of the second

;; laws:
;;   (contig-sublist? '() ys) == #t 
;;   (contig-sublist? xs '()) == #f 
;;   (contig-sublist? (cons x xs') (cons y ys')) == (contig-sublist? xs ys') 
;;    where x != y 
;;   (contig-sublist? (cons x xs') (cons y ys')) == (match-first-list? xs' ys')
;;   where x = y

(define contig-sublist? (xs ys)
    (if (null? xs)
        #t 
        (if (null? ys)
            #f
            (if (= (car xs) (car ys))
                (if (match-first-list? (cdr xs) (cdr ys))
                    #t
                    (contig-sublist? xs (cdr ys)))
                (contig-sublist? xs (cdr ys))))))


        ;; 
        (check-assert (contig-sublist? '() '(1 2)))
        (check-assert (not (contig-sublist? '(1) '())))
        (check-assert (not (contig-sublist? '(a b c) '(x a y b z c))))
        (check-assert (contig-sublist? '(x) '(x a y b z c)))

        ;; extra test cases that tests xs of size bigger than 1
        (check-assert (contig-sublist? '(x a y) '(x a y b z c)))
        (check-assert (contig-sublist? '(x a y) '(x a c x a y)))




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;
;;;;  Exercise 8


;; (mirror xs) Takes as arguemnt xs where xs is a list of S-expressions,
;; returns a list in which every list in xs is recusively mirrored, and the 
;; resulting lists are in reverse order

;; laws:
;;   (mirror '()) == '()
;;   (mirror (cons x xs')) == (append (mirror xs') x) where x is an atom
;;   (mirror (cons x xs')) == (append (mirror xs') (mirror x)) where x is a 
;;   list of S-expressions


(define mirror (xs)
    (if (null? xs)
        xs
        (if (atom? (car xs))
            (append (mirror (cdr xs)) (list1 (car xs)))
            (append (mirror (cdr xs)) (list1 (mirror (car xs)))))))

        ;;
        (check-expect (mirror '()) '())
        (check-expect (mirror '(1 2 3 4)) '(4 3 2 1))
        (check-expect (mirror '((a (b 5)) (c d) e)) '(e (d c) ((5 b) a)))




;; (flatten xs) Takes as arguemnt xs where xs is a list of S-expressions, 
;; returns a list that has the same atoms in the same order as xs but no 
;; internal brackets

;; laws:
;;   (flatten '()) == '()
;;   (flatten (cons x xs')) == (append x (flatten xs')) where x is an atom
;;   (flatten (cons x xs')) == (flatten xs') where x = '()
;;   (flatten (cons x xs')) == (append (flatten x) (flatten xs') where x is a 
;;   list


(define flatten (xs)
    (if (null? xs)
        xs
        (if (atom? (car xs))
            (if (null? (car xs))
                (flatten (cdr xs))
                (append (list1 (car xs)) (flatten (cdr xs))))
            (append (flatten (car xs)) (flatten (cdr xs))))))

        ;;
        (check-expect (flatten '()) '())
        (check-expect (flatten '(1 2 3 4)) '(1 2 3 4))
        (check-expect (flatten '((a) () ((b c) d e))) '(a b c d e))
        (check-expect (flatten '((I Ching) (U Thant) (E Coli))) 
                                                    '(I Ching U Thant E Coli))

        ;; extra tests for nested lists and nested null lists
        (check-expect (flatten '(((((a)))))) '(a))
        (check-expect (flatten '((()) a)) '(a))



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;
;;;;  Exercise 31


;; ;; (takewhile p? xs) Takes a predicate p? and a list xs and returns the longest 
;; ;; prefix of the list in which every element satisfies the predicate.

;; ;; laws:
;; ;;   (takewhile p? '()) == '()
;; ;;   (takewhile p? (cons x xs')) == '() where (p? x) = #f
;; ;;   (takewhile p? (cons x xs')) == (append x (takewhile p? xs')) where
;; ;;   (p? x) = #t

;; (define takewhile (p? xs)
;;     (if (null? xs)
;;         xs
;;         (if (p? (car xs))
;;             (append (list1 (car xs)) (takewhile p? (cdr xs)))
;;             '())))

;;         ;; 
;;         (check-expect (takewhile null? '()) '())
;;         (check-expect (takewhile atom? '((1 2) 3 4)) '())
;;         (check-expect (takewhile number? '(2 4 6 a b c cd)) '(2 4 6))



;; ;; (dropwhile p? xs) Takes a predicate p? and a list xs and removes the longest 
;; ;; prefix in the list that satisfies p? and returns whatever is left over

;; ;; laws:
;; ;;   (dropwhile p? '()) == '()
;; ;;   (dropwhile p? (cons x xs')) == (cons x xs') where (p? x) = #f  
;; ;;   (dropwhile p? (cons x xs')) == (dropwhile p? xs') where (p? x) = #t

;; (define dropwhile (p? xs)
;;     (if (null? xs)
;;         xs
;;         (if (p? (car xs))
;;             (dropwhile p? (cdr xs))
;;             xs)))

;;         ;; 
;;         (check-expect (dropwhile atom? '()) '())
;;         (check-expect (dropwhile boolean? '(4 #t #f #t b c)) '(4 #t #f #t b c))
;;         (check-expect (dropwhile number? '(2 4 6 a b c cd)) '(a b c cd))



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;  Exercise B


;; (take n xs) Takes a natural number n and a list of values xs. Returns the 
;; longest prefix of xs that contains at most n elements.

;; laws:
;;   (take 0 xs) == '() 
;;   (take n '()) == '() 
;;   (take n (cons x xs')) == (cons x (take n-1 xs')) where n > 0


(define take (n xs)
    (if (= n 0)
        '()
        (if (null? xs)
            '()
            (cons (car xs) (take (- n 1) (cdr xs))))))

        ;; replace next line with good check-expect or check-assert tests
        (check-expect (take 0 '(a b c)) '())
        (check-expect (take 5 '()) '())
        (check-expect (take 1 '(1 2 3 4)) '(1))

        ;;extra tests with n >= len(xs)
        (check-expect (take 5 '(a 1 d 4 #t)) '(a 1 d 4 #t))
        (check-expect (take 10 '(a 1 d 4 #t 6)) '(a 1 d 4 #t 6))



;; (drop n xs) Takes a natural number n and a list of values xs. Removes n 
;; elements from the front of the list and returns the remaining list

;; laws:
;;   (drop 0 xs) == xs 
;;   (drop n '()) == '()
;;   (drop n (cons x xs')) == (drop (- n 1) xs') where n > 0 

(define drop (n xs)
    (if (= n 0)
        xs
        (if (null? xs)
            xs
            (drop (- n 1) (cdr xs)))))

        ;;
        (check-expect (drop 0 '(1 2 3)) '(1 2 3))
        (check-expect (drop 10 '()) '())
        (check-expect (drop 3 '(1 2 3 a b c)) '(a b c))

        ;; test cases where n >= len(xs)
        (check-expect (drop 4 '(1 2)) '())
        (check-expect (drop 2 '(1 2)) '())

        ;; test that uses both take and drop
        (check-expect (append (take 3 '(1 2 3 a b c)) (drop 3 '(1 2 3 a b c)))
                                                            '(1 2 3 a b c))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;
;;;;  Exercise C


;; (zip xs ys) Takes as arguments a "pair" of lists xs and ys of equal length,
;; returns a list of pairs by associating corresponding values in the two 
;; lists.

;; laws:
;;   (zip '() '()) == '()
;;   (zip (cons x xs') (cons y ys')) == (cons (cons x (cons y)) (zip xs' ys'))  



(define zip (xs ys)
    (if (null? xs)
        xs
        (cons (list2 (car xs) (car ys)) (zip (cdr xs) (cdr ys)))))

        ;; ;;
        ;; (check-expect (zip '() '()) '())
        (check-expect (zip '(1) '(a)) '((1 a)))

        (check-expect (zip '(1 2 3) '(a b c)) '((1 a) (2 b) (3 c)))



;; (unzip ps) Takes as argument a list of pairs ps, converts ps to a "pair" of
;; lists and returns the result

;; (define unzip (ps)
;;     (if (null? ps)
;;         '(() ())
;;         (list2 (map car ps) (map cadr ps))))    
        
;;         ;; 
;;         (check-expect (unzip '()) '(() ()))
;;         (check-expect (unzip '((I Magnin) (U Thant) (E Coli))) 
;;                                                '((I U E) (Magnin Thant Coli)))



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;
;;;;  Exercise D


;; (arg-max f xs) Takes as arguments a function f that maps a value in set A to
;; a number, and a nonempty list xs of values in set A. Returns an element
;; x in xs for which (f x) is as large as possible.

;; laws:
;;   (arg-max f (cons x '())) == x 
;;   (arg-max f (cons x xs')) == x where f(x) is max
;;   (arg-max f (cons x xs')) == (arg-max f xs') where f(x) is not max


(define arg-max (f xs)
    (if (null? (cdr xs))
        (car xs)
        (let ([sub-max (arg-max f (cdr xs))])
            (if (>= (f (car xs)) (f sub-max))
                (car xs)
                sub-max))))

        ;; replace next line with good check-expect or check-assert tests
        (define square (a) (* a a))
        (check-expect (arg-max square '(1)) 1)
        (check-expect (arg-max square '(5 4 3 2 1)) 5)
        (check-expect (arg-max square '(1 2 3)) 3)



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;
;;;;  Exercise E

(record point [x y])


;; (x-cord p) Takes as argument a point p and returns the x coordinate

;; laws:
;;   (x-cord p) == (point-x p)

(define x-cord (p)
    (point-x p))

        ;;
        (check-expect (x-cord (make-point 10 11)) 10)
        (check-expect (x-cord (make-point 0 11)) 0)


;; (rightmost-point ps) Takes as argument a nonempty list ps of point records
;; and returns the point with the largest x coordinate

;; laws:
;;   (rightmost-point (cons p '())) == p
;;   (rightmost-point (cons p ps')) == (arg-max x-cord ps)

(define rightmost-point (ps)
    (if (null? (cdr ps))
        (car ps)
        (arg-max x-cord ps)))


        ;; 
        (check-expect (rightmost-point '((make-point 1 2))) (make-point 1 2))
        (check-expect (rightmost-point '((make-point 1 2) (make-point 3 4) 
                                         (make-point 10 1) (make-point 5 5))) 
                                         (make-point 10 1))


