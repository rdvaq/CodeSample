print r1
println r1
check r1 "hi"
expect r3 "hello"
halt 
r1 := r2
r1 := r2 + r3
r1 := r2 - r3
r1 := r2 * r3
r1 := r2 / r3
r1 := r2 cons r3
r1 := ! r2
r1 := pbool r2
GOTO section:
if r2
IF r1 GOTO section:
section:
r1 := "hi"
%gl := r1
r2 := %gl
r1 := r2 > r3
r1 := r2 < r3
r1 := r2 == r3
r1 := r2 idiv r3
printu r7
error r7
r7 := function? r9
r7 := pair? r9
r7 := symbol? r9
r7 := number? r9
r7 := boolean? r9
r7 := null? r9
r7 := nil? r9
r1 := car r4
r1 := cdr r3
r1 := hash r3