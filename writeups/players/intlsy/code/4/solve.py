import os, sys, math

def Calc(n,m):
    if m > n: n,m = m,n
    assert n >= 3
    assert m <= n
    
    ans = 0
    ans += n**3*(m**3 - 3*m**2 + 2*m)*16 - n**2*(3*m**3 - 9*m**2 + 6*m)*48
    ans += n*(2*m**4 + 20*m**3 - 77*m**2 + 58*m)*16
    ans -= (39*m**4 - 82*m**3 - 36*m**2 + 88*m)*4
    ans += (2*m - 4*n + 1)*(1 + (-1)**(m+1))*6
    ans += (1 + abs(n - 2*m + 3) - abs(n - 2*m + 4))*(((n - 2*m + 11)**4 - 42*(n - 2*m + 11)**3 + 656*(n - 2*m + 11)**2 - 4518*(n - 2*m + 11) + 11583)*2 - (4*m - 2*n - 1)*(1 + (-1)**(n+1))*3)
    assert ans%96 == 0
    return ans//96
    
N = 672328094
M = 386900246

print(Calc(N,M))
