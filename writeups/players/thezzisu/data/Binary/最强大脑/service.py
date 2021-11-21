import os
import subprocess

timeout = 60

code = input("give me code (hex): ").strip()
code = bytes.fromhex(code)
if len(code) > 0x1000:
    print("too long")
    exit()

with open("/tmp/code.bf", "wb") as f:
    f.write(code)

my_dir = os.path.abspath(os.path.dirname(__file__))
exe = os.path.join(my_dir, "bf")

subprocess.run([exe, "/tmp/code.bf"], timeout=timeout, cwd=my_dir)
