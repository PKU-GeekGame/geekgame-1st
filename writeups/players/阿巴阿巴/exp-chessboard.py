from decimal import *
import re
getcontext().prec = 1000

def calc(n, m):
    if n < m: n, m = m, n
    n = Decimal(n); m = Decimal(m)
    while True:
        try:
            # https://oeis.org/A047659
            return n ** _3 / _6 * (m ** _3 - _3 * m ** _2 + _2 * m) - n ** _2 / _2 * (_3 * m
                ** _3 - _9 * m ** _2 + _6 * m) + n / _6 * (_2 * m ** _4 + _20 * m ** _3
                - _77 * m ** _2 + _58 * m) - _1 / _24 * (_39 * m ** _4 - _82 * m ** _3
                - _36 * m ** _2 + _88 * m) + _1 / _16 * (_2 * m - _4 * n + _1) * (_1
                + (-_1) ** (m + _1)) + _1 / _2 * (_1 + abs(n - _2 * m + _3) - abs(n
                - _2 * m + _4)) * (_1 / _24 * ((n - _2 * m + _11) ** _4 - _42 * (n - _2
                                * m + _11) ** _3 + _656 * (n - _2 * m + _11) ** _2
                                - _4518 * (n - _2 * m + _11) + _11583) - _1 / _16
                                * (_4 * m - _2 * n - _1) * (_1 + (-_1) ** (n + _1)))
        except Exception as e:
            g = re.search("'(.*?)'", str(e))
            data = g.group(1)
            globals()[data] = Decimal(int(data[1:]))

for n in range(3, 5):
    for m in range(3, 10):
        print(round(calc(n, m)), end=' ')
    print()

print(calc(672328094, 386900246))