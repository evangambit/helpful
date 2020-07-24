
def pad(t, n, c=' '):
  t = str(t)
  return max(n - len(t), 0) * c + t

import hashlib
# Hashes strings to 64-bit integers.  Importantly, these hashes
# never change (in contrast, Python's built-in hash function
# changes every time the program is run).
class Hash64:
  def __init__(self):
    pass
  def __call__(self, x):
    h = hashlib.sha256()
    h.update(x.encode())
    return int(h.hexdigest()[-16:], 16)
hashfn = Hash64()

import time
class Throttler:
  def __init__(self, qps):
    self.wait_time = 1. / qps
    self.last_call = time.time() - self.wait_time
  def throttle(self):
    next_time = self.last_call + self.wait_time
    time.sleep(max(next_time - time.time(), 0.0))
    self.last_call = time.time()