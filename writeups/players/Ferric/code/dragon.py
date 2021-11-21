import requests

token = '17:MEYCIQDO2JkPqaAcs6SgWn0bvmAdVEi3F18SE/k0+2IY/bwcWQIhANvpp6zqf4Cfw/TeicbFnINwF5RT25Q9QbMbBagNM9FG'
host = 'https://prob16.geekgame.pku.edu.cn/'
balls = {}


def main():
    requests.post(host + 'api/reset', params={
        'token': token,
        'super': 'false'
    })
    state0_0 = requests.get(host + 'api/state', params={
        'token': token,
        'x': 0,
        'y': 0
    }).json()

    sizes = [24, 48, 192, 1536, 24576, 393216, 25165824]
    a = [None for _ in range(7)]
    global balls
    balls = {}

    def breakBall(x, y):
        global balls
        balls = requests.post(host + 'api/break', params={
            'token': token,
            'x': x,
            'y': y
        }).json()['balls']

    # get the FIRST ball in chunk 0, 0
    for i in range(24):
        for j in range(24):
            if state0_0['materials'][i][j] == 'FIRST':
                a[0] = (i, j)
                break
        if a[0] != None:
            break

    print(a[0])
    breakBall(a[0][0], a[0][1])
    assert(len(balls.items()) == 1)

    # levels
    for k in range(1, 7):
        # skip by sizes[k - 1]
        for i2 in range(0, sizes[k], sizes[k - 1]):
            for j2 in range(0, sizes[k], sizes[k - 1]):
                a2 = (a[k - 1][0] + i2, a[k - 1][1] + j2)
                breakBall(a2[0], a2[1])
                if len(balls.items()) == k + 1:
                    a[k] = a2
                    break
            if len(balls.items()) == k + 1:
                break
        if len(balls.items()) != k + 1:
            print("ERROR", balls)
            return False
        print(balls)

    print(requests.get(host + 'api/state', params={
        'token': token,
        'x': 0,
        'y': 0
    }).json()['flag'])
    return True


while True:
    if main():
        break
