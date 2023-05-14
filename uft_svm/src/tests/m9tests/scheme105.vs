.loadfun r0 1
	r2 := emptylist
	r0 := r1 cons r2
	return r0
el
%list1 := r0
.loadfun r0 2
	r3 := %list1
	r4 := r2
	r3 := call r3 (r4, ..., r4)
	r0 := r1 cons r3
	return r0
el
%list2 := r0
.loadfun r0 2
	r3 := null? r1
	if r3 goto L1:
	r3 := %list2
	r4 := car r1
	r5 := car r2
	r3 := call r3 (r4, ..., r5)
	r4 := %zip
	r5 := cdr r1
	r6 := cdr r2
	r4 := call r4 (r5, ..., r6)
	r0 := r3 cons r4
	return r0
	goto L2:
	L1:
	return r1
	L2:
el
%zip := r0
r0 := %zip
r1 := 1
r2 := emptylist
r1 := r1 cons r2
r2 := "a"
r3 := emptylist
r2 := r2 cons r3
r0 := call r0 (r1, ..., r2)
