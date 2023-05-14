(define parity (n)
  (letrec ([odd?  (lambda (m) (if (= m 0) #f (even? (- m 1))))]
           [even? (lambda (m) (if (= m 0) #t (odd? (- m 1))))])
    (if (odd? n) 'odd 'even)))

(check-expect (parity 0) 'even)
(check-expect (parity 1) 'odd)
(check-expect (parity 30) 'even)
(check-expect (parity 91) 'odd)

; (val f (let ([x 2]) (lambda () (set x 1))))
; (f)

(check-expect (letrec ([fact (lambda (m) (if (= 0 m) 1 (* m (fact (- m 1)))))]) (fact 5)) 120)


;;;;;;;;;;;;;;;;;;; COMP 105 SCHEME ASSIGNMENT ;;;;;;;;;;;;;;;




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;
;;;;  Exercise 1


;; (is-prefix? prefix xs) Determines if pre is a prefix of xs.
;; That is to say it will return true if and only if the following
;; hold. 
;; 1. xs is longer than or equal to the length of prefix.
;; 2. The first (length prefix) elements of xs are identical
;;    to prefix. That is to say 
;;    (append prefix (drop (length prefix) xs)) == xs
;; It is undefined behavior for either xs or prefix to not be in set 
;; LIST (ATOM)

;; laws:
;;   (is-prefix? '() ys) == #t
;;   (is-prefix? (cons a pre) '()) == #f
;;   (is-prefix? (cons y pre) (cons y ys)) == (is-prefix? pre ys)
;;   (is-prefix? (cons x pre) (cons y ys)) == #f, (not (= y x))
(define is-prefix? (pre xs)
    (|| (null? pre) 
        (&& (not (null? xs)) 
            (= (car pre) (car xs))
            (is-prefix? (cdr pre) (cdr xs)))))

        ;; Replace the next lines with good check-expect or check-assert tests
        (check-assert (is-prefix? '() '()))
        (check-assert (is-prefix? '() '(2 3)))
        (check-assert (is-prefix? '(2) '(2 3)))
        (check-assert (not (is-prefix? '(2) '(3 4))))
        (check-assert (not (is-prefix? '(2 3 4) '(2 3))))


;; (contig-sublist? xs ys) When both xs and ys are in set LIST (ATOM ), 
;; contig-sublist? determines whether the first list is a contiguous 
;; subsequence of the second. That is, (contig-sublist? xs ys) returns #t 
;; if and only if there are two other lists front and back, such that ys is 
;; equal to (append (append front xs) back).

;; laws: too challenging :/

(define contig-sublist? (xs ys)
    (|| (null? xs) 
        (&& (not (null? ys)) 
            (|| (is-prefix? xs ys) (contig-sublist? xs (cdr ys))))))

        ;; replace next line with good check-expect or check-assert tests
        (check-assert (contig-sublist? '(2) '(2)))
        (check-assert (contig-sublist? '(2) '(1 2 3)))
        (check-assert (not (contig-sublist? '(1 2 4) '(2 3))))
        (check-assert (not (contig-sublist? '(1 2 5) '(1 2 4 1 2 3 5))))
        (check-assert (contig-sublist? '(1 2 3) '(1 2 4 1 2 3 5)))
        (check-assert (contig-sublist? '() '(1 2 4 1 2 3 5)))
        (check-assert (not (contig-sublist? '(1 2 3) '(1 2))))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;
;;;;  Exercise 8


;; (mirror xs) When xs is a list of S-expressions, (mirror xs) returns a list 
;; in which every list in xs is recursively mirrored, and the resulting lists
;; are in reverse order.

;; laws:
;;   (mirror '()) == '()
;;   (mirror (cons y ys)) == (append (mirror ys) (list1 y)), y is an ATOM
;;   (mirror (cons y ys)) == (append (mirror ys) (list1 (mirror y))), y is LIST

(define mirror (xs)
    (if (null? xs)
        '()
        (if (atom? (car xs)) 
            (append (mirror (cdr xs)) (list1 (car xs)))
            (append (mirror (cdr xs)) (list1 (mirror (car xs)))))))

        ;; replace next line with good check-expect or check-assert tests
        (check-expect (mirror '(1 2 3 4 5)) '(5 4 3 2 1))
        (check-expect (mirror '((a (b 5)) (c d) e)) '(e (d c) ((5 b) a)))


;; (flatten xs) When xs is a list of S-expressions, (flatten xs) constructs a 
;; list having the same atoms as xs in the same order, but in a flat list.
;; Though not defined in the spec, I define for convenience that
;; flattening an atom returns that atom in a list

;; laws:
;;   (flatten '()) == '()
;;   (flatten y) == '(y)
;;   (flatten (cons y ys)) == (append (flatten y) (flatten ys))

(define flatten (xs)
    (if (null? xs)
        xs
        (if (atom? xs)
            (list1 xs)
            (append (flatten (car xs)) (flatten (cdr xs))))))

        ;; replace next line with good check-expect or check-assert tests
        (check-expect 
            (flatten '((I Ching) (U Thant) (E Coli))) 
            '(I Ching U Thant E Coli))
        (check-expect 
            (flatten '(((((a))))))
            '(a))
        (check-expect 
            (flatten '())
            '())
        (check-expect 
            (flatten '((a) () ((b c) d e)))
            '(a b c d e))



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;
;;;;  Exercise 31


;; (takewhile p? xs) Takes a predicate p? and a list xs and returns the longest
;; prefix of the list in which every element satisfies the predicate

;; laws:
;;   (takewhile p? '()) == '()
;;   (takewhile p? (cons y ys)) == (cons y (takewhile p? ys)), (p? y)
;;   (takewhile p? (cons y ys)) == '(), (not (p? y))

(define takewhile (p? xs)
    (if (null? xs)
        xs
        (if (p? (car xs))
            (cons (car xs) (takewhile p? (cdr xs)))
            '()))) 


        ;; replace next line with good check-expect or check-assert tests
        (check-expect (takewhile (lambda (x) (= (mod x 2) 0)) 
                        '(2 4 6 7 8 10 12)) 
                        '(2 4 6))
        (check-expect (takewhile (lambda (x) (= (mod x 2) 0)) 
                        '(3 4 6 7 8 10 12)) 
                        '())
        (check-expect (takewhile (lambda (x) (= (mod x 2) 0)) 
                        '(2 4 6 8 10 12)) 
                        '(2 4 6 8 10 12))
        (check-expect (takewhile (lambda (x) (= (mod x 2) 0)) 
                        '()) 
                        '())
        (check-expect (takewhile null? '(())) '(()))
        (check-expect (takewhile null? '(() () (2))) '(() ()))



;; (dropwhile p? xs) Takes a predicate p? and a list xs and removes the longest
;; prefix of the list in which every element satisfies the predicate

;; laws:
;;   (dropwhile p? '()) == '()
;;   (dropwhile p? (cons y ys)) == (dropwhile p? ys), (p? y)
;;   (dropwhile p? (cons y ys)) == (cons y ys), (not (p? y))

(define dropwhile (p? xs)
    (if (null? xs)
        xs
        (if (p? (car xs))
            (dropwhile p? (cdr xs))
            xs)))

        ;; replace next line with good check-expect or check-assert tests
        (check-expect (dropwhile (lambda (x) (= (mod x 2) 0)) 
                        '(2 4 6 7 8 10 12))
                        '(7 8 10 12))
        (check-expect (dropwhile (lambda (x) (= (mod x 2) 0))
                        '(3 4 6 7 8 10 12))
                        '(3 4 6 7 8 10 12))
        (check-expect (dropwhile (lambda (x) (= (mod x 2) 0))
                        '(2 4 6 8 10 12))
                        '())
        (check-expect (dropwhile (lambda (x) (= (mod x 2) 0))
                        '())
                        '())
        (check-expect (dropwhile null? '(())) '())
        (check-expect (dropwhile null? '(() () (2))) '((2)))




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;
;;;;  Exercise B


;; (take n xs) When n is a natural number and xs is a list of values, returns
;; the longest prefix of xs that contains at most n values.

;; laws:
;;   (take 0 ys) == '()
;;   (take n '()) == '()
;;   (take n (cons y ys)) == (cons y (take (- n 1) ys))

(define take (n xs)
    (if (|| (null? xs) (= n 0))
        '()
        (cons (car xs) (take (- n 1) (cdr xs)))))

        ;; replace next line with good check-expect or check-assert tests
        (check-expect (take 0 '(a b c)) '())
        (check-expect (take 1 '(a b c)) '(a))
        (check-expect (take 3 '(a b c)) '(a b c))
        (check-expect (take 4 '(a b c)) '(a b c))
        (check-expect (take 4 '((a) () (b () hello ((d))) c)) 
                        '((a) () (b () hello ((d))) c))
        (check-expect (take 3 '((a) () (b () hello ((d))) c)) 
                        '((a) () (b () hello ((d)))))
        (check-expect (take 2 '((a) () (b () hello ((d))) c)) 
                        '((a) ()))



;; (drop n xs) When n is a natural number and xs is a list of values, returns 
;; xs without the first n elements. If xs has fewer than n elements, returns
;; and empty list.

;; laws:
;;   (drop 0 ys) == ys
;;   (drop n '()) == '()
;;   (drop n (cons y ys)) == (drop (- n 1) ys)

(define drop (n xs)
    (if (|| (null? xs) (= n 0))
        xs
        (drop (- n 1) (cdr xs))))

        ;; replace next line with good check-expect or check-assert tests
        (check-expect (drop 0 '(a b c)) '(a b c))
        (check-expect (drop 1 '(a b c)) '(b c))
        (check-expect (drop 3 '(a b c)) '())
        (check-expect (drop 4 '(a b c)) '())
        (check-expect (drop 4 '((a) () (b () hello ((d))) c)) '())
        (check-expect (drop 3 '((a) () (b () hello ((d))) c)) '(c))
        (check-expect (drop 1 '((a) () (b () hello ((d))) c)) 
                        '(() (b () hello ((d))) c))



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;
;;;;  Exercise C


;; (zip xs ys) Given two lists of equal length, xs and ys, creates a new list
;; of pairs such that the ith element in the new list is the 2-list
;; (ith element in xs, ith element in ys)

;; laws:
;;   (zip '() '()) == '()
;;   (zip (cons a as) (cons b bs)) == (append (list1 (list2 a b)) (zip as bs))

(define zip (xs ys)
    (if (|| (null? xs) (null? ys))
        xs
        (append (list1 (list2 (car xs) (car ys))) (zip (cdr xs) (cdr ys)))))

        ;; replace next line with good check-expect or check-assert tests
        (check-expect (zip '(1 2 3) '(a b c))
            '((1 a) (2 b) (3 c)))
        (check-expect (zip '(11 11 15) '(Guyer Sheldon Korman))
            '((11 Guyer) (11 Sheldon) (15 Korman)))



;; (unzip ps) Given a list of pairs (lists of length 2), ps, returns a pair of
;; lists such that the ith element in the first returned list is (car ps[i])
;; and the ith element in the second returned list is (cadr ps[i]).
;; Here ps[i] represents the ith element of the input list, ps.

;; laws: Too challenging :/

(define unzip (ps)
    (foldr (lambda (xs acc) (list2 (append (list1 (car xs)) (car acc))
                                    (append (list1 (cadr xs)) (cadr acc))))
            '(() ())
            ps))

        ;; replace next line with good check-expect or check-assert tests
        (check-expect (unzip '((1 a) (2 b) (3 c)))
                        '((1 2 3) (a b c)))
        (check-expect (unzip '((11 Guyer) (11 Sheldon) (15 Korman)))
                        '((11 11 15) (Guyer Sheldon Korman)))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;
;;;;  Exercise D


;; (arg-max f xs) Expects two arguments: a function f that maps a value in 
;; set A to a number, and a nonempty list as of values in set A. 
;; It returns an element a in as for which (f a) is as large as possible. 

;; laws:
;;   (arg-max f (cons y ys)) == y, (> (f y) (f (arg-max f ys)))
;;   (arg-max f (cons y ys)) == (arg-max f ys), (<= (f y) (f (arg-max f ys)))

(define arg-max (f xs)
    (foldr (lambda (x acc) (if (< (f acc) (f x)) x acc))
            (car xs)
            xs))

        ;; replace next line with good check-expect or check-assert tests
        (check-expect (arg-max car '((105 PL) (160 Algorithms) (170 Theory)))
                        '(170 Theory))
        (check-expect (arg-max (lambda (a) (* a a)) '(5 4 3 2 1)) 5)
        (check-expect (arg-max (lambda (a) (* a a)) '(5 4 -6 2 1)) -6)



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;
;;;;  Exercise E


;; (rightmost-point ps) Takes a nonempty list of point records, ps, and returns
;; the one with the largest x coordinate

;; laws: No laws :)

(record point [x y])
(define rightmost-point (ps)
    (arg-max point-x ps))

        ;; replace next line with good check-expect or check-assert tests
        (check-expect (rightmost-point '((make-point 2 3) (make-point -1 4))) 
                        (make-point 2 3))
        (check-expect (rightmost-point '((make-point -1 4))) 
                        (make-point -1 4))

;; CS 105 - Programming Languages
;; Higher Order Functions
;; Fall 2022

;;
;; Problem 28 (b,e,f)
;;

;; (max* xs) When xs is a list of integers, (max* xs) returns the greatest
;; element in the list.

(define max* (xs)
    (foldl max (car xs) xs))

        (check-expect (max* '(0 1 2 3 4 5)) 5)
        (check-expect (max* '(0)) 0)
        (check-expect (max* '(-1 -2 -3 -4)) -1)


;; (sum xs) When xs is a list of integers, (sum xs) returns the sum of all
;; the element in the list.

(define sum (xs) 
    (foldl + 0 xs))

        (check-expect (sum '(0 1 2 3 4 5)) 15)
        (check-expect (sum '(0)) 0)
        (check-expect (sum '(-1 -2 -4 -3)) -10)


;; (product xs) When xs is a list of integers, (product xs) returns the product
;; of all the element in the list.

(define product (xs)
    (foldl * 1 xs))

        (check-expect (product '(1 2 3 4 5)) 120)
        (check-expect (product '(0)) 0)
        (check-expect (product '(-1 -2 -4 -3)) 24)
        (check-expect (product '(-1 -2 0 -3)) 0)



;;
;; Problem 29 (a,c)
;;

;; (append xs ys) When xs and ys are both lists, returns a single list,
;; xs followed by ys.

(define append (xs ys)
    (foldr cons ys xs))

        (check-expect (append '() '()) '())
        (check-expect (append '() '(1)) '(1))
        (check-expect (append '(1 2) '()) '(1 2))
        (check-expect (append '(1 2 3) '(2 3 4)) '(1 2 3 2 3 4))


;; (reverse xs) When xs is a list, returns xs but with its elements backwards.

(define reverse (xs)
    (foldl cons '() xs))

        (check-expect (reverse '()) '())
        (check-expect (reverse '(1)) '(1))
        (check-expect (reverse '(1 2)) '(2 1))
        (check-expect (reverse '(2 3 4 5)) '(5 4 3 2))



;;
;; Problem 30
;;

;; (map f xs) When xs is a list, returns a list of equal length where
;; the ith element in the returned list is equal to (f xs[i]). xs[i] denotes
;; the ith element in the input list.

(define map (f xs)
    (foldr (lambda (x acc) (cons (f x) acc)) '() xs))

        (check-expect (map (lambda (x) (* x x)) '(2 3 4 5)) '(4 9 16 25))
        (check-expect (map (lambda (x) (* x x)) '()) '())
        (check-expect (map ((curry +) 1) '(2 3 4)) '(3 4 5))


;; (filter p? xs) When xs is a list, returns a list containing all the
;; elements of xs, denote each one x, such that (= (p? x) #t). The relative
;; order of elements is maintained.

(define filter (p? xs)
    (foldr (lambda (x acc) (if (p? x) (cons x acc) acc)) '() xs))

        (check-expect (filter (lambda (x) (= x 5)) '(2 3 4 5)) '(5))
        (check-expect (filter (lambda (x) (= x 5)) '()) '())
        (check-expect (filter (lambda (x) (= (mod x 2) 0)) '(2 3 4)) '(2 4))
        (check-expect (filter (lambda (x) (= (mod x 2) 0)) '(1 3 5)) '())


;; (exists? p? xs) When xs is a list, denote a given element by x.
;; Evaluates (p? x) from left to right and returns #t once an x is found
;; such that (p? x) is true.

(define exists? (p? xs)
    (foldl (lambda (x acc) (|| acc (p? x))) #f xs))

        (check-assert (exists? (lambda (x) (= x 5)) '(2 3 4 5)))
        (check-assert (not (exists? (lambda (x) (= x 5)) '())))
        (check-assert (exists? (lambda (x) (= (mod x 2) 0)) '(2 3 4)))
        (check-assert (not (exists? (lambda (x) (= (mod x 2) 0)) '(1 3 5))))
        (check-assert  (exists? (lambda (x) (= (mod x 2) 0)) '(2 '() 5)))


;; (all? p? xs) When xs is a list, denote a given element by x.
;; Evaluates (p? x) from left to right and returns #f once an x is found
;; such that (p? x) is false.

(define all? (p? xs)
    (foldl (lambda (x acc) (&& acc (p? x))) #t xs))

        (check-assert (not (all? (lambda (x) (= x 5)) '(2 3 4 5))))
        (check-assert (all? (lambda (x) (= x 5)) '()))
        (check-assert (all? (lambda (x) (= x 5)) '(5 5)))
        (check-assert (not (all? (lambda (x) (= (mod x 2) 0)) '(2 3 4))))
        (check-assert (all? (lambda (x) (= (mod x 2) 0)) '(2 4 6)))
        (check-assert (not (all? (lambda (x) (= (mod x 2) 0)) '(1 '() 2))))



;;
;; Problem 38 (a,b,c,d)
;;

;; (member? x s) When s is a set (such as those defined below) this predicate
;; checks for membership. That is to say returns #t if and only if
;; x is in s, and false otherwise.

(define member? (x s) (s x))


;; (evens x) This function is actually a set. Membership should be checked
;; with the member? method. The elements of the set are all the numbers
;; that are divisible by 2.

(define evens (x) 
    (&& (number? x) (|| (= x 0) (= (mod x 2) 0))))

        (check-assert (not (member? '() evens)))
        (check-assert (member? 0 evens))
        (check-assert (member? 2 evens))
        (check-assert (not (member? 3 evens)))


;; (two-digits x) This function is actually a set. Membership should be checked
;; with the member? method. This set contains all the numbers in the interval
;; [10, 99]

(define two-digits (x) 
    (&& (number? x) (> x 9) (< x 100)))

        (check-assert (not (member? '() two-digits)))
        (check-assert (member? 10 two-digits))
        (check-assert (member? 50 two-digits))
        (check-assert (not (member? 100 two-digits)))
        (check-assert (not (member? 120 two-digits)))


;; (add-element x s) Set operation. Given an element x and a set s, returns
;; a set containing all elements of s in addition to x

;; Laws:
;; (member? x (add-element x s)) == #t
;; (member? x (add-element y s)) == (member? x s), where (not (equal? y x))

(define add-element (x s)
    (lambda (y) (|| (equal? x y) (member? y s))))

        (check-assert (member? 2 (add-element 2 (lambda (x) #f))))
        (check-assert (not (member? 2 (add-element 3 (lambda (x) #f)))))
        (check-assert (member? 3 (add-element 3 evens)))
        (check-assert (not (member? 100 (add-element 101 two-digits))))
        (check-assert (member? 100 (add-element 100 two-digits)))


;; (union s1 s2) Set operation. Given two sets, s1 and s2, returns a set
;; containing all elements of s1 in addition to all elements of s2.

;; Laws:
;; (member? x (union s1 s2))     == (|| (member? x s1) (member? x s2))

(define union (s1 s2)
    (lambda (x) (|| (member? x s1) (member? x s2))))

        (check-assert (member? 2 (union (lambda (x) (= x 2)) (lambda (x) #f))))
        (check-assert (not (member? 1 (union (lambda (x) (= x 3)) evens))))
        (check-assert (member? 8 (union two-digits evens)))
        (check-assert (not (member? 7 (union two-digits evens))))
        (check-assert (member? 50 (union two-digits evens)))


;; (inter s1 s2) Set operation. Given two sets, s1 and s2, returns a set
;; containing only the elements that are in both s1 and s2.

;; Laws:
;; (member? x (inter s1 s2))     == (&& (member? x s1) (member? x s2))

(define inter (s1 s2)
    (lambda (x) (&& (member? x s1) (member? x s2))))

        (check-assert (not (member? 2 (inter evens (lambda (x) #f)))))
        (check-assert (not (member? 57 (inter evens two-digits))))
        (check-assert (member? 56 (inter evens two-digits)))
        (check-assert (member? 100 (inter evens evens)))


;; (diff s1 s2) Set operation. Given two sets, s1 and s2, returns a set
;; containing only the elements that are in s1 but not in s2.

;; Laws:
;; (member? x (diff  s1 s2))     == (&& (member? x s1) (not (member? x s2)))

(define diff (s1 s2)
    (lambda (x) (&& (member? x s1) (not (member? x s2)))))

        (check-assert (member? 4 (diff evens (lambda (x) #f))))
        (check-assert (not (member? 56 (diff evens two-digits))))
        (check-assert (member? 100 (diff evens two-digits)))
        (check-assert (not (member? 0 (diff two-digits evens))))
        (check-assert (member? 75 (diff two-digits evens)))
        (check-assert (not (member? 76 (diff two-digits evens))))


;; set-ops: This record gives the client a means to store functions for
;; certain set operations all based on the same equality predicate.
(record set-ops [emptyset member? add-element union inter diff])


;; (set-ops-from eq?) Returns a record containing all the above set operations
;; that uses the given equality predicate

(define set-ops-from (eq?)
  (let ([emptyset    (lambda (x) #f)]
        [member?     (lambda (x s) (s x))]
        [add-element (lambda (x s) (lambda (y) (|| (eq? x y) (member? y s))))]
        [union       (lambda (s1 s2) (lambda (x) (|| (member? x s1)
                                                     (member? x s2))))]
        [inter       (lambda (s1 s2) (lambda (x) (&& (member? x s1)
                                                     (member? x s2))))]
        [diff        (lambda (s1 s2) (lambda (x) (&& (member? x s1)
                                                     (not (member? x s2)))))])
    (make-set-ops emptyset member? add-element union inter diff)))


        ;; Provided unit-tests
        (check-assert (function? set-ops-from))
        (check-assert (set-ops? (set-ops-from =)))

        ;; Definitions for custom unit-tests
        (val atom-set-ops     (set-ops-from =))
        (val atom-emptyset    (set-ops-emptyset    atom-set-ops))
        (val atom-member?     (set-ops-member?     atom-set-ops))
        (val atom-add-element (set-ops-add-element atom-set-ops)) 
        (val atom-union       (set-ops-union       atom-set-ops))
        (val atom-inter       (set-ops-inter       atom-set-ops))
        (val atom-diff        (set-ops-diff        atom-set-ops))

        ;; Custom unit-tests
        (check-assert (not (atom-member? 2 atom-emptyset)))
        (check-assert (atom-member? 2 (atom-add-element 2 atom-emptyset)))
        (check-assert (atom-member? 2 
                          (atom-union 
                              (atom-add-element 2 atom-emptyset)
                              atom-emptyset)))
        (check-assert (not (atom-member? 2 
                               (atom-inter 
                                   (atom-add-element 2 atom-emptyset)
                                   atom-emptyset))))
        (check-assert (atom-member? 2 
                          (atom-diff 
                              (atom-add-element 2 atom-emptyset)
                              atom-emptyset)))
        (check-assert (not (atom-member? 2 
                             (atom-diff 
                              atom-emptyset
                              (atom-add-element 2 atom-emptyset)))))



;;
;; Problem F
;;

;; (flip f) When f is a function that accepts two arguments, returns the same
;; function but with the order of arguments reversed.

;; Laws:
;;((flip f) x y) == (f y x)

(define flip (f) (lambda (x y) (f y x)))

        (check-assert ((flip >) 2 3))
        (check-assert (not ((flip <=) 2 3)))
        (check-expect ((flip append) '(2 3 4) '(1 2 3)) '(1 2 3 2 3 4))
        (check-expect ((flip -) 1 2) 1)



;;
;; Problem O
;;

;; (precedes? p?) When p? is a comparison function that represents a transitive
;; relation, precedes? returns a predicate that tells if a list of values is
;; totally ordered by that relation

;; Laws:
;; ((precedes? p?) '()) == #t
;; ((precedes? p?) (cons x '())) == #t
;; ((precedes? p?) (cons x (cons y zs)) == (&& (precedes? x y) 
;;                                             ((precedes? p) (cons y zs)))

(define ordered-by? (precedes?) 
    (letrec ([f (lambda (xs) 
                            (if (|| 
                                    (null? xs) 
                                    (null? (cdr xs)))
                                #t
                                (&& 
                                    (precedes? (car xs) (cadr xs)) 
                                    (f (cdr xs)))))]) f))

        (check-assert (function? ordered-by?))
       ;; (check-assert (function? (ordered-by? <)))
        (check-assert ((ordered-by? <) '(1 2 3)))
        (check-assert ((ordered-by? <=) '(1 2 3)))
        (check-assert (not ((ordered-by? <) '(3 2 1))))
        (check-assert ((ordered-by? >=) '(3 2 1)))
        (check-assert ((ordered-by? >=) '(3 3 3)))
        (check-assert ((ordered-by? =) '(3 3 3)))



;;
;; Probelm V
;;

;; Definitions for validator
(val the-fault list1) ; build a singleton fault set
(val no-faults '())   ; an empty fault set


;; (faults/none) Returns a validator that, when given any response, returns
;; and empty list of faults.

;; Laws:
;; ((faults/none) response) == no-faults

(define faults/none ()
    (lambda (response) no-faults))

        (check-expect ((faults/none) '([why photo])) no-faults)


;; (faults/always f) Given any value f, returns a validator that, when
;; given any response, returns a list of faults containing only f.

;; Laws:
;; ((faults/always f) response) == (the-fault f)

(define faults/always (f)
    (lambda (response) (the-fault f)))

        (check-expect ((faults/always 'x) '([why photo])) '(x))
        (check-expect ((faults/always '()) '([why photo])) '(()))
        (check-expect ((faults/always 'x) '()) '(x))


;; (bound-in? key pairs) When given an association list pairs and a key in the
;; set ATOMS, returns true if and only if the given key exists in the
;; association list. False otherwise.

;; Laws:
;; (bound-in k '()) == #f
;; (bound-in k (cons (cons k' a) xs))) == (|| (= k k') (bound-in? k xs))

(define bound-in? (key pairs)
  (if (null? pairs)
      #f
      (|| (equal? key (alist-first-key pairs))
          (bound-in? key (cdr pairs)))))

        
        (check-assert (not (bound-in? 'k '())))
        (check-assert (bound-in? 'k '([k ()])))
        (check-assert (bound-in? 'k '([x y][k ()])))
        (check-assert (not (bound-in? 'k '([x y][z ()]))))
        (check-assert (bound-in? '(()) '([x y][(()) ()])))


;; (faults/equal key value) When given a key and value, returns a validator.
;; The returned validator accepts an association-list as an argument.
;; The returned validator given an association-list called response will 
;;   return '() if key does not appear in the keys of response or if
;;   the key is appears in the keys of response but the value associated
;;   with key does not equal value.
;; The return validator given an association-list called response will return
;;   a singleton list with the key if the value associated with key in
;;   responses is the value given to faults/equal.

;; Laws:
;; ((faults/equal k v) response) == '(),  if k is not bound in the response
;; ((faults/equal k v) response) == '(),  if k is a key in response but
;;                                        (not (equal? (find k response) v))
;; ((faults/equal k v) response) == '(k), otherwise

(define faults/equal (key value)
    (lambda (response) (if (|| 
                                (not (bound-in? key response)) 
                                (not (equal? (find key response) value)))
                            '()
                            (the-fault key))))

        (check-expect ((faults/equal 0 1) '([1 1])) '())
        (check-expect ((faults/equal 0 1) '([0 1])) '(0))
        (check-expect ((faults/equal 0 1) '([0 2])) '())
        (check-expect ((faults/equal 'x '()) '([x ()])) '(x))


;; (faults/both V1 V2) When given two validators V1, V2 returns a new
;; validator which represents the union of the two.

;; Laws:
;; (exists? ((curry =) f) ((faults/both V1 V2) response)) == #t,
;;      f is a fault in either of the lists (V1 response) or (V2 response)
;; (exists? ((curry =) f) ((faults/both V1 V2) response)) == #f,
;;      otherwise

(define faults/both (V1 V2)
    (let* ([member?  (lambda (x s) (exists? ((curry =) x) s))]
           [add-elem (lambda (x s) (if (member? x s) s (cons x s)))]
           [union    (lambda (faults1 faults2)
                        (foldr add-elem faults2 faults1))])
        (lambda (response) (union (V1 response) (V2 response)))))

        (check-expect ((faults/both (faults/always 'x) (faults/none)) '())
            '(x))
        (check-expect ((faults/both (faults/always 'x) (faults/always 'y)) '())
            '(x y))
        (check-expect ((faults/both (faults/none) (faults/none)) '())
            '())
        (check-expect ((faults/both (faults/equal 'x 'y) (faults/none))
                        '([x y]))
                        '(x))


;; (faults/switch key validators) Given a key k, and a validator table, 
;; if key k is bound in the response to a value v that is bound in the 
;; validator table returns to corresponding validator.


;; Laws:
;; ((faults/switch k v) response) == ((find (find k response) v) 
;;                                                      response)

(define faults/switch (key validators)
    (lambda (response) ((find (find key response) validators) response)))


        (val testValidator (faults/switch 'why (bind 'photo (faults/always 'f) 
                                                    '())))
        (check-expect (testValidator '([why photo])) '(f))

        (set testValidator (faults/switch 'why (bind 'photo (faults/always 'f)
                                                (bind 'test (faults/none) 
                                                 '()))))
        (check-expect (testValidator '([why photo])) '(f))
        (check-expect (testValidator '([why test])) '())
        ;; Extra tests in validator-tests.scm

(val regrade-validator  ;; example for the regrade form
  (faults/switch 'why
    (bind         'photo
                  (faults/none)
      (bind       'badsubmit
                   (faults/both (faults/equal 'badsubmit_asst '...)
                                (faults/equal 'info #f))
        (bind     'badgrade
                  (faults/both
                      (faults/equal 'badgrade_asst '...)
                      (faults/both
                         (faults/equal 'info #f)
                         (faults/equal 'problem #f)))
          (bind   'recitation
                  (faults/both
                      (faults/equal 'date #f)
                      (faults/equal 'leaders #f))

            (bind '#f
                  (faults/always 'nobutton)
                  '())))))))

(check-expect (regrade-validator '([why photo]))
              '())

(check-expect (regrade-validator '([why badsubmit]
                                  [info wrong-pdf]
                                  [badsubmit_asst opsem]))
              '())

(check-expect (regrade-validator '([why badsubmit]
                                  [badsubmit_asst opsem]
                                  [info #f]))
              '(info))

(check-expect (regrade-validator '([why badsubmit]
                                  [info wrong-pdf]
                                  [badsubmit_asst ...]))
              '(badsubmit_asst))

(check-expect (regrade-validator '([why badsubmit]
                                  [info #f]
                                  [problem #f]
                                  [badsubmit_asst ...]))
              '(badsubmit_asst info))

(check-expect (regrade-validator '([why badgrade]
                                  [info #f]
                                  [problem #f]
                                  [badgrade_asst impcore]))
              '(info problem))


;; CS 105 - Programming Languages
;; Continuations
;; Fall 2022

;;
;; Problem L
;;

;; (list-of? A? v) returns a Boolean that is #t if v is a list of values, each 
;; of which satisfies A?. Otherwise, (list-of? A? v) returns #f.

;; Laws:
;; (list-of? p? '()) == #t
;; (list-of? p? (cons x xs)) == (&& (p? x) (list-of? p? xs))
;; (list-of? p? v) == #f, v has none of the forms above

(define list-of? (A? v)
    (|| (null? v)
        (&& (not (atom? v)) 
            (A? (car v))
            (list-of? A? (cdr v)))))

        (check-assert (list-of? atom? '()))
        (check-assert (list-of? atom? '(1 abc 3 #f)))
        (check-assert (not (list-of? atom? '(1 (2 4) 3 #f))))
        (check-assert (not (list-of? atom? (cons '() (cons 2 (cons 'a #f))))))
        (check-assert (list-of? function? (cons cons (cons + '()))))
        (check-assert (not (list-of? function? (cons cons (cons + '(2))))))



;;
;; Problem F
;;

;; Definitions used in the rest of the homework. These definitions let us
;; represent boolean formulas
(record not [arg])
(record or  [args])
(record and [args])


;; (formula? f) when given an arbitrary μScheme value, returns #t if the value
;; represents a Boolean formula and #f otherwise. Uses the inductive 
;; definition of formulas given in the homework.

;; Laws:
;; (formula? f) == #t, f is a symbol
;; (formula? f) == (formula? (not-arg f)), f is a 'not' record
;; (formula? f) == (list-of? formula? (or-args f)), f is an 'or' record
;; (formula? f) == (list-of? formula? (and-args f)), f is an 'and' record
;; (formula? f) == #f, f does not have any of the above forms

(define formula? (f) 
    (|| (symbol? f)
        (&& (not? f) (formula? (not-arg f)))
        (&& (or? f) (list-of? formula? (or-args f)))
        (&& (and? f) (list-of? formula? (and-args f)))))

        (check-assert (formula? 'x))
        (check-assert (formula? (make-or '(x y z))))
        (check-assert (formula? (make-and '(x y z))))
        (check-assert (formula? (make-and (list2 'x (make-not 'x)))))
        (check-assert 
            (formula? 
                (make-and (list2 (make-or (list2 'x (make-not 'x))) 'y))))
        (check-assert (formula? (make-or '())))
        (check-assert (formula? (make-and '())))
        (check-assert (not (formula? '())))
        (check-assert (not (formula? 2)))
        (check-assert (not (formula? (make-or '(x y 2)))))
        (check-assert 
            (not (formula? (make-and (list2 'x (make-not make-and))))))
        (check-assert (not (formula? (make-and (make-or '(x))))))



;;
;; Problem E
;;

;; (eval-formula f env) When f is a boolean formula and env is an association
;; list such that every variable in f is bound in env, returns true if f is
;; satisified. If f is not satisfied, returns #f.

;; Laws:
;; (eval-formula x e) == 
;;     (find x e), where x is a symbol
;; (eval-formula (make-not x) e) == 
;;     (not (eval-formula x e))
;; (eval-formula (make-or xs) e) == 
;;     (exists? (lambda (x) (eval-formula x e)) xs)
;; (eval-formula (make-and xs) e) == 
;;     (all? (lambda (x) (eval-formula x e)) xs)

(define eval-formula (f env)
    (if (not (formula? f))
        (error 'BadFormula)
        (|| (&& (symbol? f) (find f env))
            (&& (not? f) 
                (not (eval-formula (not-arg f) env)))
            (&& (or? f) 
                (exists? (lambda (x) (eval-formula x env)) (or-args f)))
            (&& (and? f) 
                (all? (lambda (x) (eval-formula x env)) (and-args f))))))

        (check-assert (eval-formula 'x '([x #t])))
        (check-assert (not (eval-formula 'x '([x #f]))))
        (check-assert 
            (eval-formula (make-or '(x y z)) '([x #t][y #f][z #f])))
        (check-assert 
            (eval-formula (make-and '(x y z)) '([x #t][y #t][z #t])))
        (check-assert 
            (not (eval-formula (make-and '(x y z)) '([x #t][y #t][z #f]))))
        (check-assert 
            (eval-formula (make-and (list2 
                                        (make-or (list2 'x (make-not 'x))) 
                                        (make-not 'x))) '([x #f])))
        (check-assert 
            (not (eval-formula 
                    (make-and (list2 
                                (make-or (list2 'x (make-not 'x))) 
                                (make-not 'x))) '([x #t]))))



;;
;; Problem S
;;

;; (find-formula-true-asst f fail succ) given a boolean formula f, finds a
;; satisfying assignment for f. That is, an assignment to symbols in f such
;; that f is true. If such an assignment is found, the success continuation
;; is called with the assignment in the 

(define find-formula-true-asst (f fail succ)
    (if (not (formula? f))
        (error 'InvalidFormula)
        (letrec 
            ;; (find-formula-asst x            bool cur fail succeed) == 
            ;;     (find-formula-symbol x bool cur fail succeed)
            ;;     , where x is a symbol
            ;; (find-formula-asst (make-not f)  bool cur fail succeed) == 
            ;;     (find-formula-asst f (not bool) cur fail succeed)
            ;; (find-formula-asst (make-or  fs) #t   cur fail succeed) == 
            ;;     (find-any-asst fs #t cur fail succeed)
            ;; (find-formula-asst (make-or  fs) #f   cur fail succeed) ==
            ;;     (find-all-asst fs #f cur fail succeed)
            ;; (find-formula-asst (make-and fs) #t   cur fail succeed) == 
            ;;     (find-all-asst fs #t cur fail succeed)
            ;; (find-formula-asst (make-and fs) #f   cur fail succeed) == 
            ;;     (find-any-asst fs #f cur fail succeed)
            ([find-formula-asst 
                (lambda (x bool cur fail succeed)
                    (cond 
                        [(symbol? x) (find-formula-symbol 
                                            x bool cur fail succeed)]
                        [(not? x) (find-formula-asst (not-arg x) (not bool)
                                                      cur fail succeed)]
                        [bool (if (or? x)
                                  (find-any-asst (or-args x)
                                            bool cur fail succeed)
                                  (find-all-asst (and-args x)
                                           bool cur fail succeed))]
                        [(not bool) (if (and? x)
                                        (find-any-asst (and-args x) 
                                           bool cur fail succeed)
                                        (find-all-asst (or-args x) 
                                           bool cur fail succeed))]))]

            ;; (find-formula-symbol x bool cur fail succeed) == 
            ;;     (succeed (bind x bool cur) fail)
            ;;     , where x is not bound in cur
            ;; (find-formula-symbol x bool cur fail succeed) == 
            ;;     (succeed cur fail), where x is bool in cur
            ;; (find-formula-symbol x bool cur fail succeed) == 
            ;;     (fail), where x is (not bool) in cur
             [find-formula-symbol 
                (lambda (x bool cur fail succeed)
                    (if (= '() (find x cur))
                        (succeed (bind x bool cur) fail)
                        (if (= bool (find x cur))
                            (succeed cur fail)
                            (fail))))]

            ;; (find-all-asst '()         bool cur fail succeed) == 
            ;;     (succeed cur fail)
            ;; (find-all-asst (cons f fs) bool cur fail succeed) == 
            ;;     (find-formula-asst f bool cur 
            ;;         fail
            ;;         (lambda (cur resume) 
            ;;             (find-all-asst fs bool cur resume succeed)))
             [find-all-asst 
                (lambda (formulas bool cur fail succeed)
                    (if (null? formulas)
                        (succeed cur fail)
                        (find-formula-asst (car formulas) bool cur 
                            fail
                            (lambda (cur resume) 
                                (find-all-asst (cdr formulas) 
                                               bool cur resume succeed)))))]

            ;; (find-any-asst '()         bool cur fail succeed) == 
            ;;     (fail)
            ;; (find-any-asst (cons f fs) bool cur fail succeed) ==
            ;;     (find-formula-asst f bool cur 
            ;;         (lambda () (find-any-asst fs bool cur fail succeed))
            ;;         succeed
             [find-any-asst 
                (lambda (formulas bool cur fail succeed)
                    (if (null? formulas)
                        (fail)
                        (find-formula-asst (car formulas) bool cur 
                            (lambda () (find-any-asst (cdr formulas) 
                                                      bool cur fail succeed))
                            succeed)))]) 
            (find-formula-asst f #t '() fail succ))
        ))

; Template for SAT Solver Test Cases

(record not [arg])
(record or  [args])
(record and [args])


;; This test case is the first one that I realize my solution missed.
;; In an and, if we set something, we want to be able to go back and
;; change it later if need be. That's why the resume continuation is
;; necessary
(val f1 (make-and (list2 (make-or '(x y)) (make-not 'x))))
(val s1 '((y #t) (x #f)))

;; This test has no solution but it might be tricky for a solver to handle 
;; this. It might decide that we can make both x and y true if it doesn't 
;; handle the nested 'and' correctly
(val f2 (make-not (make-or (list2 'x (make-not (make-and (list2 'x 'y')))))))
(val s2 'no-solution)

;; A tricky test where there is
;; only one acceptable assignment.
(val f3 (make-and 
            (list3 'x 
                (make-or (list2 'y 'z)) 
                (make-not (make-and '(x y))))))
(val s3 '((x #t) (z #t) (y #f)))

        (check-expect (find-formula-true-asst f1 
                        (lambda () 'no-solution) 
                        (lambda (assignment _) assignment)) s1)

        (check-expect (find-formula-true-asst f2 
                        (lambda () 'no-solution) 
                        (lambda (assignment _) assignment)) s2)

        (check-expect (find-formula-true-asst f3
                        (lambda () 'no-solution) 
                        (lambda (assignment _) assignment)) s3)

        ;; Checking our resume continuation
        (check-expect (find-formula-true-asst (make-or '(x y))
                        (lambda () 'no-solution)
                        (lambda (assignment resume) assignment)) '((x #t)))

        (check-expect (find-formula-true-asst (make-or '(x y))
                        (lambda () 'no-solution)
                        (lambda (assignment resume) 
                            (if (equal? assignment '((x #t)))
                                (resume)
                                assignment))) '((y #t)))

        (check-expect (find-formula-true-asst (make-or '(x y))
                        (lambda () 'no-solution)
                        (lambda (assignment resume) 
                            (if (equal? assignment '((x #t)))
                                (resume)
                                (if (equal? assignment '((y #t)))
                                    (resume)
                                    assignment)))) 'no-solution)

        ;; Singleton and 
        (check-expect (find-formula-true-asst (make-and '(x)) 
                        (lambda () 'no-solution) 
                        (lambda (assignment _) assignment)) '((x #t)))

        ;; Singleton or 
        (check-expect (find-formula-true-asst (make-or '(x)) 
                        (lambda () 'no-solution) 
                        (lambda (assignment _) assignment)) '((x #t)))

        ;; Singleton not
        (check-expect (find-formula-true-asst (make-not 'x) 
                        (lambda () 'no-solution) 
                        (lambda (assignment _) assignment)) '((x #f)))