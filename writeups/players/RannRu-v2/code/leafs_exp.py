import cv2 as cv
import matplotlib.pyplot as plt
def plot_hist(image):
    plt.hist(image.ravel(), 256, [0, 256])
    #image_hist(src)
    plt.show()

def image_hist(image):
    colors = ['blue', 'green', 'red']
    for i, color in enumerate(colors):
    	# 三个通道
        hist = cv.calcHist([image], [i], None, [256], [0, 256])
        plt.plot(hist, color=color)
    plt.show()

src = cv.imread(r'./a.png', cv.IMREAD_GRAYSCALE)   
#src = cv.imread(r'./EmbeddedCover.png')
#src = cv.imread(r'./EmbeddedCover.png', cv.IMREAD_GRAYSCALE)
#cv.imshow('src', src)

plot_hist(src)
#cv.waitKey(0)
#cv.destroyAllWindows()
'''
h = [0]*256
for i in range(1000):
    for j in range(1000):
        for k in range(0, 3):
            h[src[i, j, k]] += 1
print(h)
maxv = max(h)
maxi = h.index(maxv)
print(maxv, maxi, h.index(min(h)))

ans = ''
cnt = 0
for i in range(1000):
    for j in range(1000):
        for k in range(0, 3):
            if src[i, j, k] == maxi:
                if cnt % 8 == 0:
                    cnt += 1
                    ans += '0'
                ans = ans + '0'
                cnt += 1
            if src[i, j, k] == maxi-1:
                if cnt % 8 == 0:
                    cnt += 1
                    ans += '0'
                ans = ans + '1'
                cnt += 1

print(cnt)
'''

h = [0]*256
for i in range(1000):
    for j in range(1000):
        col = src[i, j]
        h[col] += 1
print(h)
maxv = max(h)
maxi = h.index(maxv)
print(maxv, maxi)

ans = ''
for i in range(256):
    if h[i] == 8196:
        ans += '1'
    elif h[i] == 8284:
        ans += '0'
    else:
        ans += '2'
print(ans)
a = hex(int(ans,3))
print(a)
print(bytes.fromhex(hex(int(ans,3))[2:]))
'''
ans = ''
for i in range(1000):
    for j in range(1000):
        if src[i, j] == maxi:
            ans = ans + '0'
        if src[i, j] == maxi - 1:
            ans = ans + '1'

ans = ans[:-(len(ans)%8)]
ans = hex(int(ans, 2))[2:]
print(ans)
print(bytes.fromhex(ans))
'''
