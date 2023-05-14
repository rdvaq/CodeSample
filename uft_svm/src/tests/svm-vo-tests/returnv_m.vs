.loadfun r100 0
    r1 := 1852
    return r1
el
r50 := call r100
check r50 "result from calling function" 
r200 := 1852
expect r200 "value we expected" 
