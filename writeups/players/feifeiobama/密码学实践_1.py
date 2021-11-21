from Cryptodome.Util.number import bytes_to_long,long_to_bytes

def pad(msg):
	n = 32 - len(msg) % 32
	return msg + bytes([n]) * n

n1 = 'f92bef1ba8e0ce315274e7af2aaa1bd38c03442ef858ce6141b40f6b21ce7c1bfd36a31efbbd87215b35f9bb19ef5ef7af5b022ba54ed41e04c81a7f13cd196dd123ed28c7d0e13a3f5c96c8578267ac980b462e9621955c7087563613a37a20'
n2 = 'fa2da301edf5ce214e74e7af2af84fcd940f162aab4dce7911bb037c67d91505e92bf512a8eac2674e3bebb27fe90ad6b0035c3ffe0a9b5811c20c7634de411e995594609f90b050204389d7489d78b3d0713550c365e42471fd156175ce414c'

byte1 = bytes.fromhex(n1)
byte2 = bytes.fromhex(n2)

mess2 = pad(("Sorry, I forget to verify your identity. Please give me your certificate.").encode("utf-8"))

mess1 = b''
for it in range(0,96,32):
	abcd1 = [bytes_to_long(byte1[it+i*8:it+i*8+8]) for i in range(4)]
	abcd2 = [bytes_to_long(byte2[it+i*8:it+i*8+8]) for i in range(4)]
	abcd_xor = [x1 ^ x2 for x1, x2 in zip(abcd1, abcd2)]
	for i in range(32):
		a, b, c, d = abcd_xor
		abcd_xor = [b ^ d, a, b, c]
	a, b, c, d = [long_to_bytes(x ^ bytes_to_long(mess2[it+i*8:it+i*8+8]), 8) for i, x in enumerate(abcd_xor)]
	mess1 += a+b+c+d

print(mess1.decode('utf-8'))