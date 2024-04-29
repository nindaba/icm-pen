import pandas as pd
import numpy as np
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense, LSTM, InputLayer
from sklearn.preprocessing import LabelEncoder
from sklearn.model_selection import train_test_split

def create_dataset(X, y, time_steps=1):
    Xr, yr = [], []
    for i in range(len(X) - time_steps):
        v = X.values[i:(i + time_steps)]
        Xr.append(v)
        yr.append(y[i])
    return np.array(Xr), np.array(yr)

TIME_STEPS = 300

# Load the data
data = pd.read_csv('data/digit/hand_written_digits_training.csv')

# Preprocess the data
X = data[['acc_X','acc_Y','acc_Z']]
y = data['digit']

# Encode labels
le = LabelEncoder()
y = le.fit_transform(y)

# Create sequence of windows
X, y = create_dataset(X, y, TIME_STEPS)

# Split the data into training and testing sets
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

numFeatures = 3
numHiddenUnits = 100
numClasses = 10

model = Sequential()
model.add(InputLayer(input_shape=(None, numFeatures)))  # Sequence input layer
model.add(LSTM(numHiddenUnits, return_sequences=False))  # LSTM layer
model.add(Dense(numClasses, activation='softmax'))  # Fully connected layer

# Compile the model
model.compile(loss='sparse_categorical_crossentropy', optimizer='adam', metrics=['accuracy'])

# Train the model
model.fit(X_train, y_train, epochs=10, batch_size=100, validation_data=(X_test, y_test))

# Save the model
model.save('ml/bin/digits.keras')