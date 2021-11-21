x = 672328094
y = 386900246
# x = 42
# y = 26
allnum = x * y * (x-1) * (y-1) * (x-2) * (y-2) // 6
# threenum = 2 * ((x-y+1) * y * (y-1) * (y-2) + 2 * \sum_{i=1}^{y-1} i * (i-1) * (i-2))
threenum = 2 * ((x-y+1) * y * (y-1) * (y-2) + y * (y-1) * (y-2) * (y-3) // 2) // 6
# twonum = 2 * ((x-y+1) * y * (y-1) + 2 * \sum_{i=1}^{y-1} i * (i-1)) // 2 * (x-2) * (y-2)
twonum = 2 * ((x-y+1) * y * (y-1) + 2 * (-2+y)*(-1+y)*y//3) // 2 * (x-2) * (y-2)
# twotwonum = \sum_{i=1}^{x}\sum_{j=1}^{y}(min(x-i,y-j)+min(i-1,j-1))*(min(x-i,j-1)+min(i-1,y-j))
# 不妨 i<=x//2, j<=y//2 => min(x-i,j-1)=j-1
# 1. min(i-1,y-j)=y-j 且不等
#    则 min(x-i,y-j)=y-j, min(i-1,j-1)=j-1
#    有 y-i+2 <= j <= y//2 i <= x//2 对 (y-1)*(y-1) 求和  # (sum 1 j=y-i+2 to y//2, i=y//2+2 to x//2) = 1//8 (x - y - 2) (x-y) 
#    此处求和为 (x - y - 2) *(x-y)* (y-1)*(y-1)//8
# 2. min(i-1,y-j)=i-1  y-j>=i-1  j<=y-i+1
#    2.1 min(x-i,y-j)=x-i
#        则 min(i-1,j-1)=j-1
#        有 1 <= j <= y-x+i i <= x//2 对 (x-i+j-1)*(i+j-2) 求和  # sum (x-i+j-1)*(i+j-2), j=1 to y-x+i, i=x-y+1 to x//2 = 1//96 (-2 + x) (x - 2 y) (-4 + x + 4 y) (x - 2 (1 + y))
#    2.2 min(x-i,y-j)=y-j 且不等   y-j<=x-i-1  j>=y-x+i+1
#        2.2.1 min(i-1,j-1)=i-1  j >= i 
#        有 i <= j <= y//2 i <= x//2 对 (y-j+i-1)*(i+j-2) 求和  # sum (y-j+i-1)*(i+j-2), j=i to y//2, i=1 to y//2 = 1//96 y (16 - 20 y - 4 y^2 + 5 y^3)
#        2.2.2 min(i-1,j-1)=j-1 且不等 j <= i-1
#        有 y-x+i+1 <= j <= min(i-1,y-i+1) i <= x//2 对 (y-1) * (i+j-2) 求和  #  (sum (i+j-2), j=y-x+i+1 to y-i+1, i=x-y+1 to x//2) + (sum (i+j-2), j=1 to y-i+1, i=y//2+1 to x-y)
#           + (sum (i+j-2), j=1 to i-1, i=2 to y//2) = sum((x-2i+1)*(i-2)+(2*y-x+2)(x-2i+1)//2), i=x-y+1 to x//2 +... = 1//24 (x - 2 y) (-2 + 2 x^2 + 6 y - 4 y^2 - x (3 + 2 y))
#           + 1//16 (y - 2)^2 y - 1//48 (2 x - 3 y) (4 x^2 - 6 x (y + 2) - 9 y^2 + 18 y + 8) for x element Z
twotwonum =  4*(  (x - y - 2) *(x-y)* (y-1)*(y-1)//8 + (-2+x)*(x-2*y)*(-4+x+4*y)*(x-2*(1+y))//96 + y*(16-20*y-4*y*y+5*y*y*y)//96 + (x-2*y)*(-2+2*x*x+6*y-4*y*y-x*(3+2*y))*(y-1)//24\
                  + ((y -2)*(y-2)* y//16 - (2 *x - 3 *y ) *(4 *x*x - 6 *x *(y + 2) -9*y*y+18*y+8)//48)*(y-1) )
# 深红25+浅红20+明黄52+其他115
# twotwonumelse = \sum_{i=1}^{x}\sum_{j=1}^{y} min(x-i,y-j,i-1)+min(x-i,y-j,j-1)+min(x-i,i-1,j-1)+min(y-j,i-1,j-1)
# 不妨 i<=x//2, j<=y//2 => min(x-i,j-1)=j-1                    min(y-j,i-1)+j-1+2*min(i-1,j-1)
# (j-1) 的部分 (y//2-1)*y//2//2*x//2
# min(i-1,j-1) 的部分 sum_{k=0}^{y//2-1} k*(x//2+y//2-2k-1)  = -1//48 (-2 + y) y (2 - 3 x + y)
# min(y-j,i-1) 的部分 sum_{i=1}^{y//2} y//2*(i-1) + sum_{i=y//2+1}^{x//2} (sum_{j=y-i+1}^{y//2}(y-j)  + sum_{j=1}^{y-i}(i-1)) = 1//16 (-2 + y) y^2 - 1//48 (x - y) (-4 + x^2 + 6 y - 5 x y - 2 y^2)
twotwonumelse = 4*((y//2-1)*y//2//2*x//2 + 2*(-(-2+y)*y*(2-3*x+y)//48) + (-2+y)*y*y//16-(x-y)*(-4+x*x+6*y-5*x*y-2*y*y)//48)
final = allnum - twonum + twotwonum - twotwonumelse + 2*threenum

print(final)



# 不妨 i<=x//2, j<=y//2 => min(x-i,j-1)=j-1
# 1. min(i-1,y-j)=y-j
#    则 min(x-i,y-j)=y-j, min(i-1,j-1)=j-1
#    有 y-i+1 <= j <= y//2 i <= x//2 对 (y-1)*(y-1) 求和  # (sum 1 j = 1 to y//2 , i = y to x//2) + (sum 1 j=y-i+1 to y//2, i=y//2+1 to y-1) = 1//4 (2 + x - 2 y) y + 1//8 (-2 + y) y
#    此处求和为 ((2+x-2*y)//4*y+(-2+y)*y//8)*(y-1)*(y-1)
# 2. min(i-1,y-j)=i-1
#    2.1 min(x-i,y-j)=x-i
#        则 min(i-1,j-1)=j-1
#        有 1 <= j <= y-x+i i <= x//2 对 (x-i+j-1)*(i+j-2) 求和  # sum (x-i+j-1)*(i+j-2), j=1 to y-x+i, i=x-y+1 to x//2 = 1//96 (-2 + x) (x - 2 y) (-4 + x + 4 y) (x - 2 (1 + y))
#    2.2 min(x-i,y-j)=y-j
#        2.2.1 min(i-1,j-1)=i-1
#        有 i <= j <= y-i+1 i <= x//2 对 (y-j+i-1)*(i+j-2) 求和  # sum (y-j+i-1)*(i+j-2), j=i to y-i+1, i=1 to y//2 = 1//48 y (16 - 20 y - 4 y^2 + 5 y^3)
#        2.2.2 min(i-1,j-1)=j-1
#        有 y-x+i <= j <= y-i+1 i <= x//2对 (y-1) * (i+j-2) 求和  #  sum (i+j-2), j=y-x+i to y-i+1, i=x-y+1 to x//2 = sum((x-2i+2)*(i-2)+(2*y-x+1)(x-2i+2)//2), i=x-y+1 to x//2 = 1//24 (x - 2 y) (10 + 2 x^2 + 6 y - 4 y^2 - x (9 + 2 y))



# twotwonum = \sum_{i=1}^{x}\sum_{j=1}^{y}(min(x-i,y-j)+min(i-1,j-1))*(min(x-i,j-1)+min(i-1,y-j))
# 不妨 i<=x//2, j<=y//2 => min(x-i,j-1)=j-1
# 1. min(i-1,y-j)=y-j 且不等
#    则 min(x-i,y-j)=y-j, min(i-1,j-1)=j-1
#    有 y-i+2 <= j <= y//2 i <= x//2 对 (y-1)*(y-1) 求和  # (sum 1 j=y-i+2 to y//2, i=y//2+2 to x//2) = 1//8 (x - y - 2) (x-y) 
#    此处求和为 (x - y - 2) *(x-y)* (y-1)*(y-1)//8
# 2. min(i-1,y-j)=i-1  y-j>=i-1  j<=y-i+1
#    2.1 min(x-i,y-j)=x-i
#        则 min(i-1,j-1)=j-1
#        有 1 <= j <= y-x+i i <= x//2 对 (x-i+j-1)*(i+j-2) 求和  # sum (x-i+j-1)*(i+j-2), j=1 to y-x+i, i=x-y+1 to x//2 = 1//96 (-2 + x) (x - 2 y) (-4 + x + 4 y) (x - 2 (1 + y))
#    2.2 min(x-i,y-j)=y-j 且不等   y-j<=x-i-1  j>=y-x+i+1
#        2.2.1 min(i-1,j-1)=i-1  j >= i 
#        有 i <= j <= y//2 i <= x//2 对 (y-j+i-1)*(i+j-2) 求和  # sum (y-j+i-1)*(i+j-2), j=i to y//2, i=1 to y//2 = 1//96 y (16 - 20 y - 4 y^2 + 5 y^3)
#        2.2.2 min(i-1,j-1)=j-1 且不等 j <= i-1
#        有 y-x+i+1 <= j <= min(i-1,y-i+1) i <= x//2 对 (y-1) * (i+j-2) 求和  #  (sum (i+j-2), j=y-x+i+1 to y-i+1, i=x-y+1 to x//2) + (sum (i+j-2), j=1 to y-i+1, i=y//2+1 to x-y)
#           + (sum (i+j-2), j=1 to i-1, i=2 to y//2) = sum((x-2i+1)*(i-2)+(2*y-x+2)(x-2i+1)//2), i=x-y+1 to x//2 +... = 1//24 (x - 2 y) (-2 + 2 x^2 + 6 y - 4 y^2 - x (3 + 2 y))
#           + 1//16 (y - 2)^2 y - 1//48 (2 x - 3 y) (4 x^2 - 6 x (y + 2) - 9 y^2 + 18 y + 8) for x element Z
# twotwonum =  4*(  (x - y - 2) *(x-y)* (y-1)*(y-1)//8 + (-2+x)*(x-2*y)*(-4+x+4*y)*(x-2*(1+y))//96 + y*(16-20*y-4*y*y+5*y*y*y)//96 + (x-2*y)*(-2+2*x*x+6*y-4*y*y-x*(3+2*y))*(y-1)//24\
#                   + ((y -2)*(y-2)* y//16 - (2 *x - 3 *y ) *(4 *x*x - 6 *x *(y + 2) -9*y*y+18*y+8)//48)*(y-1) )
# 深红25+浅红20+明黄52+其他115
