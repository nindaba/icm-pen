import numpy as np
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import LSTM, Dense, Dropout
from tensorflow.keras.optimizers import Adam

# Load your data
data = np.loadtxt('data/additional/valid.csv', delimiter=',')
# The first two columns are inputs, the third column is the target
inputs = data[:, :2]
target = data[:, 2]

# Reshape inputs to be [samples, time steps, features]
inputs = np.reshape(inputs, (inputs.shape[0], 1, inputs.shape[1]))

# Define the model
model = Sequential()
model.add(LSTM(100, activation='relu', input_shape=(1, 2)))
model.add(Dense(1))

model.compile(optimizer=Adam(), loss='mse')

# Train the model
model.fit(inputs, target, epochs=15, validation_split=0.2)

# Save the model
model.save('ml/bin/additional.keras')