# 1206850249105214865408 = (2^16) * 18415073381122053
#                                   (0x416c6963650005)
N = int(input().strip())
e = int(input().strip())
t1 = int(input().strip())
t2 = int(input().strip())


cert = t2 * t1 % N

print(cert)
