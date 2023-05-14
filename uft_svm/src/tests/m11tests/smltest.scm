;; (let ([cons (lambda (x xs) (begin (gc) (cons x xs)))]) (cons 1 '()))

(gc)



;; vm-hw03{czhou06}146: uft ho-vo grow.scm | valgrind svm
;; ==882982== Memcheck, a memory error detector
;; ==882982== Copyright (C) 2002-2022, and GNU GPL'd, by Julian Seward et al.
;; ==882982== Using Valgrind-3.19.0 and LibVEX; rerun with -h for copyright info
;; ==882982== Command: svm
;; ==882982== 
;; Both tests passed.
;; ==882982== 
;; ==882982== HEAP SUMMARY:
;; ==882982==     in use at exit: 0 bytes in 0 blocks
;; ==882982==   total heap usage: 504,511,777 allocs, 656 frees, 24,303,969,839 bytes allocated
;; ==882982== 
;; ==882982== All heap blocks were freed -- no leaks are possible
;; ==882982== 
;; ==882982== For lists of detected and suppressed errors, rerun with: -s
;; ==882982== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)