import numpy as np

class PCAer:
	def __init__(self, X):
		n, _ = X.shape
		cov = np.dot(X.T, X) / n
		D, V = np.linalg.eigh(cov)
		I = np.argsort(-D)
		D, V = D[I] / D.sum(), V[:, I]
		self.V = V
		self.D = D

	# Compress X down to its k principle components.
	def compress(self, X, k):
		return np.dot(X, self.V[:k])

	# Try and recover X from its K largest principle components. 
	def decompress(X):
		_, k = X.shape
		return np.dot(X, self.V[:k].T)

# Computes pair-wise (square) Euclidian distances between
# every row of A and every row of B.  Makes use of the
# identity "(a - b)^2 = a^2 + b^2 - 2 a b".
def dist2(A, B):
	assert A.shape[1] == B.shape[1]
	n, _ = A.shape
	m, _ = B.shape
	A2 = (A**2).sum(1).reshape((n, 1))
	B2 = (B**2).sum(1).reshape((1, m))
	D2 = A2 + B2 - np.dot(A, B.T) * 2.0
	# Floating point errors can yield negative distances...
	return np.maximum(D2, 0.0)