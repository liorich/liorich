import numpy as np
from sklearn import datasets, linear_model, metrics

# Student: Lior Ichiely
"""
The following code tries to approximate a Linear function to a set of data.
The set of data is a collection (422) of blood measurements (10) in diabetic patients.
The function's input is then a set of blood measurements (10) and its output is a "score" that helps predict if the patient is ill.

Using linear regression, we build a linear weight function with the train data and then perform a test.
The accuracy of the test is measured in comparison to scikit's linear regression algorithm.
"""

# Load diabetes dataset
diabetes = datasets.load_diabetes()
diabetes_X = diabetes.data  # matrix of dimensions 442x10

# Split the data into training/testing sets
diabetes_X_train = diabetes_X[:-20]
diabetes_X_test = diabetes_X[-20:]

# Split the targets into training/testing sets
diabetes_y_train = diabetes.target[:-20]
diabetes_y_test = diabetes.target[-20:]

# ==========================
# With scikit learn:
# Create linear regression object
regr = linear_model.LinearRegression()

# Train the model using the training sets
regr.fit(diabetes_X_train, diabetes_y_train)

# Make predictions using the testing set
diabetes_y_pred = regr.predict(diabetes_X_test)

# The coefficients
print("scikit Coefficients: \n", regr.coef_)
# The mean squared error
mean_squared_error = metrics.mean_squared_error(diabetes_y_test, diabetes_y_pred)
print("scikit MSE: %.2f" % mean_squared_error)
print("="*80)

# ==========================
# Linear regression as studied in class.
# Train data:
X = diabetes_X_train
y = diabetes_y_train
x0 = np.ones(422) # First Bias
X = np.insert(X,0,x0,1)

# Train initialization
W = np.ones(11)         # Weights - Coefficients
New_W = np.zeros(11)
y_pred = np.zeros(422)       # Predictions

learning_rate = 1.9 # (Weight function seems to explode when learning_rate > 2)
epochs = 20*5000 # Learning time

# Train: Gradient descent
for i in range(epochs):
    # Calculate predictions
    y_pred=np.matmul(X, W) # 422x11 X 11x1 = 422x1

    # Calculate error and cost (mean squared error - use can use the imported function metrics.mean_squared_error)
    Cost = metrics.mean_squared_error(y, y_pred)

    # Calculate gradients
    for j in range(len(W)):
        New_W[j] = W[j] - (learning_rate/422)*np.dot(y_pred-y,X[:,j]) #1x422 X 422x1

    # Update parameters
    W = np.copy(New_W)

# Test:
my_y_test = np.matmul(np.insert(diabetes_X_test,0,np.ones(20),1),W)
Cost = metrics.mean_squared_error(diabetes_y_test, my_y_test)
print("My Coefficients: \n", W[1:])
print("My MSE: ", Cost)