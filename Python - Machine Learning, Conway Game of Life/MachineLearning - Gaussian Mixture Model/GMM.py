import numpy as np
import matplotlib.pyplot as plt
from numpy import linalg as LA
import math
from scipy.stats import multivariate_normal as norm

# Student: Lior Ichiely
"""
The following code tries to approximate a Gaussian random variable to random points of data on R^2.
Part 1: Simply draws 1000 points of data from an actual GMM (Gaussian Mixture Model).
Part 2: Classifying points by K-Means - average centers of all points
Part 3: Classify points by GMM EM(Estimation, Maximization) - Try to find the gaussians in the model.
Note: There are assumed to be two Gaussians only.
"""

# Part 1: Scatter 1000 points
# Variables
P_z = [0.7 , 0.3] # Q Function
# Gauss 1
G1_mean = [-1,-1]
G1_Cov = [[0.8, 0], [0, 0.8]]
# Gauss 2
G2_mean = [1,1]
G2_Cov = [[0.75, -0.2], [-0.2, 0.6]]

# Generate 1000 points and draw:
Data = np.zeros((1000,2))
for i in range(1000):
    Data[i]=0.7*np.random.multivariate_normal(G1_mean,G1_Cov)+0.3*np.random.multivariate_normal(G2_mean,G2_Cov)

plt.title("1000 Scattered dots of Data")
plt.scatter(Data[:,0],Data[:,1])
plt.show() # Show the first scatter.
plt.clf() # Clear.



# Part 2: K-Means with 50 points
# Generate 50 points:
Data = np.zeros((50,2))
for i in range(50):
    Data[i]=0.7*np.random.multivariate_normal(G1_mean,G1_Cov)+0.3*np.random.multivariate_normal(G2_mean,G2_Cov)


K=2  # 1) Specify the number K of clusters to assign (Here K=2).
K_Cent=np.array([[-1.1,-1.1], [1.1,1.1]]) # 2) Randomly initilaize K=2 centroids.
K_Prev=np.copy(K_Cent)
# 3) repeat:
while(True):
    Gauss1Group=np.zeros((0,2))
    Gauss2Group=np.zeros((0,2))
# 3.1) Expectation: Assign to each point its closest centroid.
    for i in range(50):
        if math.dist(Data[i],K_Cent[0]) < math.dist(Data[i],K_Cent[1]):
            Gauss1Group = np.append(Gauss1Group, [Data[i]], axis=0)
        else: Gauss2Group = np.append(Gauss2Group, [Data[i]], axis=0)
    # Plot every stage

    plt.title("K-means step")
    plt.scatter(K_Cent[:, 0], K_Cent[:, 1])
    plt.scatter(Gauss1Group[:, 0], Gauss1Group[:, 1])
    plt.scatter(Gauss2Group[:, 0], Gauss2Group[:, 1])
    plt.legend(['Centroid','Dots of C1','Dots of C2'])
    plt.show()  # Show the areas.
    plt.clf()

    # 3.2) Maximization: Compute the new centroid of each cluster.
    # New centroid = 1/|Group| * sum(All points in group)
    K_Cent[0]=sum(Gauss1Group)/LA.norm(Gauss1Group,1)
    K_Cent[1]=sum(Gauss2Group)/LA.norm(Gauss2Group,1)
    # Until the centroids converge.
    if (math.dist(K_Cent[0],K_Prev[0]) < 0.03) and (math.dist(K_Cent[1],K_Prev[1]) < 0.03):
        break
    else: K_Prev=np.copy(K_Cent)

# End while.
print("Converged!")

# Part 3: EM implementation!

# 1) Generate 10000 samples.
Data = np.zeros((10000,2))
for i in range(10000):
    Data[i]=0.7*np.random.multivariate_normal(G1_mean,G1_Cov)+0.3*np.random.multivariate_normal(G2_mean,G2_Cov)
# Implement EM to fit the GMM of two gaussians for the generated data.
# - Initialize random parameters.
phi = [0.7, 0.3] #np.random.uniform(0,1)
 #np.random.normal(0, 1, size=(2,))
mu = [[-0.5, -0.5], [0.5, 0.5]]
sigma=[[[0.75, 0.4], [0.4, 0.6]], [[0.8, 0.2], [0.2, 0.6]]]
log_likelihood=np.zeros((0,1))

W = np.zeros((2,10000))
ITER=0 # This will be the iteration counter.

while(True):
    # E Step
    # Let's find the weight matrix, w(j,i), as given in the lecture. For each j,i :
    # w(j,i) = [phi(gaussian j) * p(Data[i] | gaussian j)] / [sum of all k phi(gaussian k) * p(Data[i] | gaussian k]
    for j in range(2):
        for i in range(10000):
            W[j][i]=phi[j]*norm(mu[j],sigma[j]).cdf(Data[i]) / (phi[0]*norm(mu[0],sigma[0]).cdf(Data[i]) + phi[1]*norm(mu[1],sigma[1]).cdf(Data[i]))
    # M Step
    # Let's calculate the next phi, mean and covariance based on the formulas given in the lecture:
    phi[0] = sum(W[0])/10000; phi[1] = sum(W[1])/10000;
    mu[0] = np.dot(W[0],Data)/np.sum(W[0]); mu[1]= np.dot(W[1],Data)/np.sum(W[1]);
    sigma[0] = np.dot(np.transpose(Data - mu[0]),np.multiply(W[0][:, np.newaxis],Data - mu[0])) / np.sum(W[0])
    sigma[1] = np.dot(np.transpose(Data - mu[1]),np.multiply(W[1][:, np.newaxis],Data - mu[1])) / np.sum(W[1])

    # Calculate the log likelihood.
    for i in range(10000): log_likelihood_i=np.log2(phi[0]*norm(mu[0],sigma[0]).cdf(Data[i]) + phi[1]*norm(mu[1],sigma[1]).cdf(Data[i]))
    log_likelihood=np.append(log_likelihood,log_likelihood_i);
    if(ITER==0):
        ITER += 1; continue
    if(abs(log_likelihood[ITER] - log_likelihood[ITER-1]) < 0.0001): break; # Stop condition.
    print("Log likelihood distance: ", abs(log_likelihood[ITER] - log_likelihood[ITER - 1])," On iteration number ", ITER)
    ITER+=1

# Plotting the log likelihood value for each iteration:
iter_array=np.arange(ITER+1)
plt.plot(iter_array,log_likelihood)
plt.xlabel('Iteration number'); plt.ylabel('L func'); plt.title("Log-likelihood per iteration (2-Gauss, Random init)")
plt.show()
plt.clf() # Clear.

# Gaussian contour plot. Taken from GitHub - All credit goes to user gwgundersen:
# https://gist.github.com/gwgundersen/90dfa64ca29aa8c3833dbc6b03de44be

N    = 200
X    = np.linspace(-2, 2, N);
Y    = np.linspace(-2, 2, N)
X, Y = np.meshgrid(X, Y)
pos  = np.dstack((X, Y))
rv1   = norm(mu[0],sigma[0])
rv2   = norm(mu[1],sigma[1])
Z1    = rv1.pdf(pos)
Z2    = rv2.pdf(pos)


plt.scatter(Data[:,0],Data[:,1])
plt.contour(X, Y, Z1)
plt.contour(X, Y, Z2)
plt.title("Data with Gaussian countours.")
plt.show()
plt.clf()
