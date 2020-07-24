"""
Helpful functions/classes for encoding data.
"""

# Encode an integer as 8 bytes.
def encode_int8(x):
	return x.to_bytes(8, 'big')

# Decode an integer from a byte array of length 7 (or 8).
def decode_int8(text):
	return int.from_bytes(text, 'big', signed=False)

# Encodes a list of 64-bit unsigned integers.
class UInt64List(list):
	def encode(self):
		return encode_int8(len(self)) + b''.join([encode_int8(x) for x in self])

	def append(self, x):
		assert type(x) is int, x
		super().append(x)

	@staticmethod
	def decode(data):
		r = UInt64List()
		length = decode_int8(data[0:8])
		assert length < 1000000, length
		for i in range(length):
			r.append(decode_int8(data[8*i+8:8*i+16]))
		return r, 8 * (length + 1)