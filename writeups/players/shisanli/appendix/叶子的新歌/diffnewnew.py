import os
i = 0
a = [0,0,0,0]
b = [0,0,0,0]
with open('left.bin','rb') as left:
    with open('right.bin','rb') as right:
        with open('diffnewnew.bin','wb') as diff:
            size = os.path.getsize('left.bin')
            while i < size:
                i = i + 1
                ldata = left.read(1)
                rdata = right.read(1)
                if ldata == rdata:
                    continue
                else:
                    i = i + 3
                    a[0] = ldata;
                    a[1] = left.read(1)
                    a[2] = left.read(1)
                    a[3] = left.read(1)
                    b[0] = rdata;
                    b[1] = right.read(1);
                    b[2] = right.read(1);
                    b[3] = right.read(1);
                    if a[0:3] == b[1:4]:
                        diff.write(rdata)
                        right.read(1)
                    elif a[1:4] == b[0:3]:
                        diff.write(ldata)
                        i = i + 1
                        left.read(1)
                    else:
                        raise Exception('Wrong data!'+str(hex(i))+\
                                        ' '+ldata.hex()+\
                                        ' '+rdata.hex())

                        
