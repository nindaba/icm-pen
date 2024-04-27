import os
import pandas as pd
import numpy as np
from keras.models import Sequential
from keras.layers import LSTM, Dense , Input, Flatten, Conv2D 
from keras import optimizers
from keras.preprocessing.sequence import pad_sequences
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestClassifier
import tensorflow as tf
import matplotlib.pyplot as plt



# Load the data

# list all the files in data/A/

valid_files = os.listdir('data/A/valid')
invalid_files = os.listdir('data/A/invalid')

# map by reading test_files to csv 300 rows 9 columns
# Initialize an empty list to store the data
valid = []
invalid = []

# Loop through each file
for file in valid_files:
    filepath = os.path.join('data/A/valid', file)
    df = pd.read_csv(filepath, nrows=299)
    df = pd.DataFrame(df.values.flatten()).T  
    # spectogram = np.fft.fft2(df)
    # spectogram = tf.abs(spectogram)
    # add a label 1 to the data
    valid.append((df, 1))

for file in invalid_files:
    filepath = os.path.join('data/A/invalid', file)
    df = pd.read_csv(filepath, nrows=299)
    df = pd.DataFrame(df.values.flatten()).T 
    df = pd.concat([df, pd.DataFrame([1])], axis=0) 
    # spectogram = np.fft.fft2(df)
    # spectogram = tf.abs(spectogram)
    # add a label 0 to the data
    invalid.append((df, 0))

# Convert the list of dataframes to a single dataframe
data = invalid + valid

#shuffle the data

np.random.shuffle(data)

# Split the data into inputs and targets

x_train = [x[0] for x in data]
y_train = [x[1] for x in data]

# Create model
model = RandomForestClassifier()

# x_train = pad_sequences(x_train, padding='post')
print(x_train)

model.fit(x_train, y_train)

# Train the model
# history = model.fit(data, letters, epochs=100, validation_split=0.2)

# Save the model