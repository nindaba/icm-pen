from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense, LSTM, InputLayer, Conv1D, MaxPooling1D, Flatten
from sklearn.preprocessing import LabelEncoder
import numpy as np
import pandas as pd
import os
import time
from collections import Counter


def create_model(features=9, hiddenUnits=100, classes=2):
    model = Sequential()
    model.add(InputLayer(shape=(None, features)))
    model.add(LSTM(hiddenUnits, return_sequences=False))
    model.add(Dense(classes, activation='softmax'))

    model.compile(loss='sparse_categorical_crossentropy', optimizer='adam', metrics=['accuracy'])
    return model


def create_conv_model(features=9, classes=2):
    model = Sequential()
    model.add(Conv1D(filters=features, activation='relu', kernel_size=3))
    model.add(MaxPooling1D(pool_size=2))
    model.add(Conv1D(filters=features * 2, activation='relu', kernel_size=3))
    model.add(MaxPooling1D(pool_size=2))
    model.add(Conv1D(filters=features * 2, activation='relu', kernel_size=3))
    model.add(MaxPooling1D(pool_size=2))
    model.add(Flatten())
    # model.add(Dense(classes * 2, activation='softmax'))
    model.add(Dense(classes, activation='softmax'))
    model.compile(loss='sparse_categorical_crossentropy', optimizer='adam', metrics=['accuracy'])
    return model


def load_data(base_paths='data/letters', data_paths=[]):
    data = []
    labels = []
    for path in data_paths:
        df = pd.read_csv(os.path.join(base_paths, path))
        letter = path.split('_')[0]
        data.append(df.values)
        labels.append(letter)
    data = np.array(data)
    return data, labels


def train():
    print("Training the model")
    base_paths = "data/letters"
    dataset_path = os.listdir(base_paths)
    np.random.shuffle(dataset_path)
    data, labels = load_data(base_paths=base_paths, data_paths=dataset_path)

    print(Counter(labels))

    le = LabelEncoder()
    labels = le.fit_transform(labels)
    np.save('ml/bin/label_encoder.npy', le.classes_)

    print(data.shape)
    model = create_conv_model(classes=len(le.classes_))
    start = time.time()

    summary = model.fit(data, labels, epochs=30, batch_size=32, validation_split=0.2)

    print(summary.history)
    print(f"Time taken to train the model: {time.time() - start}")

    model.save('ml/bin/icm_letters_v2.maxe.keras')
