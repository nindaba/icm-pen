from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense, LSTM, InputLayer
from sklearn.preprocessing import LabelEncoder
import numpy as np
import pandas as pd
import os

def create_model(features=9, hiddenUnits=100, classes=2):
    model = Sequential()
    model.add(InputLayer(shape=(None, features)))
    model.add(LSTM(hiddenUnits, return_sequences=False))
    model.add(Dense(classes, activation='softmax'))

    model.compile(loss='sparse_categorical_crossentropy', optimizer='adam', metrics=['accuracy'])
    return model


def load_data(base_paths='data/letters',data_paths = []):
    data = []
    labels = []
    for path in data_paths:
        df = pd.read_csv(os.path.join(base_paths,path))
        data.append(df.values)
        labels.append(path.split('_')[0])
    data = np.array(data)
    return data, labels


dataset_path = os.listdir("data/letters")

data , labels = load_data(data_paths=dataset_path)

el = LabelEncoder()
labels = el.fit_transform(labels)

model = create_model(classes=len(el.classes_))
history = model.fit(data, labels, epochs=20, batch_size=32, validation_split=0.2)

model.save('ml/bin/icm_letters_v2.keras')
