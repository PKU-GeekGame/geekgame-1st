#!/usr/bin/env python3

from functools import reduce
from PIL import Image
from typing import List, Tuple
import sys
from math import floor
from itertools import chain
import numpy as np


def read_barcode_info(path: str) -> List[bool]:
  img = Image.open(path)
  img = img.resize((256, img.height)).convert('L')
  return list(map(lambda x: img.getpixel((x, 0)) < 128, range(256)))


def read_ref_img_info(path: str) -> Tuple[Image.Image, int]:
  img = Image.open(path).convert('1')
  return (img, np.sum(np.array(img) == 0))


def get_image(info: List[bool], ref: Image.Image, black_count: int) -> Image.Image:
  expected = ref.width * ref.height
  total = sum(info)
  count = floor(expected / total)
  last = expected - ((total - 1) * count)
  pixels = list(map(lambda x: x[0], filter(lambda x: x[1], enumerate(info))))
  data = chain(map(lambda x: [x] * count, pixels[:-1]), [[pixels[-1]] * last])
  data = list(reduce(lambda l0, l1: l0 + l1, data))
  img = Image.new('L', (ref.width, ref.height))
  black_index, white_index = 0, black_count
  for y in range(ref.height):
    for x in range(ref.width):
      if ref.getpixel((x, y)):
        img.putpixel((x, y), data[white_index])
        white_index += 1
      else:
        img.putpixel((x, y), data[black_index])
        black_index += 1
  return img


if __name__ == '__main__':
  if len(sys.argv) < 4:
    print(f'usage: {sys.argv[0]} BARCODE_IMG REF_IMG OUTPUT')
    exit(1)

  info = read_barcode_info(sys.argv[1])
  (ref, black_count) = read_ref_img_info(sys.argv[2])
  img = get_image(info, ref, black_count)
  img.save(sys.argv[3])
