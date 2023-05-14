;; (define parity (n)
;;   (letrec ([odd?  (lambda (m) (if (= m 0) #f (even? (- m 1))))]
;;            [even? (lambda (m) (if (= m 0) #t (odd? (- m 1))))])
;;     (if (odd? n) 'odd 'even)))

(define test (n) 
    (letrec ([addone (lambda (m) (+ m 1))]
             [increment (lambda (m) (addone m))])
    (increment n)))

(check-expect (test 1) 2)
;; (check-expect (parity 0) 'even)
;; (check-expect (parity 1) 'odd)
;; (check-expect (parity 30) 'even)
;; (check-expect (parity 91) 'odd)


;; (let ([r0 (lambda (r1) 
;;              (letrec  ([r2 
;;                 (mkclosure 
;;                    (lambda ($closure r1) (let ([r2 1]) (+ r1 r2))) 
;;                    '())] [r3 
;;                 (mkclosure 
;;                    (lambda ($closure r1) 
;;                       (let* ([r2 (CAPTURED-IN 0 $closure)]
;;                              [r3 r1]) 
;;                         (r2 r3))) 
;;                    (cons r2 '()))]) 
;;                 (let* ([r4 r3]
;;                        [r5 r1]) 
;;                   (r4 r5))))]) 
;;   (set test r0))

;; (let ([r0 (lambda (r1) 
;;              (let ([r4 r2]) 
;;                (letrec  ([r2 
;;                   (mkclosure 
;;                      (lambda ($closure r1) (let ([r2 1]) (+ r1 r2))) 
;;                      '())] [r3 
;;                   (mkclosure 
;;                      (lambda ($closure r1) 
;;                         (let* ([r2 (CAPTURED-IN 0 $closure)]
;;                                [r3 r1]) 
;;                           (r2 r3))) 
;;                      (cons r4 '()))]) 
;;                   (let* ([r4 r3]
;;                          [r5 r1]) 
;;                     (r4 r5)))))]) 
;;   (set test r0))




