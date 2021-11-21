import os
import struct
import matplotlib.pyplot as plt
import numpy as np

def sep(b1:bytes, b2:bytes):
    l1 = len(b1)
    l2 = len(b2)

    ab1 = []
    ab2 = []
    same = []
    diff = []
    
    i1 = 0
    i2 = 0

    while i1<l1 and i2<l2:
        if b1[i1] != b2[i2]:
            if (i1<l1-1 and b1[i1+1] == b2[i2] and b1[i1] != b2[i2+1]) or i1 in (3998, ):
                ab1.append(b1[i1])
                diff.append(b1[i1])
                i1+=1
            elif (i2<l2-1 and b1[i1] == b2[i2+1] and b1[i1+1] != b2[i2]) or i2 in (14484,14738):
                ab2.append(b2[i2])
                diff.append(b2[i2])
                i2+=1
            elif (b1[i1] == b2[i2+1] and b1[i1+1] == b2[i2] and\
                b1[i1+1] != b2[i2+2] and b1[i1+2] == b2[i2+1]):
                ab1.append(b1[i1])
                diff.append(b1[i1])
                i1+=1
            elif (b1[i1] == b2[i2+1] and b1[i1+1] == b2[i2] and\
                b1[i1+1] == b2[i2+2] and b1[i1+2] != b2[i2+1]):
                ab2.append(b2[i2])
                diff.append(b2[i2])
                i2+=1
            # if i1<l1-1 and i2<l2-1 and b1[i1+1] == b2[i2+1]:
            #     # same bit jump
            #     ab1.append(b1[i1])
            #     ab2.append(b2[i2])
            #     diff.append(b1[i1])
            #     diff.append(b2[i2])
            #     i1+=1
            #     i2+=1
            
            # elif i2<l2-2 and b1[i1] == b2[i2+2]:
            #     ab2.append(b2[i2])
            #     ab2.append(b2[i2+1])
            #     diff.append(b2[i2])
            #     diff.append(b2[i2+1])
            #     i2+=2
            # elif i1<l1-2 and b1[i1+2] == b2[i2]:
            #     ab1.append(b1[i1])
            #     ab1.append(b1[i1+1])
            #     diff.append(b1[i1])
            #     diff.append(b1[i1+1])
            #     i1+=2
            # elif i1<l1-5 and i2<l2-5 and b1[i1+5] == b2[i2+5]:
            #     # same bit jump
            #     for ii in range(5):
            #         ab1.append(b1[i1+ii])
            #         ab2.append(b2[i2+ii])
            #         diff.append(b1[i1+ii])
            #         diff.append(b2[i2+ii])
            #     i1+=5
            #     i2+=5
            # elif i1<l1-65 and i2<l2-65 and b1[i1+65] == b2[i2+65]:
            #     # same bit jump
            #     for ii in range(65):
            #         ab1.append(b1[i1+ii])
            #         ab2.append(b2[i2+ii])
            #         diff.append(b1[i1+ii])
            #         diff.append(b2[i2+ii])
            #     i1+=65
            #     i2+=65
            else:
                # print('ptn fail',i1,l1,i2,l2)
                raise(Exception('ptn fail'))
                # pass
        if i1<l1:
            same.append(b1[i1])

        if i2==l2-1:
            # not universal but nah
            assert i1==l1-2
            ab1.append(b1[i1+1])
            diff.append(b1[i1+1])
            same.append(b1[i1])
            break
        i1+=1
        i2+=1
    return (ab1, ab2, same, diff)

if __name__=='__main__':
    os.chdir('tbz2\To_the_past - 副本\MEMORY')
    with open('left.bin','rb') as f:
        b1 = f.read()
    with open('right.bin','rb') as f:
        b2 = f.read()
    ab1,ab2,same,diff=sep(b1,b2)
    # print([ab1, ab2, len(ab1), len(ab2)])
    with open('left_sep.bin','wb') as f:
        f.write(bytes(ab1))
    with open('right_sep.bin','wb') as f:
        f.write(bytes(ab2))
    with open('same_sep.bin','wb') as f:
        f.write(bytes(same))
    with open('diff.bin','wb') as f:
        f.write(bytes(diff))

    # with open('超级马里奥兄弟.nes','rb') as f:
    #     mario = f.read()

    # with open('diff.nes','rb') as f:
    #     diff = f.read()


    # ab1,ab2,same,diff2=sep(mario[:],diff[:])

    # plt.plot(np.arange(len(mario)), np.array(list(mario)),\
    #     np.arange(len(diff)), np.array(list(diff)))


    # ldiff = len(diff)-len(mario)
    # # shiftdiff = 0
    # for shiftdiff in range(ldiff):
    #     shift_mario = np.concatenate((np.ones((shiftdiff,),dtype=np.int32), list(mario), np.ones((ldiff-shiftdiff,),dtype=np.int32)))
    #     plt.plot(np.arange(len(diff)), shift_mario == np.array(list(diff)))
    #     # plt.show()
    #     plt.savefig('test_shift/test_shift%03d.png'%(shiftdiff,))
    #     plt.title('shift=%03d'%(shiftdiff,))
    #     plt.clf()

    print('done')