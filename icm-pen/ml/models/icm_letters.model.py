from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense, LSTM, InputLayer
import numpy as np
import pandas as pd
import os

def create_model(features=9, hiddenUnits=100, classes=2):
    model = Sequential()
    model.add(InputLayer(shape=(None, features)))  # Sequence input layer
    model.add(LSTM(hiddenUnits, return_sequences=False))  # LSTM layer
    model.add(Dense(classes, activation='softmax'))  # Fully connected layer

    # Compile the model
    model.compile(loss='sparse_categorical_crossentropy', optimizer='adam', metrics=['accuracy'])
    return model


def load_data(base_paths,data_paths = [], labels = []):
    data = []
    for path in data_paths:
        data.append(pd.read_csv(base_paths+path).values)
    data = np.array(data)
    return data, labels


a_letters_path = os.listdir("data/letters")
a_labaels = [0] * len(a_letters_path)

b_letters_path = os.listdir("data/letters")
b_labels = [1] * len(b_letters_path)

a_data, a_labels = load_data("data/letters", a_letters_path, a_labaels)
null_data, b_labels = load_data("data/letters", b_letters_path, b_labels)

data , labels = np.concatenate((a_data, null_data)), np.concatenate((a_labels, b_labels))

model = create_model()

history = model.fit(data, labels, epochs=20, batch_size=32)

model.save('ml/bin/icm_letters.keras')

print(model.predict(data[-1:]))