import math

import matplotlib.pyplot as plt
import numpy as np
import torch
from torch import nn, optim

mm = torch.matmul

class Squeeze(nn.Module):
	def forward(self, x):
		return x.squeeze()

"""
Computes the Euclidian distance between every row in two
matrices.
"""
def dist2(A, B):
	assert len(A.shape) == 2
	assert len(B.shape) == 2
	n = A.shape[0]
	m = B.shape[0]
	assert A.shape[1] == B.shape[1]
	A2 = (A**2).sum(1).reshape((n, 1))
	B2 = (B**2).sum(1).reshape((1, m))
	return A2 + B2 - mm(A, B.T) * 2.0

class RBF(nn.Module):
	def __init__(self, dim):
		super().__init__()
		self.length = nn.Parameter(
			torch.ones(1, dim) * 1,
			requires_grad=True
		)
		self.scale = nn.Parameter(
			torch.ones(1, dim) * -1,
			requires_grad=True
		)

	def forward(self, A, B):
		assert len(A.shape) == 2
		assert len(B.shape) == 2
		assert A.shape[1] == B.shape[1]

		D2 = dist2(A, B)
		return torch.exp(
			D2 / -torch.exp(self.length)
		) * torch.exp(self.scale)

class HiddenKernel(nn.Module):
	def __init__(self, dim, width):
		super().__init__()
		self.seq = nn.Sequential(
			nn.Linear(dim, width),
			nn.LeakyReLU(inplace=True),
			nn.Linear(width, width),
			nn.LeakyReLU(inplace=True),
		)

	def forward(self, A, B):
		assert len(A.shape) == 2
		assert len(B.shape) == 2
		A = self.seq(A)
		B = self.seq(B)
		return torch.exp(dist2(A, B) / -2.0)

class Poly(nn.Module):
	def __init__(self, degree):
		super().__init__()
		self.degree = degree

	def forward(self, A, B):
		return (mm(A, B.T) + 1.0)**self.degree

class GaussianProcess(nn.Module):
	def __init__(self, kernel, prior = None):
		super().__init__()
		self.kernel = kernel
		self.prior = prior

	def forward(self, train, trainY, test, noise=None, drop_variance=False):
		n, d = train.shape
		m, d = test.shape
		assert len(trainY.shape) == 1
		assert trainY.shape[0] == n
		assert train.shape[1] == test.shape[1]

		if noise is not None:
			assert len(noise.shape) == 1
			assert noise.shape[0] == n

		K = self.kernel(train, train)
		if noise is not None:
			K = K + torch.diag(noise)

		inv = torch.inverse(K)

		K2 = self.kernel(test, train)

		prior = (lambda x: 0) if self.prior is None else self.prior

		residual = (trainY - prior(train)).reshape((-1, 1))
		mean = prior(test) + mm(K2, mm(inv, residual))
		
		if drop_variance:
			return mean

		var = self.kernel(test, test) - mm(K2, mm(inv, K2.T))
		return mean, var

	def loss(self, X, Y, noise=None):
		n, d = X.shape
		assert len(Y.shape) == 1
		assert Y.shape[0] == n

		K = self.kernel(X, X)
		if noise is not None:
			assert len(noise.shape) == 1
			assert noise.shape[0] == n
			K = K + torch.diag(noise)

		inv = torch.inverse(K)

		prior = (lambda x: 0) if self.prior is None else self.prior

		Y = Y - prior(X)

		a = mm(Y.T, mm(inv, Y))
		b = torch.logdet(K)
		c = n * math.log(2.0 * math.pi)
		return (a + b + c) / 2.0

n = 48
X = torch.randn(n, 1)

def f(x):
	if x < -1:
		return 0
	elif x < 0:
		return 1
	else:
		return math.sin(x)
Y = torch.tensor([f(x[0]) for x in X])

w = np.linalg.lstsq(
	np.concatenate([
		np.ones((n, 1)),
		X.cpu().detach().numpy(),
		X.cpu().detach().numpy()**2,
		X.cpu().detach().numpy()**3,
		X.cpu().detach().numpy()**4,
		X.cpu().detach().numpy()**5,
		X.cpu().detach().numpy()**6,
	], 1),
	Y.cpu().detach().numpy(),
	rcond=-1
)[0]

wa, wb, wc, wd, we, wf, wg = float(w[0]), float(w[1]), float(w[2]), float(w[3]), float(w[4]), float(w[5]), float(w[6])
# prior = lambda x: wa + x[:,0] * wb + x[:,0]**2 * wc + x[:,0]**3 * wd + x[:,0]**4 * we + x[:,0]**5 * wf + x[:,0]**6 * wg
prior = lambda x: float(Y.mean())

noise = torch.ones(n) * 0.01

PX = torch.arange(-5, 5, 0.02).reshape((-1, 1))

gp = GaussianProcess(RBF(1), prior=prior)

num_params = sum([p.nelement() for p in gp.parameters()])
print(f'{num_params} parameters')
L = []
if num_params > 0:
	optimizer = optim.SGD(gp.parameters(), lr=3e-2, weight_decay=1e-3)
	for it in range(5000):
		optimizer.zero_grad()
		loss = gp.loss(X, Y, noise=noise)
		if torch.isnan(loss):
			exit(0)
		if it % 20 == 0:
			print(it, float(loss))
		L.append(-float(loss))
		loss.backward()
		with torch.no_grad():
			for p in gp.parameters():
				p.grad.clamp_(-0.3, 0.3)
		optimizer.step()

PY, V = gp(X, Y, PX, noise=noise, drop_variance=False)

X = X.cpu().detach().numpy()[:,0]
Y = Y.cpu().detach().numpy()
V = torch.diag(V).cpu().detach().numpy().clip(0, float('inf'))
PX = PX.cpu().detach().numpy()[:,0]
PY = PY.cpu().detach().numpy()[:,0]

plt.subplot(1, 2, 1)
plt.plot(L)
plt.ylabel('Log Likelihood')
plt.xlabel('Iteration')

plt.subplot(1, 2, 2)
plt.scatter(X, Y)
plt.plot(PX, PY, c='r')
# plt.plot(PX, PY + V * 2, c='b')
# plt.plot(PX, PY - V * 2, c='b')
plt.ylim(-2, 2)

plt.show()




