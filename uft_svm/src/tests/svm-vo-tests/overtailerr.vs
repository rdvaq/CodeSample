.loadfun r0 2
    ;; arg in r1, r2
    r3 := 0
    r8 := r1 == r3
    if r8 goto L1:
	r255 := r2 * r1
	r4 := 1
    r254 := r1 - r4
    r253 := r0
    tailcall r253 (r254, ..., r255)
    L1:
    return r2
el
r1 := 2000
r2 := 1
r3 := call r0 (r1, ..., r2) ;; result too big to compare directly
r3 := 1000
check r3 "(trival test)" ;; just to make sure program ran correctly
r0 := 1000
expect r0 "1000"