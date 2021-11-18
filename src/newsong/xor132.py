text = '''
Do you remember those past events?

flag{th3_Sun5et_h0r1zon_0815}

And the final password is: ItsMeMrLeaf
'''.strip()

text = '\r\n'.join(text.split('\n'))
text += '\r\n'

for i in text:
  print(hex(ord(i) ^ 132))
