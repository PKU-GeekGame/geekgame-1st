stor2 = "0x15eea4b2551f0c96d02a5d62f84cac8112690d68c47b16814e221b8a37d6c4d3"
stor3 = "0x293edea661635aabcd6deba615ab813a7610c1cfb9efb31ccc5224c0e4b37372"

s2 = int(stor2, 16)
s3 = int(stor3, 16)

ans = 0

for i in range(64):
    j = i << 2
    tmp = (s3 >> j) & 0xf
    tmp2 = 5 * i + 7 * (s2 >> j)
    ans |= ((tmp-tmp2)& 0xf) << j

print(bytes.fromhex(hex(ans)[2:]))
