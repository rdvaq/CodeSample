.loadfun r0 2
    ;; arg in r1, r2
    r3 := 0
    r8 := r1 == r3
    if r8 goto L1:
	r6 := r2 * r1
	r4 := 1
    r5 := r1 - r4
    r4 := r0
    tailcall r4 (r5, ..., r6)
    L1:
    return r2
el
;; check function correctness
r1 := 10
r2 := 1
r3 := call r0 (r1, ..., r2) 
check r3 "(fact 10)"
r10 := 3628800
expect r10 "3628800"

;; check function doesn't cause stack overflow
r1 := 2001
r2 := 1
r3 := call r0 (r1, ..., r2) ;; result too big to compare directly
r3 := 1000
check r3 "(trival test)" ;; just to make sure program ran correctly
r10 := 1000
expect r10 "1000"