#!/usr/bin/env python3

from random import randint
import sys

__MIN_BLOCK_SIZE = 8
__MAX_BLOCK_SIZE = 16


def next_block() -> bytes:
  return bytes([randint(0, 255) for _ in
                range(randint(__MIN_BLOCK_SIZE, __MAX_BLOCK_SIZE))])


if len(sys.argv) < 3:
  print(f'usage: {sys.argv[0]} INPUT OUTPUT_NAME')
  exit(1)


file1 = bytes()
file2 = bytes()
with open(sys.argv[1], 'rb') as f:
  for i in f.read():
    block = next_block()
    file1 += block
    file2 += block
    if randint(0, 1):
      file1 += bytes([i])
    else:
      file2 += bytes([i])
with open(f'{sys.argv[2]}1.bin', 'wb') as f:
  f.write(file1)
with open(f'{sys.argv[2]}2.bin', 'wb') as f:
  f.write(file2)
