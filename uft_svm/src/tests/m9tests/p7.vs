.loadfun r0 1
	r2 := r1
	r2 := 5
	r3 := 1
	r0 := r2 + r3
	return r0
el
%changex := r0
r0 := %changex
r1 := 3
r0 := call r0 (r1, ..., r1)
