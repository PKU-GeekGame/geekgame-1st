import os, sys

template = "Wakarimasu! {pdate} you-kali-vm {parch} GNU/Linux {pnproc}"

def Try(password):
    print(password)
    ret = os.system('7za x b.7z -p"%s"'%password)
    if ret == 0:
        print("OHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH")
        sys.exit(0)

# 'p' means 'param'
pdate = [
"Sat 06 Nov 2021 03:44:15 PM CST",
"Sat 06 Nov 2021 15:44:15 CST"
"Nov  6, 2021 15:44:15 CST",
"Nov 06, 2021 15:44:15 CST",
"Nov 6, 2021 15:44:15 CST",
"2021年 11月 06日 星期六 15:44:15 CST",
"2021年 11月 6日 星期六 15:44:15 CST",
"2021年 11月  6日 星期六 15:44:15 CST",
"2021年 11月 06日 星期六 下午 03:44:15 CST",
"2021年 11月 6日 星期六 下午 03:44:15 CST",
"2021年 11月  6日 星期六 下午 03:44:15 CST",
"2021年 11月 06日 星期六 下午 3:44:15 CST",
"2021年 11月 6日 星期六 下午 3:44:15 CST",
"2021年 11月  6日 星期六 下午 3:44:15 CST",
"Sat Nov  6 15:44:15 CST 2021",
"Sat Nov 6 15:44:15 CST 2021",
"Sat Nov 06 15:44:15 CST 2021",
"Sat Nov  6 3:44:15 PM CST 2021",
"Sat Nov 6 3:44:15 PM CST 2021",
"Sat Nov 06 3:44:15 PM CST 2021",
"Sat Nov  6 03:44:15 PM CST 2021",
"Sat Nov 6 03:44:15 PM CST 2021",
"Sat Nov 06 03:44:15 PM CST 2021"
]
parch = ["i686","i386","x86","x86_64","aarch64"]
pnproc = ["8"]

for _pdate in pdate:
    for _parch in parch:
        for _pnproc in pnproc:
            Try(template.format(pdate=_pdate,parch=_parch,pnproc=_pnproc))
