stor2 = 0x15eea4b2551f0c96d02a5d62f84cac8112690d68c47b16814e221b8a37d6c4d3
stor3 = 0x293edea661635aabcd6deba615ab813a7610c1cfb9efb31ccc5224c0e4b37372

def func_0089(arg0):
    var0 = 0
    var1 = 0
    while var1 < 64:
        t =  ((((arg0 >> var1*4) + var1*5 + (stor2 >> var1*4)*7)&15) << (var1*4))
        var0 = var0 | t
        var1 += 1
        
    if var0 == stor3:
        print("Check Passed! Congratulations!")
    else:
        print(var0)
        print(stor3)
        print(stor3-var0)

def Reverse():
    arg0 = 0
    var0 = stor3
    for i in range(63,-1,-1):
        curVar0Bits = (var0 >> (i*4)) & 15
        curAdd = i*5 + (stor2 >> i*4)*7
        
        found = False
        for j in range(0,16):
            newArg0 = arg0 + (j << i*4)
            if (((newArg0>>i*4) + curAdd)&15) == curVar0Bits:
                arg0 = newArg0
                found = True
                break
        assert found
    return arg0
        
arg0 = Reverse()

func_0089(arg0)

print(arg0)

arg0hex = hex(arg0)[2:]
s = ''
for i in range(0,len(arg0hex),2):
    s += chr( int( arg0hex[i:i+2] , 16 ) )

print(s)
