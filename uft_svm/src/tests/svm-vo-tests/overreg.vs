.loadfun r0 1
    r2 := 0
    r3 := 1
    r2 := r1 == r2
    if r2 goto L1:
    r255 := r1 - r3
    r254 := r0
    r3 := call r254 (r255, ..., r255)
    r1 := r1 * r3
    return r1
    L1:
    return r3
el
r1 := 78  ;;reg overflow
r3 := call r0 (r1, ..., r1)
check r3 "(factorial 77)"
r0 := 720
expect r0 "120"