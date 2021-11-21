import os, sys
import base64

keys = [ chr(x) for x in range(ord('A'),ord('Z')+1) ] + [ str(x) for x in range(0,6) ]
assert len(keys) == 32
charset = keys + ["a","b","c","d"]

a,b,c,d = "a","b","c","d"

for key in keys:
    a,b,c,d = b,c,d,a+c+key
    
def Simplify(s):
    result = ''
    for c in charset:
        if s.count(c)%2 == 1:
            result += c
    return result
    
a = Simplify(a)
b = Simplify(b)
c = Simplify(c)
d = Simplify(d)
print(a,b,c,d)
