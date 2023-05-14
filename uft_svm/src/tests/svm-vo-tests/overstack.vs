.loadfun r0 1
    ;; arg in r1
    r2 := 0
    r3 := 1
    r2 := r1 == r2
    if r2 goto L1:
    r5 := r1 - r3
    r4 := r0
    r3 := call r4 (r5, ..., r5)
    r1 := r1 * r3
    return r1
    L1:
    return r3
el
r1 := 2001 ;; should cause stack overflow
r3 := call r0 (r1, ..., r1)
check r3 "(factorial 5)"
r0 := 720
expect r0 "120"