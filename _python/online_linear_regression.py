"""
Woodbury matrix identity:

inv(A + UCV) = inv(A) - inv(A) U inv(inv(C) + V inv(A) U) V inv(A)

If C=I and U and V are vectors with shape [d, 1] and [1, d] respectively, this simplifies to

inv(A + UV) = inv(A) - inv(A) U inv(I + V inv(A) U) V inv(A)

Where 'V inv(A) U' is simply a scalar, so inv(I + V inv(A) U) is simple floating-point division.

This means we can compute inv(A + UV) from inv(A), without ever taking another matrix inverse.

inv(A + UV) = inv(A) - (inv(A) U V inv(A)) / (1 + V inv(A) U)

Which has computation complexity O(d^2) – far better than the O(d^3) a naive computation of inv(A + UV) would have

This forms the basis of "online" linear regression, where the goal is to efficiently update a linear model.

Recall the formula for linear regression is inv(X' X) X' y.  This takes O(d^3 + n d^2) time to compute from scratch.

But if we use the above expression and let U = V' = [new x point] we can compute the inverse of the new X'X much more quickly.
"""

import random
import numpy as np

# Performs online linear regression.  Supports any number of
# independent and dependent variables.
class OnlineLinearRegressor:
  # Each row of X and Y represents a datapoint.
  def __init__(self, X, Y, regularizer = 0.0):
    assert len(X.shape) == 2
    assert len(Y.shape) == 2
    assert X.shape[0] == Y.shape[0]
    cov = np.dot(X.T, X) + np.eye(X.shape[1]) * regularizer
    self._icov = np.linalg.inv(cov)
    self._xy = np.dot(X.T, Y)
    self.w = np.dot(self._icov, self._xy)

  def _dot(self, *matrices):
    r = matrices[0]
    for mat in matrices[1:]:
      r = np.dot(r, mat)
    return r

  # Update self.w n additional observations.
  # x: [n, c]
  # y: [n, d]
  # Runs in O(n^3 + nc^2 + cn^2 + dnc)
  # In typical online situations n=1, in which case this runs in O(c^2 + dc)
  def add(self, x, y):
    n = x.shape[0]
    assert len(x.shape) == 2
    assert len(y.shape) == 2
    assert y.shape[0] == n
    self._xy += np.dot(x.T, y)
    s = np.linalg.inv(np.eye(n) + self._dot(x, self._icov, x.T))
    # NOTE: we need to re-order these operations to eliminate an O(d^3) operation.
    self._icov -= self._dot(
      np.dot(self._icov, x.T), # d n
      s,                       # n n
      np.dot(x, self._icov)   # n d
    )
    self.w = np.dot(self._icov, self._xy)

def test():
  # Create random dataset.
  X = np.random.normal(0, 1, (10, 2))
  Y = np.random.normal(0, 1, (10, 3))
  # Choose random regularization term.
  r = random.random()

  # Perform regresssion offline / naively
  w = np.dot(np.linalg.inv(np.dot(X.T, X) + np.eye(X.shape[1]) * r), np.dot(X.T, Y))

  # Perform online regression, by training on the first half of
  # the dataset, and then the second half.
  olr = OnlineLinearRegressor(X[:-5], Y[:-5], regularizer=r)
  olr.add(X[-5:], Y[-5:])

  # Print distance between online result and groundtruth result.
  print(np.abs(w - olr.w).mean())

test()
