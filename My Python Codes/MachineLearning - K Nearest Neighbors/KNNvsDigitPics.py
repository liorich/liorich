import json
import numpy as np
from utils import load, plot_sample

# Student: Lior Ichiely
"""
The following code classifies which number is present in an image, taken from a set of 3 and 5 number images.
The machine trains on the best K to choose for K-Nearest Neighbors from the train data, and then classifies the test data.
"""

ID = '318745395'

# ==== Classify function
def Classify(Xtrainer, Ytrainer, Test, k):
    normaVector = np.zeros(11520)  # Allocate NormaVector Memory
    for j in range(11520):
        normaVector[j] = np.linalg.norm(Test - Xtrainer[j])
        # Uses frobenius Norm:
        # Norma[i] = sqrt( sum(1,N) of (Test-Xtrain[i])^2) )
        # So, the closest image by this Norm would be the one that achieves the minimal value.

    # Grab the indices of the best Normas.
    kArray = np.argsort(normaVector)[:k]
    # Now, using Ytrain, simply count how many 3's and 5's were inserted.
    threes = 0
    fives = 0
    for j in kArray:
        if Ytrainer[j] == 3:
            threes += 1
        else:
            fives += 1
    # Return the winner.
    if fives > threes:
        return 5
    return 3

# Loading and plot a sample of the data
# ---------------------------------------
# The MNIST database contains in total of 60000 train images and 10000 test images
# of handwritten digits.
# This database is a benchmark for many classification algorithms, including neural networks.
# For further reading, visit http://yann.lecun.com/exdb/mnist/

# First, we will load the data from .MAT file we have prepared for this assignment: MNIST_3_and_5.mat

Xtrain, Ytrain, Xvalid, Yvalid, Xtest = load('MNIST_3_and_5.mat')

# The data is divided into 2 pairs:
# (Xtrain, Ytrain) , (Xvalid, Yvalid)
# In addition, you have unlabeled test sample in Xtest.

# ====== The classifier decides on the optimal k value out of a few based on error precentage with Xvalid, Yvalid.
optimalK = 1 # Init value

Yvalid_test = np.zeros(1522)  # Allocate memory for validation test
AccK_test = np.zeros(10)  # Allocate memory for Accuracy vector, based on values of K
for k in range(1, 21, 2):  # Skip even values of k, so we don't get a stalemate.
    for i in range(1522):
        Yvalid_test[i] = Classify(Xtrain, Ytrain, Xvalid[i], k)
    # Now, Find the accuracy between Yvalid_test and the actual answers, Yvalid.
    accuracy=0
    for i in range(1522):
        if Yvalid_test[i] == Yvalid[i]: accuracy += 1
    AccK_test[int((k-1)/2)] = accuracy/1522
    print('Tested for K=', k)

# Now we have a few accuracy values for different k-valued estimations. Let's use the best we found.
K = np.argmax(AccK_test)*2 + 1
print('K chosen was K= ', K)

# Time to test!
print('Now testing.')
Ytest = np.zeros(1902)
for i in range(1902):
    Ytest[i] = Classify(Xtrain, Ytrain, Xtest[i], K)

# Finally, results will be saved into a <ID>.txt file, where each row is an element in Ytest.
# Save classification results
print('Saving...')
np.savetxt(f'{ID}.txt', Ytest, delimiter=", ", fmt='%i')
print('Done.')


