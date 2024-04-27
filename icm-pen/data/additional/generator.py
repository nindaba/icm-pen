import numpy as np

# Create an array of shape (1000, 3) with integer type
data = np.zeros((100000, 3), dtype=np.int8)

for i in range(100000):
    # Generate two random digits
    num1 = np.random.randint(0, 10)
    num2 = np.random.randint(0, 10)

    # Compute the sum
    result = num1 + num2

    # Store the numbers and the result in the data array
    data[i, 0] = num1
    data[i, 1] = num2
    data[i, 2] = result

# Save the data
np.savetxt('data/additional/valid.csv', data, delimiter=',')