import functools
import sys
import tempfile
import shutil
import os
import subprocess

print = functools.partial(print, flush=True)

def main():
    print('What do you think should be present in a debug info file ?')
    print('Give me your answer, I will debug it for you :)')
    print('File > ')

    content = sys.stdin.buffer.read(0x1024)
    print(len(content))
    with tempfile.TemporaryDirectory() as tempdir:
        bin_path = os.path.join(tempdir, 'hello')
        shutil.copyfile('/home/ctf/hello', bin_path)
        with open(os.path.join(tempdir, 'hello.debug'), 'wb') as f:
            f.write(content)
        os.chdir(tempdir) 
        os.chmod(bin_path, 0o755)
        subprocess.run(['gdb', 'hello', '-ex', 'b main', '-ex', 'r', '-ex', 'p a', '-ex', 'c', '-ex', 'q'])
        

if __name__ == '__main__':
    main()